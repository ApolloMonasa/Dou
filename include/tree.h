#pragma once
#include <vector>
#include <map>
#include <string>
#include "pai.h"


class Node {
public:
    Node();
    Node(Pai *, bool);
    bool win;
    Pai *p;
    vector<Node *> child;
};

void getTree(Node *, int *, int *);

bool checkEmpty(int *);