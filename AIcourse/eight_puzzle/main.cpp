#include <iostream>
#include <map>
#include <unordered_set>
#include <stack>
#include <queue>
#include "puzzle.hpp"
using namespace std;

// {是否成功, 步数, 拓展节点数}
using ResultTriple = tuple<bool, unsigned int, unsigned int>;
ostream& operator<<(ostream& os, const ResultTriple& result) {
    if (std::get<0>(result)) {
        os << "success, steps: " << std::get<1>(result) << ", ";
    } else {
        os << "failed, ";
    }
    os << "expand: " << std::get<2>(result);
    return os;
}

// 用于限制 深度有限搜索 的深度
const int MAX_DEPTH = 25;

/**
 * @brief 广度优先搜索
 * @param initialPuzzle 初始状态Puzzle
 * @param tgtPuzzle 目标状态Puzzle
 * @return {是否成功, 步数, 拓展节点数}
 */
ResultTriple BreadthFirstSearch(const Puzzle& initPuzzle, const Puzzle& tgtPuzzle) {
    cout << "initial State: " << endl << initPuzzle << endl;
    // 用于记录当前状态是否被访问过
    map<unsigned int, bool> visited;
    Puzzle initPuzzle_copy(initPuzzle);
    initPuzzle_copy.depth = 0;
    initPuzzle_copy.precedeActionList.clear();
    int expandNodeCount = 0;
    /*
		请在该位置完成广度优先搜索函数。
	*/
    queue<Puzzle> puzzleNodeQueue;
    puzzleNodeQueue.emplace(std::move(initPuzzle_copy));
    while (!puzzleNodeQueue.empty()) {
        Puzzle currentPuzzle = std::move(puzzleNodeQueue.front());
        puzzleNodeQueue.pop();
        if (currentPuzzle == tgtPuzzle) {
            for (int i = 0; i < currentPuzzle.precedeActionList.size(); i++) {
                printAction(currentPuzzle.precedeActionList[i], i + 1);
            }
            cout << "\nfinal result:\n" << currentPuzzle << endl;
            return {true, currentPuzzle.depth, expandNodeCount};
        } else {
            visited[currentPuzzle.visitedNum()] = true;
            for (const Direction& action : currentPuzzle.nextActionList) {
                Puzzle nextPuzzleNode = currentPuzzle.moveTo(action);
                if (visited.find(nextPuzzleNode.visitedNum()) != visited.end()) {
                    // 已经访问过
                    continue;
                }
                nextPuzzleNode.precedeActionList.push_back(action);
                puzzleNodeQueue.emplace(std::move(nextPuzzleNode));
                expandNodeCount++;
            }
        }
    }
    return {false, 0, expandNodeCount};
}

/**
 * @brief 深度有限搜索，最大深度限度为25
 * @param initPuzzle 初始状态Puzzle
 * @param tgtPuzzle 目标状态Puzzle
 * @param maxDepth 最大深度
 * @return {是否成功, 步数, 拓展节点数}
 */
