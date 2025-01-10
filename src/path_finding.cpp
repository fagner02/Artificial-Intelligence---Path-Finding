
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

void a_star(point start, point target, cost_fn cost, block nodes[space_size][space_size], bool& shouldDraw) {
    vector<point> open;
    vector<point> closed;

    nodes[start.x][start.y].info.cost = 0;
    nodes[start.x][start.y].info.heuristic = heuristic(start, target);
    nodes[start.x][start.y].info.step = 0;
    nodes[start.x][start.y].info.from = { -1, -1 };

    open.push_back(start);

    while (!open.empty()) {
        auto min = min_element(open.begin(), open.end(), [&](point a, point b) {
            block nodeA = nodes[a.x][a.y];
            block nodeB = nodes[b.x][b.y];
            return nodeA.info.cost + heuristic(a, target) < nodeB.info.cost + heuristic(b, target);
            });
        point current = *min;
        open.erase(min);

        closed.push_back(current);

        if (current.x == target.x && current.y == target.y) {
            cout << "Found target\n";;
            calculate_path(start, target, nodes, shouldDraw);
            return;
        }

        for (int i = 0; i < 4; i++) {
            point next = { current.x + dirs[i].x, current.y + dirs[i].y };
            if (next.x < 0 || next.x >= space_size || next.y < 0 || next.y >= space_size) {
                continue;
            }


            if (find(closed.begin(), closed.end(), next) != closed.end()) {
                continue;
            }
            float new_cost = nodes[current.x][current.y].info.cost + cost(i, nodes[current.x][current.y].info.step + 1);

            auto found = find(open.begin(), open.end(), next);
            if (found == open.end()) {
                open.push_back(next);
            } else if (new_cost >= nodes[next.x][next.y].info.cost) {
                continue;
            }

            nodes[next.x][next.y].info.step = nodes[current.x][current.y].info.step + 1;
            nodes[next.x][next.y].info.cost = new_cost;
            nodes[next.x][next.y].info.heuristic = heuristic(next, target);
            nodes[next.x][next.y].info.from = current;

            set_block_colors(nodes, next, shouldDraw);
        }
    }
}