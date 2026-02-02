#pragma once
#include <vector>
#include <map>
#include <string>
#include <tuple>
#include "pai.h"


/**
 * @brief 博弈树节点
 * 
 * 记录了当前的胜负状态、上一步打出的牌以及后续分支。
 */
class Node {
public:
    Node();
    /**
     * @brief 构造函数
     * @param p 上一步打出的牌
     * @param win 当前节点代表的玩家是否必胜
     */
    Node(Pai *p, bool win);
    
    bool win;           ///< 当前节点胜负状态 (true=必胜, false=必败)
    Pai *p;             ///< 到达此节点所打出的牌 (上家出的牌)
    vector<Node *> child; ///< 后续可能的走法分支
};

/**
 * @brief 构建/搜索博弈树
 * 
 * 使用 Min-Max 算法和记忆化搜索来判定当前局面的胜负。
 * 
 * @param root 当前根节点
 * @param a 当前玩家手牌 (轮到谁出牌)
 * @param b 对手玩家手牌
 */
void getTree(Node *root, int *a, int *b);

/**
 * @brief 检查手牌是否为空
 * @param arr 手牌数组
 * @return 如果手牌为空（打完），返回 true
 */
bool checkEmpty(int *arr);