ResultTriple DepthFirstSearch(const Puzzle& initPuzzle, const Puzzle& tgtPuzzle, int maxDepth = MAX_DEPTH) {
    cout << "max depth: " << maxDepth << ", initial State: " << endl << initPuzzle << endl;
    // 用于记录当前状态是否被访问过
    map<unsigned int, bool> visited;
    Puzzle initPuzzle_copy(initPuzzle);
    initPuzzle_copy.depth = 0;
    initPuzzle_copy.precedeActionList.clear();
    int expandNodeCount = 0;
    /*
		请在该位置完成深度有限搜索，最大深度限度为25。
	*/
    stack<Puzzle> puzzleStack;
    puzzleStack.emplace(std::move(initPuzzle_copy));
    while (!puzzleStack.empty()) {
        Puzzle currentPuzzle = std::move(puzzleStack.top());
        puzzleStack.pop();
        if (currentPuzzle == tgtPuzzle) {
            for (int i = 0; i < currentPuzzle.precedeActionList.size(); i++) {
                printAction(currentPuzzle.precedeActionList[i], i + 1);
            }
            cout << "\nfinal result:\n" << currentPuzzle << endl;
            return {true, currentPuzzle.depth, expandNodeCount};
        } else if (currentPuzzle.depth >= maxDepth) {
            if (puzzleStack.empty()) {
                continue;
            }
            // 清理访问记录
            int lastDepth = puzzleStack.top().depth;
            for (int i = currentPuzzle.depth - 1; i > lastDepth; i--) {
                const Direction& action = currentPuzzle.precedeActionList.back();
                currentPuzzle.precedeActionList.pop_back();
                switch(action) {
                    case Direction::UP:
                        currentPuzzle.move(Direction::DOWN);
                        break;
                    case Direction::DOWN:
                        currentPuzzle.move(Direction::UP);
                        break;
                    case Direction::LEFT:
                        currentPuzzle.move(Direction::RIGHT);
                        break;
                    case Direction::RIGHT:
                        currentPuzzle.move(Direction::LEFT);
                        break;
                }
                visited.erase(currentPuzzle.visitedNum());
            }
        } else {
            visited[currentPuzzle.visitedNum()] = true;
            if (currentPuzzle.depth < MAX_DEPTH) {
                for (const Direction& action : currentPuzzle.nextActionList) {
                    Puzzle nextPuzzleNode = currentPuzzle.moveTo(action);
                    if (visited.find(nextPuzzleNode.visitedNum()) != visited.end()) {
                        // 已经访问过
                        continue;
                    }
                    nextPuzzleNode.precedeActionList.push_back(action);
                    puzzleStack.emplace(std::move(nextPuzzleNode));
                    expandNodeCount++;
                }
            }
        }
    }
    return {false, 0, expandNodeCount};
}

int Heuristic1(const Puzzle& puzzle, const Puzzle& tgtPuzzle) {
    int incorrectCount = 0;
    for (int i = 0; i < puzzle.puzzle.size(); i++) {
        if (puzzle.puzzle[i] != tgtPuzzle.puzzle[i]) {
            incorrectCount++;
        }
    }
    return incorrectCount;
}

/**
 * @brief 启发式搜索框架
 * @param initPuzzle 初始状态Puzzle
 * @param tgtPuzzle 目标状态Puzzle
 * @param heuristic 启发式函数指针
 * @return {是否成功, 步数, 拓展节点数}
 */
ResultTriple heuristicSearch(const Puzzle& initPuzzle, const Puzzle& tgtPuzzle, int (*heuristic)(const Puzzle&, const Puzzle&) = Heuristic1) {
    cout << "initial State: " << endl << initPuzzle << endl;
    Puzzle initPuzzle_copy(initPuzzle);
    initPuzzle_copy.depth = 0;
    initPuzzle_copy.precedeActionList.clear();
    int expandNodeCount = 0;
    // 记录距离，防止反复计算
    struct HeuristicInfo {
        Puzzle puzzle;
        int distance;   // 记录 depth + heuristic
        // hash unordered_set要求
        size_t operator()(const HeuristicInfo& info) const {
            return info.puzzle.visitedNum();
        }
        // equal unordered_set要求
        bool operator()(const HeuristicInfo& lhs, const HeuristicInfo& rhs) const {
            return lhs.puzzle == rhs.puzzle;
        }
        // min_element要求
        bool operator<(const HeuristicInfo& other) const {
            return distance < other.distance;
        }
    };
    // 记录候选和已访问节点 用unordered_set优化性能
    unordered_set<HeuristicInfo, HeuristicInfo, HeuristicInfo> openSet;
    unordered_set<HeuristicInfo, HeuristicInfo, HeuristicInfo> closeSet;
    int d = heuristic(initPuzzle_copy, tgtPuzzle);
    openSet.insert({std::move(initPuzzle_copy), d});
    while (!openSet.empty()) {
        HeuristicInfo currentInfo = *min_element(openSet.begin(), openSet.end());
        const Puzzle& currentPuzzle = currentInfo.puzzle;
        openSet.erase(currentInfo);
        // 遍历子节点
        for (const Direction& action : currentPuzzle.nextActionList) {
            Puzzle nextPuzzleNode = currentPuzzle.moveTo(action);
            nextPuzzleNode.precedeActionList.push_back(action);
            // 一般情况下应该在取出来的时候判断，而不是这里
            // 由于最终结果是depth，所以可以在遍历动作时判断是否为目标状态，不会影响最优性
            if (nextPuzzleNode == tgtPuzzle) {
                for (int i = 0; i < nextPuzzleNode.precedeActionList.size(); i++) {
                    printAction(nextPuzzleNode.precedeActionList[i], i + 1);
                }
                cout << "\nfinal result:\n" << nextPuzzleNode << endl;
                return {true, nextPuzzleNode.depth, expandNodeCount};
            }
            // 加入openSet
            int distance = nextPuzzleNode.depth + heuristic(nextPuzzleNode, tgtPuzzle);
            HeuristicInfo nextInfo = {std::move(nextPuzzleNode), distance};
            if (closeSet.find(nextInfo) != closeSet.end()) {
                // 在closeSet中，跳过
                continue;
            }
            auto it = openSet.find(nextInfo);
            if (it != openSet.end()) {
                // 在openSet中，更新距离
                if (distance < it->distance) {
                    openSet.erase(it);
                    openSet.emplace(std::move(nextInfo));
                }
            } else {
                openSet.emplace(std::move(nextInfo));
                expandNodeCount++;
            }
        }
        closeSet.emplace(std::move(currentInfo));
    }
    return {false, 0, expandNodeCount};
}

