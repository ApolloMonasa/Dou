#include <cstdio>
#include <iostream>
#include <stack>
#include "./include/tree.h"
using namespace std;

int a[MAX_N + 5];
int b[MAX_N + 5];

void read(FILE *fin, int *arr) {
    int x;
    while (fscanf(fin, "%d", &x) != EOF) {
        if (x == 0) break;
        arr[x]++;
    }
}

void read_data() {
    FILE *fin = fopen("input.txt", "r");
    read(fin, a);
    read(fin, b);
    fclose(fin);
}


void output_solution(Node *root, int *a, int *b) {
    stack<Node *> st;
    st.push(root);
    while (!st.empty()) {
        if (checkEmpty(a)) {
            printf("A Wins!\n");
            return;
        }
        if (checkEmpty(b)) {
            printf("B Wins!\n");
            return;
        }
        printf("%s : ", st.size() % 2 ? "-->" : "   "); Pai::output_arr(a);
        printf("%s : ", st.size() % 2 ? "   " : "-->"); Pai::output_arr(b);
        Node *node = st.top();
        
        // Lazy expansion: If node has no children but game is not over, generate them.
        // This handles nodes that were skipped during initial tree building due to memoization.
        int *curr_hand = st.size() % 2 ? a : b;
        int *opp_hand = st.size() % 2 ? b : a;
        
        if (node->child.empty() && !checkEmpty(opp_hand)) { // Check opp_hand because checkEmpty checks if PREVIOUS player won
             vector<Pai *> t = Pai::getLegalPai(curr_hand, node->p);
             for (auto p : t) {
                 Node *child = new Node(p, 0);
                 p->take(curr_hand);
                 getTree(child, opp_hand, curr_hand);
                 p->back(curr_hand);
                 node->child.push_back(child);
             }
        }

        int no;
        do {
            printf("[%3d] : back\n", -1);
            for (int i = 0; i < node->child.size(); i++) {
                printf("[%3d] : [%d]", i, node->child[i]->win);
                node->child[i]->p->output() << endl;
            }
            cout << "INPUT : ";
            cin >> no;
            if (no == -1) break;
            if (no >= 0 && no < (int)node->child.size()) break;
        } while(1);
        if (no == -1) {
            st.pop();
            node->p->back(st.size()%2?a:b);
        }
        else {
            node->child[no]->p->take(st.size()%2?a:b);
            st.push(node->child[no]);
        }
    }
}

int main(){
    read_data();
    cout << "read data done ......" << endl;
    Node *rt = new Node;
    cout << "analysis start ......" << endl;
    getTree(rt, a, b);
    cout << "analysis done  ......" << endl;
    output_solution(rt, a, b);    
    return 0;
}