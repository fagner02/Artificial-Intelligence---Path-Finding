
#include <path_finding.h>

std::string generate_log(
    point start,
    point target,
    int visited_qty,
    int generated_qty,
    block blocks[space_size][space_size],
    std::string algorithm,
    int cost_id,
    int heuristic_id,
    float cost,
    std::vector<int> order = { 0, 1, 2, 3 }
) {
    std::vector<point> path = {};

    point _target = target;
    while (target.x != -1 && target.y != -1) {
        path.push_back(target);
        target = blocks[target.x][target.y].info.from;
    }
    std::stringstream ss;
    ss << algorithm << "," << visited_qty << "," << generated_qty << "," << path.size() << ",[";
    for (auto p : path) {
        ss << "[" << p.x << " " << p.y << "]";
    }
    ss << "], [" << start.x << " " << start.y << "], [" << _target.x << " " << _target.y
        << "],";
    ss << cost_id << "," << heuristic_id << ",";
    ss << cost << ",\"[";

    ss << order[0];
    for (int i = 1; i < 4; i++) {
        ss << " " << order[i];
    }
    ss << "]\"";
    return ss.str();
}

std::string dfs(
    point start,
    point target,
    cost_fn cost,
    block blocks[space_size][space_size], bool& shouldDraw,
    int cost_id,
    std::vector<int> order,
    bool animate
) {

    int visited_qty = 1;
    int generated_qty = 0;

    std::stack<point> stack;
    stack.push(start);
    generated_qty++;
    blocks[start.x][start.y].info.cost = 0;
    blocks[start.x][start.y].info.step = 0;
    blocks[start.x][start.y].info.from = { -1, -1 };
    while (!stack.empty()) {
        point node = stack.top();
        stack.pop();

        for (int i = 0; i < 4; i++) {
            int dir = order[i];
            point next = { node.x + dirs[dir].x, node.y + dirs[dir].y };
            if (next.x < 0 || next.x >= space_size || next.y < 0 || next.y >= space_size) {
                continue;
            }
            visited_qty++;
            float costValue = cost(dir, blocks[node.x][node.y].info.step + 1);
            if (next.x == target.x && next.y == target.y) {
                std::cout << "Found target\n"
                    << blocks[node.x][node.y].info.cost + 1 << "\n";
                blocks[next.x][next.y].info.cost = blocks[node.x][node.y].info.cost + costValue;
                blocks[next.x][next.y].info.step = blocks[node.x][node.y].info.step + 1;
                blocks[next.x][next.y].info.from = node;
                visited_qty++;
                generated_qty++;

                if (animate) calculate_path(start, target, blocks, shouldDraw);
                return generate_log(start, target, visited_qty, generated_qty, blocks, "dfs", cost_id, -1, blocks[next.x][next.y].info.cost, order);
            }

            if (blocks[next.x][next.y].info.cost == -1) {
                stack.push(next);
                generated_qty++;
                blocks[next.x][next.y].info.cost = blocks[node.x][node.y].info.cost + costValue;
                blocks[next.x][next.y].info.step = blocks[node.x][node.y].info.step + 1;
                blocks[next.x][next.y].info.from = node;

                if (animate)   set_block_colors(blocks, next, shouldDraw);
            }
        }
    }

    if (animate) calculate_path(start, target, blocks, shouldDraw);

    return "null";
}

