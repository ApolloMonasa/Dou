/**
 * @file main.cpp
 * @brief 程序入口与交互界面
 * 
 * 负责读取输入、初始化博弈树搜索、并提供命令行交互界面供用户浏览博弈过程。
 */

#include <cstdio>
#include <iostream>
#include <stack>
#include "./include/tree.h"
using namespace std;

int a[MAX_N + 5]; ///< 玩家A的手牌计数数组
int b[MAX_N + 5]; ///< 玩家B的手牌计数数组

/**
 * @brief 从文件读取手牌数据
 * @param fin 文件指针
 * @param arr 目标数组
 */
void read(FILE *fin, int *arr) {
    int x;
    while (fscanf(fin, "%d", &x) != EOF) {
        if (x == 0) break;
        arr[x]++;
    }
}

/**
 * @brief 读取所有数据
 * 
 * 默认读取 "input.txt" 文件。
 * 第一行为玩家A的手牌，以0结束。
 * 第二行为玩家B的手牌，以0结束。
 */
void read_data() {
    FILE *fin = fopen("input.txt", "r");
    if (!fin) {
        // 如果找不到文件，尝试读取标准输入或者提示用户
        // 这里简单地回退到标准输入读取，方便测试
        cout << "input.txt not found, reading from stdin..." << endl;
        fin = stdin;
    }
    read(fin, a);
    read(fin, b);
    if (fin != stdin) fclose(fin);
}

/**
 * @brief 交互式输出解决方案
 * 
 * 允许用户通过命令行输入选择分支，模拟对局过程。
 * 
 * @param root 博弈树根节点
 * @param a 初始玩家A手牌
 * @param b 初始玩家B手牌
 */
void output_solution(Node *root, int *a, int *b) {
    stack<Node *> st;
    st.push(root);
    
    // st.size() 的奇偶性决定当前是谁的回合
    // 初始 size=1 (root)，对应 A 出牌。
    // A 出牌后 push child，size=2，对应 B 出牌。
    // 所以：奇数=A出牌，偶数=B出牌。
    
    while (!st.empty()) {
        if (checkEmpty(a)) {
            printf("A Wins!\n");
            return;
        }
        if (checkEmpty(b)) {
            printf("B Wins!\n");
            return;
        }
        
        // 打印当前双方手牌
        printf("%s : ", st.size() % 2 ? "-->" : "   "); Pai::output_arr(a);
        printf("%s : ", st.size() % 2 ? "   " : "-->"); Pai::output_arr(b);
        
        Node *node = st.top();
        
        // 延迟展开 (Lazy Expansion):
        // 如果当前节点没有子节点（可能是因为 Memoization 跳过了生成，或者刚走到这一步），
        // 且游戏未结束，则现场生成子节点。
        int *curr_hand = st.size() % 2 ? a : b;
        int *opp_hand = st.size() % 2 ? b : a;
        
        // 注意：checkEmpty(opp_hand) 是检查上一个出牌的人是否赢了。
        // 如果上一个人打完牌，opp_hand 为空，这里不应该进入（前面的 checkEmpty 应该已经拦截）。
        
        if (node->child.empty()) { 
             vector<Pai *> t = Pai::getLegalPai(curr_hand, node->p);
             for (auto p : t) {
                 Node *child = new Node(p, 0);
                 p->take(curr_hand);
                 // 调用 getTree 来计算子节点的胜负状态 (内部使用 solve 快速计算)
                 // getTree(child, next_player, current_player)
                 getTree(child, opp_hand, curr_hand);
                 p->back(curr_hand);
                 node->child.push_back(child);
             }
        }

        // 用户交互循环
        int no;
        do {
            printf("[%3d] : back\n", -1);
            for (int i = 0; i < node->child.size(); i++) {
                // 显示选项：[序号] : [胜负状态] 牌型
                // win=1 表示该分支对当前出牌者是有利的（即走出这一步后，对手无法必胜？）
                // 在 tree.cc 中定义：node->win = solve(opp, curr, p)。即对手是否必胜。
                // 如果 node->win 为 true，表示对手必胜 -> 对我不利。
                // 如果 node->win 为 false，表示对手无法必胜 -> 对我有利。
                // 所以显示 [0] 是好棋，[1] 是坏棋。
                printf("[%3d] : [%d]", i, node->child[i]->win);
                node->child[i]->p->output() << endl;
            }
            cout << "INPUT : ";
            if (!(cin >> no)) { // 处理输入错误或 EOF
                return;
            }
            
            if (no == -1) break;
            if (no >= 0 && no < (int)node->child.size()) break;
            cout << "Invalid input!" << endl;
        } while(1);
        
        if (no == -1) {
            // 回退
            st.pop();
            // 恢复手牌：谁刚出的牌，谁收回去
            // pop 后栈顶变了。
            // 原栈顶是 node。node->p 是上一步打出的牌（导致进入 node）。
            // 这一步是谁打的？是导致 node 入栈的那个人。
            // 如果当前是 A 出牌 (size奇数)，那么 node 是上一步 B 打出的结果。
            // 这里的逻辑稍微有点绕：
            // node 存储的是“上家出的牌”。
            // st.push(root) -> root->p 是 PASS。
            // 循环开始，A 面对 PASS 出牌。
            // A 选择 child[no]。child[no]->p 是 A 出的牌。
            // st.push(child[no])。
            // 下一轮，B 面对 A 出的牌。
            
            // 当我们 pop 时，例如从 size=2 (B出牌) 回退到 size=1 (A出牌)。
            // 我们需要撤销 A 出的牌。
            // A 出的牌存储在当前栈顶 (pop前的栈顶) 的 p 中。
            // 但这里 st.pop() 已经执行了。node 变量还保留着原栈顶。
            // node->p 就是 A 出的牌。
            // A 是谁？ pop 后 size=1 (A)。
            // 或者用 pop 前的 size=2 (偶数) -> 上一步是 A (奇数)。
            // 所以：st.size() (pop后) % 2 ? a : b 是当前玩家。
            // 上一步玩家是 opp_hand。
            // 这里的逻辑：
            // if (no == -1) st.pop(); node->p->back(...)
            // node 是 pop 出来的节点。
            // 这个节点是谁打出的？
            // 根节点 (root) 是没人打出的 (PASS)。back 无操作。
            // 子节点是某人打出的。
            // 如果 pop 后 size=1。说明刚才 pop 掉的是 size=2 的节点。
            // size=2 的节点是 A 打出的牌形成的局面。
            // 所以应该把牌加回给 A。
            // A 对应 size=1。
            // 所以 back 的目标是 st.size()%2 ? a : b。
            node->p->back(st.size() % 2 ? a : b);
        }
        else {
            // 前进
            // 扣除手牌
            // 当前是谁出牌？ size % 2 ? a : b。
            node->child[no]->p->take(st.size() % 2 ? a : b);
            st.push(node->child[no]);
        }
    }
}

int main(){
    read_data();
    cout << "read data done ......" << endl;
    
    Node *rt = new Node;
    cout << "analysis start ......" << endl;
    
    // 初始分析：计算根节点的胜负状态
    getTree(rt, a, b);
    
    cout << "analysis done  ......" << endl;
    
    output_solution(rt, a, b);    
    return 0;
}