#include <iostream>
#include <vector>
#include <stack>

using namespace std;

// Implement iterative depth-first search algorithm
void dfs(int start, int n, vector<vector<int>>& graph) {
    vector<bool> visited(n, false);
    stack<int> stack;
    stack.push(start);
    visited[start] = true;
    while (!stack.empty()) {
        int node = stack.top();
        stack.pop();
        cout << node << " ";
        for (int i = 0; i < graph[node].size(); i++) {
            if (!visited[graph[node][i]]) {
                stack.push(graph[node][i]);
                visited[graph[node][i]] = true;
            }
        }
    }
}

int main() {
    cout << "Hello, World!\n";

    // call dfs
    vector<vector<int>> graph = {
        {1, 2},
        {0, 3, 4},
        {0, 5},
        {1},
        {1},
        {2}
    };
    dfs(0, 6, graph);

    return 0;
}