std::string a_star(
    point start,
    point target,
    cost_fn cost,
    heuristic_fn heuristic,
    block nodes[space_size][space_size],
    bool& shouldDraw,
    int cost_id,
    int heuristic_id,
    bool animate
) {
    std::vector<point> open;
    std::vector<point> closed;

    int visited_qty = 1;
    int generated_qty = 0;

    nodes[start.x][start.y].info.cost = 0;
    nodes[start.x][start.y].info.heuristic = heuristic(start, target);
    nodes[start.x][start.y].info.step = 0;
    nodes[start.x][start.y].info.from = { -1, -1 };

    open.push_back(start);
    generated_qty++;

    while (!open.empty()) {
        auto min = min_element(open.begin(), open.end(), [&](point a, point b) {
            block nodeA = nodes[a.x][a.y];
            block nodeB = nodes[b.x][b.y];
            return nodeA.info.cost + heuristic(a, target) < nodeB.info.cost + heuristic(b, target); });
        point current = *min;
        open.erase(min);

        closed.push_back(current);
        generated_qty++;

        if (current.x == target.x && current.y == target.y) {
            std::cout << "Found target\n";
            if (animate) calculate_path(start, target, nodes, shouldDraw);
            return generate_log(start, target, closed.size(), open.size(), nodes, "a_star", cost_id, heuristic_id, nodes[current.x][current.y].info.cost);
        }

        for (int i = 0; i < 4; i++) {
            point next = { current.x + dirs[i].x, current.y + dirs[i].y };
            if (next.x < 0 || next.x >= space_size || next.y < 0 || next.y >= space_size) {
                continue;
            }

            if (find(closed.begin(), closed.end(), next) != closed.end()) {
                continue;
            }

            visited_qty++;
            float new_cost = nodes[current.x][current.y].info.cost + cost(i, nodes[current.x][current.y].info.step + 1);

            auto found = find(open.begin(), open.end(), next);
            if (found == open.end()) {
                open.push_back(next);
                generated_qty++;
            } else if (new_cost >= nodes[next.x][next.y].info.cost) {
                continue;
            }

            nodes[next.x][next.y].info.step = nodes[current.x][current.y].info.step + 1;
            nodes[next.x][next.y].info.cost = new_cost;
            nodes[next.x][next.y].info.heuristic = heuristic(next, target);
            nodes[next.x][next.y].info.from = current;

            if (animate) set_block_colors(nodes, next, shouldDraw);
        }
    }

    return "null";
}

std::string bfs(point start,
    point target,
    cost_fn cost,
    block blocks[space_size][space_size],
    bool& shouldDraw,
    int cost_id,
    std::vector<int> order,
    bool animate
) {
    int visited_qty = 1;
    int generated_qty = 0;

    std::queue<point> queue;
    queue.push(start);
    generated_qty++;

    blocks[start.x][start.y].info.cost = 0;
    blocks[start.x][start.y].info.step = 0;
    blocks[start.x][start.y].info.from = { -1, -1 };

    while (!queue.empty()) {
        point current = queue.front();
        queue.pop();

        for (int i = 0; i < 4; i++) {
            int dir = order[i];
            point next = { current.x + dirs[dir].x, current.y + dirs[dir].y };
            if (next.x < 0 || next.x >= space_size || next.y < 0 || next.y >= space_size) {
                continue;
            }

            float costValue = cost(dir, blocks[current.x][current.y].info.step + 1);
            visited_qty++;
            if (next.x == target.x && next.y == target.y) {
                std::cout << "Found target\n"
                    << blocks[current.x][current.y].info.cost + 1 << "\n";
                blocks[next.x][next.y].info.cost = blocks[current.x][current.y].info.cost + costValue;
                blocks[next.x][next.y].info.step = blocks[current.x][current.y].info.step + 1;
                blocks[next.x][next.y].info.from = current;
                if (animate) calculate_path(start, target, blocks, shouldDraw);
                return generate_log(start, target, visited_qty, generated_qty, blocks, "bfs", cost_id, -1, blocks[next.x][next.y].info.cost, order);
            }
            if (blocks[next.x][next.y].info.cost == -1) {
                queue.push(next);
                generated_qty++;
                blocks[next.x][next.y].info.cost = blocks[current.x][current.y].info.cost + costValue;
                blocks[next.x][next.y].info.step = blocks[current.x][current.y].info.step + 1;
                blocks[next.x][next.y].info.from = current;

                if (animate)  set_block_colors(blocks, next, shouldDraw);
            }
        }
    }
    return "null";
}

