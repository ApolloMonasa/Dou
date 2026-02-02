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

// Memoization table
// Key: encoded state string
// Value: true if the current player (whose turn it is to play from 'a') can win
std::map<std::string, bool> memo;

std::string hashState(int *a, int *b, Pai *p) {
    std::string s = "";
    // Encode hand a
    for (int i = 3; i < MAX_N; i++) s += std::to_string(a[i]) + ",";
    s += "|";
    // Encode hand b
    for (int i = 3; i < MAX_N; i++) s += std::to_string(b[i]) + ",";
    s += "|";
    // Encode previous move (Pai type and value)
    if (p) {
        s += std::to_string((int)p->type) + ",";
        
        if (p->type == PaiType::PASS_T) s += "PASS";
        else {
             s += "PASS_CHECK";
             return "";
        }
    }
    return s;
}

// Helper to serialize Pai for better memoization (optional, but let's stick to PASS for now)

void getTree(Node *root, int *a, int *b) {
    if (checkEmpty(b)) {
        root->win = false;
        return ;
    }
    
    // Check memoization
    std::string stateKey = "";
    // We only memoize if the previous move was PASS (free turn)
    // This is a safe subset of states to memoize without complex Pai serialization
    if (root->p->type == PaiType::PASS_T) {
        stateKey = hashState(a, b, root->p);
        if (!stateKey.empty() && memo.count(stateKey)) {
            root->win = memo[stateKey];
            return;
        }
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
    
    // Store result in memo
    if (!stateKey.empty()) {
        memo[stateKey] = root->win;
    }
}