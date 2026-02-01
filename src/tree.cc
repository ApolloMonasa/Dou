#include "../include/pai.h"
#include "../include/tree.h"
#include <vector>

bool checkEmpty(int *a) {
    for (int i = 3; i < MAX_N; i++) {
        if (a[i]) return false;
    }
    return true;
}

Node::Node() : win(false), p(new PASS()) {}

Node::Node(Pai *p, bool win) : p(p), win(win) {}
void getTree(Node *root, int *a, int *b) {
    if (checkEmpty(b)) {
        root->win = false;
        return ;
    }
    vector<Pai *> t = Pai::getLegalPai(a, root->p);
    for (int i = 0; i < t.size(); i++) {
        Node *node = new Node(t[i], 0);
        t[i]->take(a);
        getTree(node, b, a);
        t[i]->back(a);
        root->child.push_back(node);
        if (node->win == false) {
            root->win = true;
            break;
        }
    }
}