/**
 * @brief 启发式搜索1，启发式函数使用不正确位置的数码个数
 * @param initPuzzle 初始状态Puzzle
 * @param tgtPuzzle 目标状态Puzzle
 * @param heuristic 启发式函数指针
 * @return {是否成功, 步数, 拓展节点数}
 */
ResultTriple heuristicSearchInformedByIncorrectNum(const Puzzle& initPuzzle, const Puzzle& tgtPuzzle) {
    /*
		请在该位置完成启发式搜索，启发式函数使用不正确位置的数码个数。
	*/
    return heuristicSearch(initPuzzle, tgtPuzzle, Heuristic1);
}

int Heuristic2(const Puzzle& puzzle, const Puzzle& tgtPuzzle) {
    int manhattanDistance = 0;
    for (int i = 0; i < puzzle.puzzle.size(); i++) {
        char value = puzzle.puzzle[i];
        if (value != 0) { // 忽略空格
            // 计算当前数码在目标状态中的位置
            int targetIndex = 0;
            for (int j = 0; j < tgtPuzzle.puzzle.size(); j++) {
                if (tgtPuzzle.puzzle[j] == value) {
                    targetIndex = j;
                    break;
                }
            }
            int currentRow = i / 3;
            int currentCol = i % 3;
            int targetRow = targetIndex / 3;
            int targetCol = targetIndex % 3;
            manhattanDistance += abs(currentRow - targetRow) + abs(currentCol - targetCol);
        }
    }
    return manhattanDistance;
}

/**
 * @brief 启发式搜索2，启发式函数采用到目标位置的曼哈顿距离
 * @param initPuzzle 初始状态Puzzle
 * @param tgtPuzzle 目标状态Puzzle
 * @return {是否成功, 步数, 拓展节点数}
 */
ResultTriple heuristicSearchInformedByManhattonDis(const Puzzle& initPuzzle, const Puzzle& tgtPuzzle) {
    /*
		请在该位置完成启发式搜索，启发式函数采用到目标位置的曼哈顿距离。
	*/
    return heuristicSearch(initPuzzle, tgtPuzzle, Heuristic2);
}

int main() {
    Puzzle tgtPuzzle;
    Puzzle initPuzzle;
    Puzzle::randomWalk(20, initPuzzle);
    {
        cout << "=========广度优先搜索=========\n";
        ResultTriple result = BreadthFirstSearch(initPuzzle, tgtPuzzle);
        cout << result << "\n\n";
    }
    {
        cout << "=======深度有限优先搜索=======\n";
        ResultTriple result = DepthFirstSearch(initPuzzle, tgtPuzzle);
        cout << result << "\n\n";
    }
    {
        cout << "=========启发式搜索1=========\n";
        ResultTriple result = heuristicSearchInformedByIncorrectNum(initPuzzle, tgtPuzzle);
        cout << result << "\n\n";
    }
    {
        cout << "=========启发式搜索2=========\n";
        ResultTriple result = heuristicSearchInformedByManhattonDis(initPuzzle, tgtPuzzle);
        cout << result << "\n\n";
    }
}