#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <set>
#include <algorithm>

using namespace std;

// أنواع الخلايا
enum CellType {
    EMPTY = 0,
    WALL = 1,     // خلايا سوداء
    FLOODED = 2,  // خلايا رمادية (غارقة)
    GOAL = 3      // Target 'X'
};

struct Point {
    int x, y;
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
    bool operator<(const Point& other) const {
        if (x != other.x) return x < other.x;
        return y < other.y;
    }
};

struct Node {
    Point pos;
    int g; // التكلفة المقطوعة
    double h; // القيمة الإرشادية
    double f; // التكلفة الكلية f = g + h أو f = h للـ Greedy
    vector<Point> path;

    bool operator>(const Node& other) const {
        return f > other.f;
    }
};

class AntMaze {
private:
    int M, N;
    vector<vector<int>> grid;
    Point goal;

public:
    AntMaze(int rows, int cols, vector<vector<int>> g, Point goalPos)
        : M(rows), N(cols), grid(g), goal(goalPos) {}

    // 1. حساب المسافة الإقليدية h1
    double heuristic_h1(Point p) {
        return sqrt(pow(p.x - goal.x, 2) + pow(p.y - goal.y, 2));
    }

    // 2. حساب مسافة مانهاتن h2
    double heuristic_h2(Point p) {
        return abs(p.x - goal.x) + abs(p.y - goal.y);
    }

    // التحقق من صحة الحركة
    bool isValid(int x, int y) {
        if (x < 0 || x >= M || y < 0 || y >= N) return false;
        if (grid[x][y] == WALL) return false;
         // لا يمكن المرور عبر الجدران
        return true; // الخلايا الفارغة والغارقة والهدف مسموح بدخولها
    }

    // تنفيذ البحث (Greedy أو A*)
    void solve(Point start, string algorithmType, int heuristicType) {
        priority_queue<Node, vector<Node>, greater<Node>> openList;
        set<Point> visited;

        Node startNode;
        startNode.pos = start;
        startNode.g = 0;
        startNode.h = (heuristicType == 1) ? heuristic_h1(start) : heuristic_h2(start);
        startNode.f = (algorithmType == "Greedy") ? startNode.h : (startNode.g + startNode.h);
        startNode.path.push_back(start);

        openList.push(startNode);
        int nodesExpanded = 0;

        // الاتجاهات: أعلى، أسفل، يمين، يسار + البقاء في المكان
        int dx[] = {-1, 1, 0, 0, 0};
        int dy[] = {0, 0, 1, -1, 0};

        while (!openList.empty()) {
            Node current = openList.top();
            openList.pop();

            if (visited.count(current.pos)) continue;
            visited.insert(current.pos);
            nodesExpanded++;

            // وصل للهدف
            if (current.pos == goal) {
                cout << "[" << algorithmType << " | h" << heuristicType << "] Target Found!" << endl;
                cout << "  - Total Path Cost (Cost): " << current.g << endl;
                cout << "  - Nodes Expanded: " << nodesExpanded << endl;
                cout << "  - Path: ";
                for (auto p : current.path) cout << "(" << p.x << "," << p.y << ") ";
                cout << "\n----------------------------------------\n";
                return;
            }

            for (int i = 0; i < 5; i++) {
                int nx = current.pos.x + dx[i];
                int ny = current.pos.y + dy[i];

                if (isValid(nx, ny)) {
                    Point nextPos = {nx, ny};
                    if (!visited.count(nextPos)) {
                        Node child;
                        child.pos = nextPos;
                        child.g = current.g + 1; // كل حركة تكلف 1
                        child.h = (heuristicType == 1) ? heuristic_h1(nextPos) : heuristic_h2(nextPos);
                        child.f = (algorithmType == "Greedy") ? child.h : (child.g + child.h);
                        child.path = current.path;
                        child.path.push_back(nextPos);

                        openList.push(child);
                    }
                }
            }
        }
        cout << "[" << algorithmType << " | h" << heuristicType << "] No path found!" << endl;
    }
};

int main() {
    // تمثيل المتاهة 0: فارغ، 1: جدار، 2: غارق، 3: هدف
    vector<vector<int>> grid = {
        {0, 0, 0, 0, 3},
        {1, 2, 1, 1, 1},
        {0, 2, 0, 0, 0},
        {1, 2, 1, 0, 1},
        {0, 2, 0, 0, 0}
    };

    Point goal = {0, 4};
    AntMaze maze(5, 5, grid, goal);

    // 5 حالات بداية مختلفة للاختبار (5 Start States)
    vector<Point> startStates = {
        {4, 0}, // النملة في المكان الأساسي أسفل اليسار
        {4, 2},
        {2, 0},
        {0, 0},
        {2, 3}
    };

    for (int i = 0; i < startStates.size(); i++) {
        cout << "=== Test Case " << i + 1 << ": Start at ("
             << startStates[i].x << ", " << startStates[i].y << ") ===\n";
        maze.solve(startStates[i], "Greedy", 1);
        maze.solve(startStates[i], "Greedy", 2);
        maze.solve(startStates[i], "A*", 1);
        maze.solve(startStates[i], "A*", 2);
        cout << "\n";
    }

    return 0;
}
