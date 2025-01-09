#include <iostream>
#include <vector>
#include <stack>
#include <tuple>
#include <cstring>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <thread>


using namespace std;

const int space_size = 6;

struct point {
    int x, y;
};

const point dirs[] = {
    {0, 1}, {1, 0}, {0, -1}, {-1, 0}
};

bool operator==(const point& lhs, const point& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

void print_path(vector<point> path, int visited[space_size][space_size]) {
    std::cout << "\x1b[H\x1b[J" << std::flush;
    for (int i = 0; i < space_size; i++) {
        for (int j = 0; j < space_size; j++) {
            point p = { i, j };
            auto it = std::find(path.begin(), path.end(), p);
            if (it != path.end()) {
                cout << "\033[32;1m" << setw(4) << visited[i][j] << "\033[0m" << setw(4) << " | ";
            } else {
                cout << setw(4) << visited[i][j] << setw(4) << " | ";
            }
        }
        cout << "\n";
    }
}

void print(point target, point next, point prev, int visited[space_size][space_size]) {
    std::cout << "\x1b[H\x1b[J" << std::flush;
    for (int i = 0; i < space_size; i++) {
        for (int j = 0; j < space_size; j++) {
            if (i == next.x && j == next.y) {
                cout << "\033[1;36m" << setw(4) << "X" << "\033[0m" << setw(4) << " | ";
            } else if (i == prev.x && j == prev.y) {
                cout << "\033[1;31m" << setw(4) << "P" << "\033[0m" << setw(4) << " | ";
            } else if (i == target.x && j == target.y) {
                cout << setw(4) << "T" << setw(4) << " | ";
            } else if (visited[i][j] == -1) {
                cout << setw(4) << visited[i][j] << setw(4) << " | ";
            } else {
                cout << "\033[1;32m" << setw(4) << visited[i][j] << "\033[0m" << setw(4) << " | ";
            }
        }
        cout << "\n";
    }
    cout << "\n";
}

void dfs(point start, point target) {
    int visited[space_size][space_size];
    std::memset(visited, -1, sizeof(visited));

    stack<point> stack;
    stack.push(start);
    visited[start.x][start.y] = 0;
    while (!stack.empty()) {
        point node = stack.top();
        stack.pop();

        for (int i = 0; i < 4; i++) {
            point next = { node.x + dirs[i].x, node.y + dirs[i].y };
            if (next.x < 0 || next.x >= space_size || next.y < 0 || next.y >= space_size) {
                continue;
            }
            if (next.x == target.x && next.y == target.y) {
                cout << "Found target\n" << visited[node.x][node.y] + 1 << "\n";
            }
            if (visited[next.x][next.y] == -1 ||
                visited[node.x][node.y] + 1 < visited[next.x][next.y]) {
                stack.push(next);

                visited[next.x][next.y] = visited[node.x][node.y] + 1;

                this_thread::sleep_for(chrono::milliseconds(500));
                print(target, next, node, visited);
            }
        }
    }

    vector<point> path = { target };

    while (target.x != start.x || target.y != start.y) {
        cout << target.x << "," << target.y << "\n";
        point last = target;
        bool assigned = false;
        for (int i = 0; i < 4; i++) {
            point next = { last.x + dirs[i].x, last.y + dirs[i].y };
            if (next.x < 0 || next.x >= space_size || next.y < 0 || next.y >= space_size) {
                continue;
            }
            if (!assigned) {
                target = next;
                assigned = true;
            } else {
                if (visited[next.x][next.y] < visited[target.x][target.y]) {
                    target = next;
                }
            }
        }
        path.push_back(target);
        this_thread::sleep_for(chrono::milliseconds(500));
        print_path(path, visited);
    }
}

int main() {
    cout << "Hello, World!\n";

    dfs(point{ 0, 0 }, point{ 1, 5 });

    return 0;
}