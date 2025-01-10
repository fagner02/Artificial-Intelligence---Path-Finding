
#include <path_finding.h>

void dfs(
    point start,
    point target,
    cost_fn cost,
    block blocks[space_size][space_size], bool& shouldDraw
) {
    stack<point> stack;
    stack.push(start);
    blocks[start.x][start.y].info.cost = 0;
    blocks[start.x][start.y].info.step = 0;
    blocks[start.x][start.y].info.from = { -1, -1 };
    while (!stack.empty()) {
        point node = stack.top();
        stack.pop();

        for (int i = 0; i < 4; i++) {
            point next = { node.x + dirs[i].x, node.y + dirs[i].y };
            if (next.x < 0 || next.x >= space_size || next.y < 0 || next.y >= space_size) {
                continue;
            }
            float costValue = cost(i, blocks[node.x][node.y].info.step + 1);
            if (next.x == target.x && next.y == target.y) {
                cout << "Found target\n" << blocks[node.x][node.y].info.cost + 1 << "\n";
                blocks[next.x][next.y].info.cost = blocks[node.x][node.y].info.cost + costValue;
                blocks[next.x][next.y].info.step = blocks[node.x][node.y].info.step + 1;
                blocks[next.x][next.y].info.from = node;
                calculate_path(start, target, blocks, shouldDraw);
                return;
            }

            if (blocks[next.x][next.y].info.cost == -1) {
                stack.push(next);

                blocks[next.x][next.y].info.cost = blocks[node.x][node.y].info.cost + costValue;
                blocks[next.x][next.y].info.step = blocks[node.x][node.y].info.step + 1;
                blocks[next.x][next.y].info.from = node;

                set_block_colors(blocks, next, shouldDraw);
            }
        }
    }

    calculate_path(start, target, blocks, shouldDraw);
}

float heuristic(point a, point b) {
    return 0;
}

struct a_star_node {
    point pos;
    float cost;
    float heuristic;
    float step;
};

void a_star(point start, point target, cost_fn cost) {
    vector<point> open;
    vector<point> closed;

    a_star_node nodes[space_size][space_size];

    for (int i = 0; i < space_size; i++) {
        for (int j = 0; j < space_size; j++) {
            nodes[i][j] = {
                {i, j},
                -1,
                -1,
                -1
            };
        }
    }

    nodes[start.x][start.y].cost = 0;
    nodes[start.x][start.y].heuristic = heuristic(start, target);
    nodes[start.x][start.y].step = 0;

    open.push_back(start);

    while (!open.empty()) {
        auto min = min_element(open.begin(), open.end(), [&](point a, point b) {
            a_star_node nodeA = nodes[a.x][a.y];
            a_star_node nodeB = nodes[b.x][b.y];
            return nodeA.cost + heuristic(a, target) < nodeB.cost + heuristic(b, target);
            });
        point current = *min;
        open.erase(min);

        closed.push_back(current);

        if (current.x == target.x && current.y == target.y) {
            cout << "Found target\n";
            break;
        }

        for (int i = 0; i < 4; i++) {
            point next = { current.x + dirs[i].x, current.y + dirs[i].y };
            if (next.x < 0 || next.x >= space_size || next.y < 0 || next.y >= space_size) {
                continue;
            }


            nodes[next.x][next.y].step = nodes[current.x][current.y].step + 1;
            nodes[next.x][next.y].cost = nodes[current.x][current.y].cost + cost(i, nodes[next.x][next.y].step);
            nodes[next.x][next.y].heuristic = heuristic(next, target);

            if (find(closed.begin(), closed.end(), next) != closed.end()) {
                continue;
            }

            float costValue = 1;
            if (find(open.begin(), open.end(), next) == open.end()) {
                open.push_back(next);
            }

            if (current.x != start.x && current.y != start.y) {
                costValue = 1.4;
            }

            if (current.x != target.x && current.y != target.y) {
                costValue = 1.4;
            }
        }
    }
}