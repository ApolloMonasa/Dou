/**
 * @file main.cpp
 * @brief 程序入口与交互界面
 * 
 * 负责读取输入、初始化博弈树搜索、并提供命令行交互界面供用户浏览博弈过程。
 */

#include <cstdio>
#include <iostream>
#include <stack>
#include <sstream>
#include <cstring>
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
        // cout << "input.txt not found, reading from stdin..." << endl;
        fin = stdin;
    }
    read(fin, a);
    read(fin, b);
    if (fin != stdin) fclose(fin);
}

// Helper to escape JSON strings
string json_escape(const string& s) {
    string ret = "";
    for (char c : s) {
        if (c == '"') ret += "\\\"";
        else if (c == '\\') ret += "\\\\";
        else ret += c;
    }
    return ret;
}

/**
 * @brief 交互式输出解决方案
 * 
 * 允许用户通过命令行输入选择分支，模拟对局过程。
 * 
 * @param root 博弈树根节点
 * @param a 初始玩家A手牌
 * @param b 初始玩家B手牌
 * @param jsonMode 是否为 JSON 模式
 */
void output_solution(Node *root, int *a, int *b, bool jsonMode) {
    stack<Node *> st;
    st.push(root);
    
    // st.size() 的奇偶性决定当前是谁的回合
    // 初始 size=1 (root)，对应 A 出牌。
    // A 出牌后 push child，size=2，对应 B 出牌。
    // 所以：奇数=A出牌，偶数=B出牌。
    
    while (!st.empty()) {
        if (!jsonMode) {
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
        }
        
        Node *node = st.top();
        
        // 延迟展开 (Lazy Expansion):
        // 如果当前节点没有子节点（可能是因为 Memoization 跳过了生成，或者刚走到这一步），
        // 且游戏未结束，则现场生成子节点。
        int *curr_hand = st.size() % 2 ? a : b;
        int *opp_hand = st.size() % 2 ? b : a;
        
        // 如果手牌为空，说明上一手牌打完就赢了
        bool isWin = checkEmpty(opp_hand); // opp_hand 是刚出完牌的人
        
        // 如果游戏没结束且需要展开
        if (!isWin && node->child.empty()) { 
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

        if (jsonMode) {
            // JSON Output Format
            /*
            {
                "turn": "A" or "B",
                "hand_a": [3, 3, 4, ...],
                "hand_b": [7, ...],
                "game_over": true/false,
                "winner": "A" or "B" or null,
                "options": [
                    { "id": 0, "desc": "DAN 3", "win": false },
                    ...
                ]
            }
            */
            cout << "{";
            cout << "\"turn\": \"" << (st.size() % 2 ? "A" : "B") << "\",";
            
            // Output Hands
            auto printHand = [](int *arr) {
                cout << "[";
                bool first = true;
                for (int i = 3; i < MAX_N; i++) {
                    for (int k = 0; k < arr[i]; k++) {
                        if (!first) cout << ",";
                        cout << i;
                        first = false;
                    }
                }
                cout << "]";
            };
            
            cout << "\"hand_a\": "; printHand(a); cout << ",";
            cout << "\"hand_b\": "; printHand(b); cout << ",";
            
            bool aWin = checkEmpty(a);
            bool bWin = checkEmpty(b);
            
            if (aWin) {
                cout << "\"game_over\": true, \"winner\": \"A\", \"options\": []";
            } else if (bWin) {
                cout << "\"game_over\": true, \"winner\": \"B\", \"options\": []";
            } else {
                cout << "\"game_over\": false, \"winner\": null, \"options\": [";
                for (int i = 0; i < node->child.size(); i++) {
                    if (i > 0) cout << ",";
                    // Capture output from Pai::output() to string
                    stringstream ss;
                    streambuf* old_buf = cout.rdbuf(ss.rdbuf());
                    node->child[i]->p->output();
                    cout.rdbuf(old_buf);
                    
                    cout << "{\"id\": " << i << ", \"desc\": \"" << json_escape(ss.str()) << "\", \"win\": " << (node->child[i]->win ? "true" : "false") << "}";
                }
                cout << "]";
            }
            cout << "}" << endl; // End of JSON line, flush
        }

        // 用户交互循环
        int no;
        do {
            if (!jsonMode) {
                printf("[%3d] : back\n", -1);
                for (int i = 0; i < node->child.size(); i++) {
                    printf("[%3d] : [%d]", i, node->child[i]->win);
                    node->child[i]->p->output() << endl;
                }
                cout << "INPUT : ";
            }
            
            if (!(cin >> no)) { // 处理输入错误或 EOF
                return;
            }
            
            if (no == -1) break;
            if (no >= 0 && no < (int)node->child.size()) break;
            if (!jsonMode) cout << "Invalid input!" << endl;
        } while(1);
        
        if (no == -1) {
            // 回退
            if (st.size() > 1) { // 根节点不能回退
                st.pop();
                node->p->back(st.size() % 2 ? a : b);
            }
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

int main(int argc, char** argv){
    bool jsonMode = false;
    if (argc > 1 && strcmp(argv[1], "--json") == 0) {
        jsonMode = true;
    }

    read_data();
    if (!jsonMode) cout << "read data done ......" << endl;
    
    Node *rt = new Node;
    if (!jsonMode) cout << "analysis start ......" << endl;
    
    // 初始分析：计算根节点的胜负状态
    getTree(rt, a, b);
    
    if (!jsonMode) cout << "analysis done  ......" << endl;
    
    output_solution(rt, a, b, jsonMode);    
    return 0;
}