#pragma once

#include <iostream>
#include <ostream>
#include <string>
#include <vector>
using namespace std;
#define MAX_N 18

/**
 * @brief 牌型枚举
 * 
 * 定义了斗地主中所有支持的牌型
 */
enum class PaiType {
    DAN_T,      ///< 单张 (Single)
    DUIZI_T,    ///< 对子 (Pair)
    SHUNZI_T,   ///< 顺子 (Straight)
    LIANDUI_T,  ///< 连对 (Consecutive Pairs)
    SIDAIER_T,  ///< 四带二 (Four with Two)
    FEIJI_T,    ///< 飞机 (Aircraft)
    SANDAI_T,   ///< 三带 (Three with One/Pair)
    ZHADAN_T,   ///< 炸弹 (Bomb)
    WANGZHA_T,  ///< 王炸 (Rocket)
    PASS_T,     ///< 不出 (Pass)
};

/**
 * @brief 牌型基类
 * 
 * 所有具体牌型都继承自此类。
 * 提供了出牌、回溯、比大小、编码等虚接口。
 */
class Pai {
public:
    /**
     * @brief 构造函数
     * @param pt 牌型类型
     */
    Pai(PaiType pt);

    /**
     * @brief 获取当前手牌中所有可能的合法牌型
     * @param arr 当前手牌数组（索引3-17代表点数3-大王）
     * @return 包含所有可能牌型的指针向量
     */
    static vector<Pai *> getPai(int *arr);

    /**
     * @brief 获取能够压制上家牌的所有合法牌型
     * @param arr 当前手牌数组
     * @param pai 上家打出的牌（如果上家PASS，则可以出任意牌）
     * @return 合法出牌列表
     */
    static vector<Pai *> getLegalPai(int *arr, Pai *pai);

    /**
     * @brief 获取牌面名称
     * @param code 牌的点数 (3-17)
     * @return 牌面字符串 (如 "Ace", "2", "King")
     */
    static string getName(int code);

    /**
     * @brief 打印手牌数组
     * @param arr 手牌数组
     */
    static void output_arr(int *arr);

    /**
     * @brief 打印牌型信息 (纯虚函数)
     */
    virtual ostream& output() = 0;

    /**
     * @brief 比较牌型大小 (纯虚函数)
     * @param pre 上家出的牌
     * @return 如果当前牌大于上家牌，返回 true
     */
    virtual bool operator>(Pai *pre) = 0;

    virtual ~Pai() = default;

    /**
     * @brief 从手牌中扣除当前牌型 (纯虚函数)
     * @param arr 手牌数组
     */
    virtual void take(int *) = 0;

    /**
     * @brief 将当前牌型加回手牌 (纯虚函数)
     * @param arr 手牌数组
     */
    virtual void back(int *) = 0;

    /**
     * @brief 编码牌型为整数 (用于状态压缩)
     * @return 32位无符号整数，包含类型和关键参数
     */
    virtual unsigned int encode() { return 0; }

    PaiType type; ///< 牌型类型
};

/**
 * @brief 不出 (Pass)
 */
class PASS : public Pai {
public:
    PASS();
    static vector<Pai *> get(int *arr);
    ostream& output() override;
    bool operator>(Pai *pre) override;
    void take(int *) override;
    void back(int *) override;
    unsigned int encode() override { return (unsigned int)PaiType::PASS_T; }
};

/**
 * @brief 单张 (Single)
 */
class DAN : public Pai {
    int x; ///< 牌的点数
public:
    DAN(int x);
    static vector<Pai *> get(int *arr);
    ostream& output() override;
    bool operator>(Pai *pre) override;
    void take(int *) override;
    void back(int *) override;
    unsigned int encode() override { 
        return ((unsigned int)PaiType::DAN_T) | (x << 4);
    }
};

/**
 * @brief 对子 (Pair)
 */
class DUIZI : public Pai {
    int x; ///< 对子的点数
public:
    DUIZI(int x);
    static vector<Pai *> get(int *arr);
    ostream& output() override;
    bool operator>(Pai *pre) override;
    void take(int *) override;
    void back(int *) override;
    unsigned int encode() override {
        return ((unsigned int)PaiType::DUIZI_T) | (x << 4);
    }
};

