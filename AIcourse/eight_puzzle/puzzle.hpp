#pragma once
#include <array>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <ctime>
#include <cassert>
#include <random>
#include <algorithm>

using COOR = std::array<int, 2>;

enum class Direction {
    UP = 0,
    DOWN = 1,
    LEFT = 2,
    RIGHT = 3
};

const COOR directions[4] = {
    {-1, 0},  // UP
    {1, 0},  // DOWN
    {0, -1},  // LEFT
    {0, 1}  // RIGHT
};

/**
 * @brief 输出动作信息
 * @param action 动作方向
 * @param index 步数索引
 */
void printAction(const Direction& action, int index) {
    const std::string directions[4] = {"up", "down", "left", "right"};
    int dirIndex = static_cast<int>(action);
    if (dirIndex >= 0 && dirIndex < 4) {
        std::cout << "@step " << index << ": " << directions[dirIndex] << "\n";
    }
}

class Puzzle {
public:
    /**
     * @brief 记录九宫格内每一个格子对应的数码。每个元素为数码，0表示空格
     * 一维数组表示二维数组，对应元素顺序如下:
     * [0][0], [0][1], [0][2], [1][0], [1][1], [1][2], [2][0], [2][1], [2][2]
     * 使用 std::array 是因为懒得写移动构造和复制构造
     */
    std::array<char, 9> puzzle;
    /**
     * @brief 后继可操作动作状态列表
     * (-1, 0) 代表向上移动, (1, 0) 代表向下移动, (0, -1) 代表向左移动, (0, 1) 代表向右移动
     * 通过 Direction 枚举类表示
     * 具体值使用 directions 数组获取
     */
    std::vector<Direction> nextActionList;
    std::vector<Direction> precedeActionList;
    int depth;

    /**
     * @brief 默认构造函数，生成目标状态Puzzle节点，深度为0，自动更新nextActionList
     */
    Puzzle(unsigned int state = 876543210, int depth = 0) : depth(depth) {
        for (int i = 0; i < puzzle.size(); i++) {
            puzzle[i] = state % 10;
            state /= 10;
        }
        updateActionList();
    };

    friend std::ostream& operator<<(std::ostream& os, const Puzzle& puzzle) {
        for (int i = 0; i < puzzle.puzzle.size(); i++) {
            os << static_cast<int>(puzzle.puzzle[i]);
            if (i % 3 == 2)
                os << "\n";
            else
                os << "  ";
        }
        return os;
    }

    bool operator==(const Puzzle& other) const {
        return puzzle == other.puzzle;
    }

    /**
     * @brief 计算当前节点的唯一访问编号(十进制)
     * @return 访问编号
     */
    unsigned int visitedNum() const {
        unsigned int mapValue = 0;
        for (int i = 0; i < puzzle.size(); i++) {
            mapValue = mapValue * 10 + static_cast<unsigned int>(puzzle[i]);
        }
        return mapValue;
    }

    /**
     * @brief 找到 空格 0 所在的位置
     * @return 一个长度为2的array，分别代表行号和列号
     */
    COOR findZeroPosition() const {
        for (int i = 0; i < puzzle.size(); i++) {
            if (puzzle[i] == 0) {
                return {i / 3, i % 3};
            }
        }
        throw std::runtime_error("Zero position not found");
    }
    /**
     * @brief 根据当前状态更新nextActionList
     */
    void updateActionList(bool shuffle = true) {
        const COOR zeroPos = findZeroPosition();
        const int row = zeroPos[0];
        const int col = zeroPos[1];
        nextActionList.clear();
        if (row >= 1) nextActionList.push_back(Direction::UP);
        if (row <= 1) nextActionList.push_back(Direction::DOWN);
        if (col >= 1) nextActionList.push_back(Direction::LEFT);
        if (col <= 1) nextActionList.push_back(Direction::RIGHT);
        // 随机打乱
        if (!shuffle) return;
        static std::default_random_engine rng(static_cast<unsigned>(std::time(nullptr)));
        std::shuffle(nextActionList.begin(), nextActionList.end(), rng);
    }

    /**
     * @brief 执行动作action，更新当前Puzzle状态
     * @param action 移动方向
     */
    void move(const Direction& action) {
        const COOR zeroPos = findZeroPosition();
        const int x = zeroPos[0];
        const int y = zeroPos[1];

        const COOR moveDirection = directions[static_cast<int>(action)];
        const int targetX = x + moveDirection[0];
        const int targetY = y + moveDirection[1];

        std::swap(puzzle[x * 3 + y], puzzle[targetX * 3 + targetY]);
        updateActionList();
    }

    /**
     * @brief 给定动作action和当前Puzzle节点, 返回执行该动作后新的Puzzle节点 深度加1
     * @param action 移动方向
     */
    Puzzle moveTo(const Direction& action) const {
        Puzzle nextPuzzleNode(*this);
        nextPuzzleNode.move(action);
        nextPuzzleNode.depth += 1;
        return nextPuzzleNode;
    }

    /**
     * @brief 对Puzzle节点执行随机游走, 得到打乱后的Puzzle(保证可解)
     * @param steps 随机游走步数
     * @param puzzleNode 初始Puzzle节点
     */
    static void randomWalk(int steps, Puzzle& puzzleNode) {
        static std::default_random_engine rng(static_cast<unsigned>(std::time(nullptr)));
        for (int i = 0; i < steps; i++) {
            puzzleNode.updateActionList(false); // 后面有随机数了
            int actionSize = puzzleNode.nextActionList.size();
            std::uniform_int_distribution<int> dist(0, actionSize - 1);
            int randomIndex = dist(rng);
            puzzleNode.move(puzzleNode.nextActionList[randomIndex]);
        }
    }
};