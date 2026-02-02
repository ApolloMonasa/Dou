# NeoDou (斗地主残局求解器)

![C++](https://img.shields.io/badge/language-C++11-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

NeoDou 是一个高效的斗地主残局求解器，基于 Min-Max 搜索算法，并结合了 Alpha-Beta 剪枝、状态压缩（State Compression）和记忆化搜索（Memoization）等高级优化技术。它能够在极短时间内求解复杂的残局，并提供交互式的出牌推演功能。

## ✨ 功能特性

*   **全牌型支持**：
    *   单张 (Single)、对子 (Pair)
    *   三带一/对 (Three with One/Pair)
    *   顺子 (Straight)、连对 (Consecutive Pairs)
    *   **四带二** (Four with Two)：支持带两单或两对（不带王）
    *   **飞机** (Aircraft)：支持不带、带单、带对
    *   炸弹 (Bomb)、王炸 (Rocket)
*   **极速求解**：
    *   **状态压缩**：将手牌压缩为 64 位整数，牌型压缩为 32 位整数，极大降低内存占用和哈希计算开销。
    *   **记忆化搜索**：使用 `std::unordered_map` 缓存已计算的局面（不仅是 PASS 局面，而是所有局面），实现 O(1) 的重复状态查找。
    *   **零内存分配 (Zero-Allocation)**：核心求解过程不创建树节点对象，仅进行纯递归计算，性能强悍。
*   **交互式推演**：
    *   提供命令行交互界面，显示当前最佳出牌建议（[0]表示好棋，[1]表示坏棋）。
    *   支持回退（输入 -1）和分支选择。

## 🚀 快速开始

### 编译

确保您的环境安装了支持 C++11 的编译器（如 g++, clang++, MSVC）。

**Linux / macOS / Windows (MinGW)**:
```bash
g++ -std=c++11 -I ./include main.cpp src/pai.cc src/tree.cc -o dou_solver
```

**Windows (PowerShell)**:
```powershell
g++ -std=c++11 -I .\include main.cpp src\pai.cc src\tree.cc -o dou_solver.exe
```

### 运行

1.  准备输入文件 `input.txt`。格式为两行数字，分别代表玩家 A（我方）和玩家 B（对手）的手牌，每行以 `0` 结束。
    *   3-10: 对应点数
    *   11-13: J, Q, K
    *   14: A
    *   15: 2
    *   16: 小王
    *   17: 大王

    **示例 (input.txt)**:
    ```text
    3 3 3 4 4 4 5 5 6 6 0
    7 0
    ```
    *(解释：A 有 3334445566，B 有一张 7)*

2.  运行程序：
    ```bash
    ./dou_solver
    # 或者 Windows:
    .\dou_solver.exe
    ```

3.  根据提示输入数字选择出牌分支。
    *   `[ 0] : [0] ...` 表示这是一步必胜/不败的好棋。
    *   `[ 1] : [1] ...` 表示这是一步必败的坏棋。
    *   输入 `-1` 可以悔棋（回退到上一步）。

## 📁 项目结构

*   `include/`
    *   `pai.h`: 牌型类的定义（基类 Pai 及各种子类）。
    *   `tree.h`: 博弈树节点定义及核心求解函数声明。
*   `src/`
    *   `pai.cc`: 牌型逻辑的具体实现（生成、比较、出牌、回溯）。
    *   `tree.cc`: Min-Max 搜索算法、状态压缩与记忆化表的实现。
*   `main.cpp`: 程序入口，负责 I/O 和交互循环。

## 🧠 算法原理

本求解器本质上是在寻找博弈树中的 **纳什均衡** 点。
1.  **Min-Max 搜索**：假设双方都极其聪明，永远选择对自己最有利的走法。
2.  **Memoization**：由于斗地主出牌顺序不同可能到达相同的残局（例如先出3再出4，和先出4再出3），利用哈希表记录 `<HandA, HandB, LastMove>` 的胜负结果，避免重复计算。
3.  **Lazy Expansion**：交互界面中，只在用户走到某个节点时才生成其子节点，节省内存。

## 📝 License

MIT License