std::string dijkstra(point start,
    point target,
    cost_fn cost,
    block blocks[space_size][space_size],
    bool& shouldDraw,
    int cost_id,
    bool animate
) {
    std::vector<point> pq;

    blocks[start.x][start.y].info.cost = 0;
    blocks[start.x][start.y].info.step = 0;
    blocks[start.x][start.y].info.from = { -1, -1 };

    int visited_qty = 1;
    int generated_qty = 0;

    pq.push_back(start);
    generated_qty++;

    while (!pq.empty()) {
        auto min = min_element(pq.begin(), pq.end(), [&](point a, point b) {
            return blocks[a.x][a.y].info.cost < blocks[b.x][b.y].info.cost;
            });

        point current = *min;
        pq.erase(min);

        if (current.x == target.x && current.y == target.y) {
            std::cout << "Found target\n";
            if (animate) calculate_path(start, target, blocks, shouldDraw);
            return generate_log(start, target, visited_qty, generated_qty, blocks, "dijkstra", cost_id, -1, blocks[current.x][current.y].info.cost);
        }

        for (int i = 0; i < 4; i++) {
            point next = { current.x + dirs[i].x, current.y + dirs[i].y };

            if (next.x < 0 || next.x >= space_size || next.y < 0 || next.y >= space_size) {
                continue;
            }

            visited_qty++;
            float costValue = cost(i, blocks[current.x][current.y].info.step + 1);
            float new_cost = blocks[current.x][current.y].info.cost + costValue;

            if (blocks[next.x][next.y].info.cost == -1 || new_cost < blocks[next.x][next.y].info.cost) {
                blocks[next.x][next.y].info.cost = new_cost;
                blocks[next.x][next.y].info.step = blocks[current.x][current.y].info.step + 1;
                blocks[next.x][next.y].info.from = current;
                pq.push_back(next);
                generated_qty++;
                if (animate) set_block_colors(blocks, next, shouldDraw);
            }
        }
    }

    return "null";
}

std::string greedy_search(
    point start,
    point target,
    cost_fn cost,
    heuristic_fn heuristic,
    block nodes[space_size][space_size],
    bool& shouldDraw,
    int cost_id,
    int heuristic_id,
    bool animate
) {
    std::vector<point> open;

    int visited_qty = 1;
    int generated_qty = 0;

    nodes[start.x][start.y].info.cost = 0;
    nodes[start.x][start.y].info.heuristic = heuristic(start, target);
    nodes[start.x][start.y].info.from = { -1, -1 };
    open.push_back(start);
    generated_qty++;

    while (!open.empty()) {
        auto min_it = min_element(open.begin(), open.end(), [&](point a, point b) {
            return nodes[a.x][a.y].info.heuristic < nodes[b.x][b.y].info.heuristic;
            });

        point current = *min_it;
        open.erase(min_it);

        if (current.x == target.x && current.y == target.y) {
            std::cout << "Found target\n";
            if (animate) calculate_path(start, target, nodes, shouldDraw);
            return generate_log(start, target, visited_qty, generated_qty, nodes, "greedy_search", cost_id, heuristic_id, nodes[current.x][current.y].info.cost);
        }

        for (int i = 0; i < 4; i++) {
            point next = { current.x + dirs[i].x, current.y + dirs[i].y };

            if (next.x < 0 || next.x >= space_size || next.y < 0 || next.y >= space_size) {
                continue;
            }

            visited_qty++;

            if (nodes[next.x][next.y].info.cost != -1) {
                continue;
            }

            nodes[next.x][next.y].info.heuristic = heuristic(next, target);
            nodes[next.x][next.y].info.from = current;
            nodes[next.x][next.y].info.cost = 0;
            open.push_back(next);
            generated_qty++;
            if (animate) set_block_colors(nodes, next, shouldDraw);
        }
    }

    return "null";
}
