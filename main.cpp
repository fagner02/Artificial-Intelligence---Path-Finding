#include <iostream>
#include <vector>
#include <stack>
#include <tuple>
#include <map>
#include <cstring>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <thread>
#include "structures.h"
#include "constants.h"
#include "costs.h"
#include "print.h"

using namespace std;

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

    std::map<pair<int, int>, std::vector<point>> graph;

    for (int i = 0; i < space_size; i++) {
        for (int j = 0; j < space_size; j++) {
            graph.insert({ {i, j}, {} });
        }
    }

    for (int i = 0; i < space_size; i++) {
        for (int j = 0; j < space_size; j++) {
            for (int k = 0; k < 4; k++) {
                point next = { i + dirs[k].x, j + dirs[k].y };
                if (next.x < 0 || next.x >= space_size || next.y < 0 || next.y >= space_size) {
                    continue;
                }
                graph[{i, j}].push_back(next);
            }
        }
    }

    dfs(point{ 0, 0 }, point{ 5, 5 }, costs[3]);

    return 0;
}