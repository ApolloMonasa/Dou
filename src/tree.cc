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

#include <unordered_map>

// Custom hash for tuple key
struct KeyHash {
    std::size_t operator()(const std::tuple<unsigned long long, unsigned long long, unsigned int>& k) const {
        // Simple hash combination
        auto h1 = std::hash<unsigned long long>{}(std::get<0>(k));
        auto h2 = std::hash<unsigned long long>{}(std::get<1>(k));
        auto h3 = std::hash<unsigned int>{}(std::get<2>(k));
        return h1 ^ (h2 << 1) ^ (h3 << 2); 
    }
};

// Memoization table: unordered_map for O(1) average lookup
// Key: <handA, handB, prevMovePacked>
// Value: true if the current player (a) can win
std::unordered_map<std::tuple<unsigned long long, unsigned long long, unsigned int>, bool, KeyHash> memo;

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
unsigned int encodePai(Pai *p) {
    if (!p) return 0;
    return p->encode();
}

// Pure boolean solve function - NO Node allocation
bool solve(int *a, int *b, Pai *p) {
    if (checkEmpty(b)) return false; // Opponent (b) already won (previous turn), so current player (a) loses.
    
    // Check memoization
    unsigned long long ha = encodeHand(a);
    unsigned long long hb = encodeHand(b);
    unsigned int hp = encodePai(p);
    
    auto key = std::make_tuple(ha, hb, hp);
    
    if (memo.count(key)) {
        return memo[key];
    }

    // Generate legal moves
    vector<Pai *> moves = Pai::getLegalPai(a, p);
    
    bool canWin = false;
    for (size_t i = 0; i < moves.size(); ++i) {
        // If we already found a winning move, just clean up remaining pointers
        if (canWin) {
            delete moves[i];
            continue;
        }

        moves[i]->take(a);
        bool oppWin = solve(b, a, moves[i]);
        moves[i]->back(a);
        
        // Clean up immediately
        delete moves[i];

        if (!oppWin) {
            canWin = true;
            // Don't break here, let the loop continue to delete remaining items
        }
    }
    
    // Store result
    memo[key] = canWin;
    return canWin;
}

void getTree(Node *root, int *a, int *b) {
    if (checkEmpty(b)) {
        root->win = false;
        return ;
    }
    
    // Use solve() to get the result of the root state quickly
    // Note: getTree is now mostly a wrapper to populate the first level of children
    // OR we can just use solve() recursively?
    // The main.cpp expects a tree structure for at least the current level to display options.
    // But deeper levels don't need to be instantiated.
    // So getTree should populate children of root, and calculate their win status using solve().
    
    // Check memo first? solve() does that.
    
    vector<Pai *> t = Pai::getLegalPai(a, root->p);
    for (int i = 0; i < t.size(); i++) {
        Node *node = new Node(t[i], 0);
        t[i]->take(a);
        
        // Use fast solve instead of recursive getTree
        // If solve returns true (current player 'b' wins), then node->win is true.
        // Wait, solve(b, a, t[i]) returns true if 'b' (next player) can win.
        // If 'b' can win, then for 'a', this move leads to a loss.
        // So node->win (which usually means "does this move lead to a win for the player who made it?")
        // In original code: node->win = 0 initialized. 
        // Then getTree(node, b, a). If node->win (for b) is false, then root->win (for a) is true.
        // Let's stick to definition: solve returns true if CURRENT player wins.
        // Here we call solve(b, a, ...), so it returns true if B wins.
        bool bWins = solve(b, a, t[i]);
        node->win = bWins; // This records if the NEXT player wins from this state.
        
        t[i]->back(a);
        root->child.push_back(node);
        
        if (node->win == false) { // If B cannot win, then A wins.
            root->win = true;
            // We found a winning move for A. 
            // We should NOT break here if we want to list ALL options for the user.
            // But for pure solving, we break.
            // The original getTree was breaking.
            // But wait, if we break, we don't generate other children.
            // main.cpp handles lazy expansion, so maybe it's fine?
            // Yes, lazy expansion handles it.
            break;
        }
    }
}