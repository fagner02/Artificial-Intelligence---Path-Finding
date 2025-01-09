#include <iostream>
#include <vector>
#include <stack>
#include <tuple>
#include <cstring>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <thread>
#include "costs.cpp"

using namespace std;

const int space_size = 6;

struct point {
    int x, y;
};

struct visited_info {
    float cost;
    int step;
};

const point dirs[] = {
    /*0: down*/ {0, 1},
    /*1: right*/{1, 0},
    /*2: up*/   {0, -1},
    /*3: left*/ {-1, 0}
};

bool operator==(const point& lhs, const point& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

void print_path(vector<point>& path, visited_info visited[space_size][space_size]) {
    std::cout << "\x1b[H\x1b[J" << std::flush;
    for (int y = space_size - 1; y > -1; y--) {
        for (int x = 0; x < space_size; x++) {
            point p = { x, y };
            auto it = std::find(path.begin(), path.end(), p);
            if (it != path.end()) {
                cout << "\033[32;1m" << setw(4) << visited[x][y].cost << "\033[0m" << setw(4) << " | ";
            } else {
                cout << setw(4) << visited[x][y].cost << setw(4) << " | ";
            }
        }
        cout << "\n";
    }
}

void print(point target, point next, point prev, visited_info visited[space_size][space_size]) {
    std::cout << "\x1b[H\x1b[J" << std::flush;
    for (int i = 0; i < space_size; i++) {
        for (int j = 0; j < space_size; j++) {
            if (i == next.x && j == next.y) {
                cout << "\033[1;36m" << setw(4) << "X" << "\033[0m" << setw(4) << " | ";
            } else if (i == prev.x && j == prev.y) {
                cout << "\033[1;31m" << setw(4) << "P" << "\033[0m" << setw(4) << " | ";
            } else if (i == target.x && j == target.y) {
                cout << setw(4) << "T" << setw(4) << " | ";
            } else if (visited[i][j].cost == -1) {
                cout << setw(4) << visited[i][j].cost << setw(4) << " | ";
            } else {
                cout << "\033[1;32m" << setw(4) << visited[i][j].cost << "\033[0m" << setw(4) << " | ";
            }
        }
        cout << "\n";
    }
    cout << "\n";
}

void calculate_path(point start, point target, visited_info visited[space_size][space_size]) {
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
                if (visited[next.x][next.y].cost < visited[target.x][target.y].cost) {
                    target = next;
                }
            }
        }
        path.push_back(target);
        this_thread::sleep_for(chrono::milliseconds(500));
        print_path(path, visited);
    }
}


void dfs(point start, point target, cost_fn cost) {
    visited_info visited[space_size][space_size];

    for (int i = 0; i < space_size; i++) {
        for (int j = 0; j < space_size; j++) {
            visited[i][j].cost = -1;
            visited[i][j].step = -1;
        }
    }

    stack<point> stack;
    stack.push(start);
    visited[start.x][start.y].cost = 0;
    visited[start.x][start.y].step = 0;
    while (!stack.empty()) {
        point node = stack.top();
        stack.pop();

        for (int i = 0; i < 4; i++) {
            point next = { node.x + dirs[i].x, node.y + dirs[i].y };
            if (next.x < 0 || next.x >= space_size || next.y < 0 || next.y >= space_size) {
                continue;
            }
            if (next.x == target.x && next.y == target.y) {
                cout << "Found target\n" << visited[node.x][node.y].cost + 1 << "\n";
            }
            if (visited[next.x][next.y].cost == -1 ||
                visited[node.x][node.y].cost + 1 < visited[next.x][next.y].cost) {
                stack.push(next);

                visited[next.x][next.y].cost = visited[node.x][node.y].cost + cost(i, visited[node.x][node.y].step + 1);
                visited[next.x][next.y].step = visited[node.x][node.y].step + 1;
                // this_thread::sleep_for(chrono::milliseconds(500));
                // print(target, next, node, visited);
            }
        }
    }

    calculate_path(start, target, visited);
}

struct node {
    point p;
    string label;
};

int main() {
    cout << "Hello, World!\n";

    node nodes[space_size * space_size];
    vector<vector<node>> graph(space_size * space_size, vector<node>());

    int index = 0;
    for (int i = 0; i < space_size; i++) {
        for (int j = 0; j < space_size; j++) {
            nodes[index].p = { i, j };

            for (int k = 0; k < 4; k++) {
                point next = { i + dirs[k].x, j + dirs[k].y };
                if (next.x < 0 || next.x >= space_size || next.y < 0 || next.y >= space_size) {
                    continue;
                }
                graph[index].push_back(nodes[next.x * space_size + next.y]);
            }
        }
        cout << "\n";
    }

    dfs(point{ 0, 0 }, point{ 5, 5 }, costs[3]);

    return 0;
}