/**
 * @brief 顺子 (Straight)
 * 
 * 连续的单张，至少5张。
 */
class SHUNZI : public Pai {
    int head;   ///< 起点点数
    int length; ///< 长度
public:
    SHUNZI(int head, int length);
    static vector<Pai *> get(int *arr);
    ostream& output() override;
    bool operator>(Pai *pre) override;
    void take(int *) override;
    void back(int *) override;
    unsigned int encode() override {
        return ((unsigned int)PaiType::SHUNZI_T) | (head << 4) | (length << 9);
    }
};

/**
 * @brief 三带 (Three with One/Pair)
 * 
 * 三张一样的牌，可以带一张单牌或一对。
 */
class SANDAI : public Pai {
    int x;     ///< 三张的主值
    Pai *dai;  ///< 带的牌 (PASS, DAN, DUIZI)
public:
    SANDAI(int x, Pai *dai);
    static vector<Pai *> get(int *arr);
    ostream& output() override;
    bool operator>(Pai *pre) override;
    void take(int *) override;
    void back(int *) override;
    unsigned int encode() override {
        unsigned int dai_code = dai->encode();
        return ((unsigned int)PaiType::SANDAI_T) | (x << 4) | (dai_code << 9);
    }
};

/**
 * @brief 炸弹 (Bomb)
 * 
 * 四张一样的牌。
 */
class ZHADAN : public Pai {
    int x; ///< 炸弹点数
public:
    ZHADAN(int x);
    static vector<Pai *> get(int *arr);
    ostream& output() override;
    bool operator>(Pai *pre) override;
    void take(int *) override;
    void back(int *) override;
    unsigned int encode() override {
        return ((unsigned int)PaiType::ZHADAN_T) | (x << 4);
    }
};

/**
 * @brief 王炸 (Rocket)
 * 
 * 大王 + 小王。
 */
class WANGZHA : public Pai {
public:
    WANGZHA();
    static vector<Pai *> get(int *arr);
    ostream& output() override;
    bool operator>(Pai *pre) override;
    void take(int *) override;
    void back(int *) override;
    unsigned int encode() override {
        return ((unsigned int)PaiType::WANGZHA_T);
    }
};

/**
 * @brief 四带二 (Four with Two)
 * 
 * 四张一样的牌，带两张单牌或两对。
 */
class SIDAIER : public Pai {
    int x;          ///< 主牌点数
    int d1, d2;     ///< 带牌的点数
    bool isDui;     ///< 是否带对子
public:
    SIDAIER(int x, int d1, int d2, bool isDui);
    static vector<Pai *> get(int *arr);
    ostream& output() override;
    bool operator>(Pai *pre) override;
    void take(int *) override;
    void back(int *) override;
    unsigned int encode() override {
        return ((unsigned int)PaiType::SIDAIER_T) | (x << 4) | (d1 << 9) | (d2 << 13) | (isDui ? (1<<17) : 0);
    }
};

/**
 * @brief 飞机 (Aircraft)
 * 
 * 连续的三张，可以带同数量的单牌或对子。
 */
class FEIJI : public Pai {
    int head;           ///< 机身起点
    int length;         ///< 机身组数
    int wingType;       ///< 翅膀类型：0=不带，1=单，2=对
    vector<int> wings;  ///< 翅膀点数列表
public:
    FEIJI(int head, int length, int wingType, const vector<int>& wings);
    static vector<Pai *> get(int *arr);
    ostream& output() override;
    bool operator>(Pai *pre) override;
    void take(int *) override;
    void back(int *) override;
    unsigned int encode() override {
        return ((unsigned int)PaiType::FEIJI_T) | (head << 4) | (length << 9) | (wingType << 13);
    }
};

/**
 * @brief 连对 (Consecutive Pairs)
 * 
 * 连续的对子，至少3对。
 */
class LIANDUI : public Pai {
    int head;   ///< 起点点数
    int length; ///< 对子对数
public:
    LIANDUI(int head, int length);
    static vector<Pai *> get(int *arr);
    ostream& output() override;
    bool operator>(Pai *pre) override;
    void take(int *) override;
    void back(int *) override;
    unsigned int encode() override {
        return ((unsigned int)PaiType::LIANDUI_T) | (head << 4) | (length << 9);
    }
};

