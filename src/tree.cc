/**
 * @file tree.cc
 * @brief 博弈树搜索算法实现
 * 
 * 包含 Min-Max 搜索算法、Alpha-Beta 剪枝优化（隐含在循环中断中）、
 * 状态压缩与哈希表记忆化（Memoization）实现。
 */

#include "../include/pai.h"
#include "../include/tree.h"
#include <vector>
#include <unordered_map>

/**
 * @brief 检查手牌是否已打完
 * @param a 手牌数组
 * @return true 如果已空
 */
bool checkEmpty(int *a) {
    for (int i = 3; i < MAX_N; i++) {
        if (a[i]) return false;
    }
    return true;
}

Node::Node() : win(false), p(new PASS()) {}

Node::Node(Pai *p, bool win) : p(p), win(win) {}

// ==========================================
// 状态压缩与哈希表 (State Compression & Hash)
// ==========================================

/**
 * @brief 自定义哈希函数，用于 unordered_map 的 Key (std::tuple)
 */
struct KeyHash {
    std::size_t operator()(const std::tuple<unsigned long long, unsigned long long, unsigned int>& k) const {
        // 简单的异或哈希组合
        auto h1 = std::hash<unsigned long long>{}(std::get<0>(k));
        auto h2 = std::hash<unsigned long long>{}(std::get<1>(k));
        auto h3 = std::hash<unsigned int>{}(std::get<2>(k));
        return h1 ^ (h2 << 1) ^ (h3 << 2); 
    }
};

/**
 * @brief 全局记忆化表 (Memoization Table)
 * 
 * Key: <我方手牌(压缩), 敌方手牌(压缩), 上家出牌(压缩)>
 * Value: 当前玩家是否必胜 (true=必胜)
 */
std::unordered_map<std::tuple<unsigned long long, unsigned long long, unsigned int>, bool, KeyHash> memo;

/**
 * @brief 手牌压缩函数
 * 
 * 将 15 种牌（3-大王）的数量（每种0-4张）压缩到一个 uint64_t 中。
 * 每种牌占用 3 bits (2^3=8 > 4)。
 * 
 * @param arr 手牌数组
 * @return 压缩后的 64 位整数
 */
unsigned long long encodeHand(int *arr) {
    unsigned long long code = 0;
    for (int i = 3; i < MAX_N; i++) {
        code |= ((unsigned long long)arr[i] & 0x7) << ((i - 3) * 3);
    }
    return code;
}

/**
 * @brief 牌型压缩函数
 * @param p 牌型对象
 * @return 压缩后的 32 位整数
 */
unsigned int encodePai(Pai *p) {
    if (!p) return 0;
    return p->encode();
}

// ==========================================
// 核心求解算法 (Solver)
// ==========================================

/**
 * @brief 快速求解函数 (Zero-Allocation Solver)
 * 
 * 纯递归函数，不创建 Node 对象，用于快速判定胜负。
 * 
 * @param a 当前玩家手牌
 * @param b 对手玩家手牌
 * @param p 上家打出的牌
 * @return true 如果当前玩家(a)必胜
 */
bool solve(int *a, int *b, Pai *p) {
    // 基础情况：如果对手已经打完牌，那么我方输了（上一轮对手赢了）
    if (checkEmpty(b)) return false; 
    
    // 1. 查表 (Memoization Lookup)
    unsigned long long ha = encodeHand(a);
    unsigned long long hb = encodeHand(b);
    unsigned int hp = encodePai(p);
    
    auto key = std::make_tuple(ha, hb, hp);
    
    if (memo.count(key)) {
        return memo[key];
    }

    // 2. 生成所有合法走法
    vector<Pai *> moves = Pai::getLegalPai(a, p);
    
    bool canWin = false;
    for (size_t i = 0; i < moves.size(); ++i) {
        // 如果已经找到必胜路径，后续只需清理内存
        if (canWin) {
            delete moves[i];
            continue;
        }

        // 3. 递归搜索 (Min-Max)
        moves[i]->take(a);
        // 交换角色：solve(对手, 我, 我出的牌)
        // 如果对手必输 (!oppWin)，则我必胜
        bool oppWin = solve(b, a, moves[i]);
        moves[i]->back(a);
        
        delete moves[i]; // 立即释放内存

        if (!oppWin) {
            canWin = true;
            // 找到赢法后不立即 break，而是继续循环以清理剩余 moves 的内存
        }
    }
    
    // 4. 存表 (Store Result)
    memo[key] = canWin;
    return canWin;
}

/**
 * @brief 构建第一层博弈树节点
 * 
 * 用于 UI 显示当前可选的走法。
 * 内部调用 solve() 快速计算子节点的胜负状态。
 */
void getTree(Node *root, int *a, int *b) {
    if (checkEmpty(b)) {
        root->win = false;
        return ;
    }
    
    vector<Pai *> t = Pai::getLegalPai(a, root->p);
    for (int i = 0; i < t.size(); i++) {
        Node *node = new Node(t[i], 0);
        t[i]->take(a);
        
        // 使用 solve 快速判断子节点状态
        // solve(b, a, t[i]) 返回 true 表示 B 必胜
        // 如果 B 必胜，则对于 A 来说 node->win 是 false（但这通常记录的是该节点代表的局面是否对当前出牌者有利？）
        // 这里定义 node->win 为：如果走到该节点（即 A 出了 t[i] 后），接下来的玩家（B）能否必胜。
        bool bWins = solve(b, a, t[i]);
        node->win = bWins; 
        
        t[i]->back(a);
        root->child.push_back(node);
        
        // 如果发现有一步能让 B 必败（即 node->win == false），则 A 必胜
        if (node->win == false) { 
            root->win = true;
            // 找到必胜走法后，可以提前结束生成（如果只需要胜负结果）
            // 但为了 UI 体验，我们可以选择生成所有走法，或者只生成这一步
            // 这里保留 break 逻辑以加快响应，但注意这会导致 UI 只显示部分选项
            // 如果想显示所有选项，应移除 break。
            // 考虑到这是为了“展示最优解”，break 是合理的。
            break;
        }
    }
}