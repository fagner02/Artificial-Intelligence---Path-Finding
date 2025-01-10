
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
    while (!stack.empty()) {
        point node = stack.top();
        stack.pop();

        for (int i = 0; i < 4; i++) {
            point next = { node.x + dirs[i].x, node.y + dirs[i].y };
            if (next.x < 0 || next.x >= space_size || next.y < 0 || next.y >= space_size) {
                continue;
            }
            if (next.x == target.x && next.y == target.y) {
                cout << "Found target\n"
                    << blocks[node.x][node.y].info.cost + 1 << "\n";
            }
            float costValue = cost(i, blocks[node.x][node.y].info.step + 1);
            if (blocks[next.x][next.y].info.cost == -1 || blocks[node.x][node.y].info.cost + costValue < blocks[next.x][next.y].info.cost) {
                stack.push(next);

                blocks[next.x][next.y].info.cost = blocks[node.x][node.y].info.cost + costValue;
                blocks[next.x][next.y].info.step = blocks[node.x][node.y].info.step + 1;

                set_block_colors(blocks, next, shouldDraw);
            }
        }
    }

    calculate_path(start, target, blocks, target, shouldDraw);
}