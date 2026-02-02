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
// Key: <handA, handB, prevMovePacked>
// Value: true if the current player (a) can win
std::map<std::tuple<unsigned long long, unsigned long long, unsigned int>, bool> memo;

// Compress 15 card counts (3-17) into a uint64_t
// Each card count takes 3 bits (0-4)
unsigned long long encodeHand(int *arr) {
    unsigned long long code = 0;
    for (int i = 3; i < MAX_N; i++) {
        code |= ((unsigned long long)arr[i] & 0x7) << ((i - 3) * 3);
    }
    return code;
}

// Compress Pai info into uint32_t
// Type: 4 bits (0-15)
// Value/Head: 5 bits (0-31)
// Length: 4 bits (0-15)
// Extra: 9 bits (type+val for attachment)
unsigned int encodePai(Pai *p) {
    if (!p) return 0;
    unsigned int code = 0;
    code |= ((unsigned int)p->type & 0xF); // 0-3
    
    int val = 0;
    int len = 0;
    int extra = 0;

    // Extract details based on type
    return p->encode();
}


// Helper to serialize Pai for better memoization (optional, but let's stick to PASS for now)

void getTree(Node *root, int *a, int *b) {
    if (checkEmpty(b)) {
        root->win = false;
        return ;
    }
    
    // Check memoization
    // Use tuple key for O(logN) lookup with compressed data
    unsigned long long ha = encodeHand(a);
    unsigned long long hb = encodeHand(b);
    unsigned int hp = encodePai(root->p);
    
    auto key = std::make_tuple(ha, hb, hp);
    
    if (memo.count(key)) {
        root->win = memo[key];
        return;
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
    memo[key] = root->win;
}