#pragma once

#include <iostream>
#include <ostream>
#include <string>
#include <vector>
using namespace std;
#define MAX_N 18

enum class PaiType {
    DAN_T,
    DUIZI_T,
    SHUNZI_T,
    LIANDUI_T,
    SANDAI_T,
    ZHADAN_T,
    WANGZHA_T,
    PASS_T,
};

///@brief 出牌方式
class Pai {
public:
    Pai(PaiType pt);
    static vector<Pai *> getPai(int *arr);
    static vector<Pai *> getLegalPai(int *arr, Pai *pai);
    static string getName(int code);
    static void output_arr(int *arr);
    virtual ostream& output() = 0;
    virtual bool operator>(Pai *pre) = 0;
    virtual ~Pai() = default;
    virtual void take(int *) = 0;
    virtual void back(int *) = 0;
    PaiType type;
};


class PASS : public Pai {
public:
    PASS();
    static vector<Pai *> get(int *arr);
    ostream& output() override;
    bool operator>(Pai *pre) override;
    void take(int *) override;
    void back(int *) override;
};

class DAN : public Pai {
    /// 打的是哪张单牌
    int x;
public:
    DAN(int x);
    static vector<Pai *> get(int *arr);
    ostream& output() override;
    bool operator>(Pai *pre) override;
    void take(int *) override;
    void back(int *) override;
};

class DUIZI : public Pai {
    /// 打的是哪张对子
    int x;
public:
    DUIZI(int x);
    static vector<Pai *> get(int *arr);
    ostream& output() override;
    bool operator>(Pai *pre) override;
    void take(int *) override;
    void back(int *) override;
};

class SHUNZI : public Pai {
    /// 起点和长度
    int head, length;
public:
    SHUNZI(int head, int length);
    static vector<Pai *> get(int *arr);
    ostream& output() override;
    bool operator>(Pai *pre) override;
    void take(int *) override;
    void back(int *) override;
};

class SANDAI : public Pai {
    /// 3x带?
    int x;
    Pai *dai;
public:
    SANDAI(int x, Pai *dai);
    static vector<Pai *> get(int *arr);
    ostream& output() override;
    bool operator>(Pai *pre) override;
    void take(int *) override;
    void back(int *) override;
};

class ZHADAN : public Pai {
    /// 打4x
    int x;
public:
    ZHADAN(int x);
    static vector<Pai *> get(int *arr);
    ostream& output() override;
    bool operator>(Pai *pre) override;
    void take(int *) override;
    void back(int *) override;
};

class WANGZHA : public Pai {
public:
    WANGZHA();
    static vector<Pai *> get(int *arr);
    ostream& output() override;
    bool operator>(Pai *pre) override;
    void take(int *) override;
    void back(int *) override;
};

class LIANDUI : public Pai {
    /// 起点和长度
    int head, length;
public:
    LIANDUI(int head, int length);
    static vector<Pai *> get(int *arr);
    ostream& output() override;
    bool operator>(Pai *pre) override;
    void take(int *) override;
    void back(int *) override;
};

