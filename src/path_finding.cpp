
#include <path_finding.h>

std::string dfs(
    point start,
    point target,
    cost_fn cost,
    int cost_id,
    std::set<point> constraints,
    block blocks[space_size][space_size],
    bool& shouldDraw,
    std::vector<int> order,
    bool animate
) {
    int visited_qty = 0;

    std::vector<node*> tree_nodes = { };

    add_node(tree_nodes, nullptr, start, constraints, 0, -1);

    std::set<node*> visited{ tree_nodes[0] };
    std::stack<node*> stack;
    stack.push(tree_nodes[0]);

    if (animate) set_block_data(blocks, *tree_nodes[0]);

    while (!stack.empty()) {
        node* current = stack.top();
        stack.pop();
        visited_qty++;

        for (int i = 0; i < 4; i++) {
            int dir = order[i];
            point next = { current->pos + dirs[dir] };
            if (next.x < 0 || next.x >= space_size || next.y < 0 || next.y >= space_size) {
                continue;
            }

            float costValue = current->data.cost + cost(dir, current->data.step + 1);

            if (isGoal(next, target, current, constraints)) {
                std::cout << "Found target\n";
                auto new_node = add_node(tree_nodes, current, next, {}, costValue, -1);

                if (animate) {
                    set_block_data(blocks, *new_node);
                    animatePath(*new_node, blocks, shouldDraw);
                }

                return generate_log(start, target, visited_qty, count_tree_nodes(tree_nodes[0]), "dfs", cost_id, -1, new_node->data.cost, calculatePath(*new_node), order, constraints);
            }

            auto existent = std::find_if(visited.begin(), visited.end(),
                [&](node* n) {
                    return n->pos == next;
                }
            );

            if (existent == visited.end() || shouldRevisit((*existent), next, current, constraints)) {
                if (existent != visited.end()) {
                    visited.erase(existent);
                }
                node* new_node = add_node(tree_nodes, current, next, constraints, costValue, -1);

                stack.push(new_node);
                visited.insert(new_node);

                if (animate) {
                    set_block_data(blocks, *new_node);
                    setBlockColors(blocks, shouldDraw, next);
                }
            }
        }
    }

    if (animate) animatePath(*tree_nodes[tree_nodes.size() - 1], blocks, shouldDraw);

    return generate_log(start, target, visited_qty, count_tree_nodes(tree_nodes[0]), "dfs", cost_id, -1, -1, calculatePath(*tree_nodes[tree_nodes.size() - 1]), order);
}

std::string a_star(
    point start,
    point target,
    cost_fn cost,
    heuristic_fn heuristic,
    int cost_id,
    int heuristic_id,
    std::set<point> constraints,
    block blocks[space_size][space_size],
    bool& shouldDraw,
    bool animate
) {
    std::vector <node*> tree_nodes = { };
    add_node(tree_nodes, nullptr, start, constraints, 0, heuristic(start, target));
    if (animate) set_block_data(blocks, *tree_nodes[0]);

    std::vector<node*> open;
    std::vector<node*> visited;

    open.push_back(tree_nodes[0]);

    int visited_qty = 0;

    while (!open.empty()) {
        auto min = min_element(open.begin(), open.end(), [&](node* a, node* b) {
            return a->data.cost + heuristic(a->pos, target) < b->data.cost + heuristic(b->pos, target);
            }
        );

        node* current = *min;
        open.erase(min);
        visited_qty++;
        visited.push_back(current);

        if (current->pos == target && current->data.found_goal) {
            std::cout << "Found target\n";
            // print_tree(tree_nodes[0]);
            if (animate) animatePath(*current, blocks, shouldDraw);
            std::vector<node> path = calculatePath(*current);
            return generate_log(start, target, visited.size(), open.size(), "a_star", cost_id, heuristic_id, current->data.cost, path, { 0,1,2,3 }, constraints);
        }

        for (int i = 0; i < 4; i++) {
            point next = { current->pos + dirs[i] };

            if (next.x < 0 || next.x >= space_size || next.y < 0 || next.y >= space_size) {
                continue;
            }

            auto visited_node = find_if(visited.begin(), visited.end(), [&](node* a) {return a->pos == next;});
            if (visited_node != visited.end()) {
                if (shouldRevisit(*visited_node, next, current, constraints)) {
                    visited.erase(visited_node);
                } else {
                    continue;
                }
            }
            float new_cost = current->data.cost + cost(i, current->data.step + 1);

            auto found = find_if(open.begin(), open.end(),
                [&](node* n) {
                    return n->pos == next;
                }
            );

            node* next_node = nullptr;

            if (found == open.end() || (shouldRevisit(*found, next, current, constraints))) {
                next_node = add_node(tree_nodes, current, next, constraints, new_cost, heuristic(next, target));

                if (found != open.end()) {
                    open.erase(found);
                }
                open.push_back(next_node);

                if (animate) {
                    set_block_data(blocks, *next_node);
                    setBlockColors(blocks, shouldDraw, next);
                }

                continue;
            } else {
                next_node = *found;
            }
            if (next_node->data.cost > new_cost && !(shouldRevisit(next_node, next, current, constraints))) {
                continue;
            }
            next_node = add_node(tree_nodes, current, next, constraints, new_cost, heuristic(next, target));

            if (animate) {
                set_block_data(blocks, *next_node);
                setBlockColors(blocks, shouldDraw, next);
            }
        }
    }
    // print_tree(tree_nodes[0]);
    return generate_log(start, target, visited.size(), open.size(), "a_star", cost_id, heuristic_id, -1, {}, { 0,1,2,3 }, constraints);
}

std::string bfs(point start,
    point target,
    cost_fn cost,
    block blocks[space_size][space_size],
    bool& shouldDraw,
    int cost_id,
    std::set<point> constraints,
    std::vector<int> order,
    bool animate
) {
    std::vector<node*> tree_nodes = { };

    add_node(tree_nodes, nullptr, start, constraints, 0, 0);

    int visited_qty = 0;

    std::vector<node*> visited{ tree_nodes[0] };
    std::queue<node*> queue;
    queue.push(tree_nodes[0]);

    if (animate) set_block_data(blocks, *tree_nodes[0]);

    while (!queue.empty()) {
        node* current = queue.front();
        queue.pop();
        visited_qty++;

        for (int i = 0; i < 4; i++) {
            int dir = order[i];
            point next = current->pos + dirs[dir];
            if (next.x < 0 || next.x >= space_size || next.y < 0 || next.y >= space_size) {
                continue;
            }

            float costValue = current->data.cost + cost(dir, current->data.step + 1);

            if (isGoal(next, target, current, constraints)) {
                std::cout << "Found target\n" << current->data.cost + 1 << "\n";

                node* new_node = add_node(tree_nodes, current, next, {}, costValue, -1);
                if (animate) {
                    set_block_data(blocks, *new_node);
                    animatePath(*new_node, blocks, shouldDraw);
                }
                return generate_log(start, target, visited_qty, count_tree_nodes(tree_nodes[0]), "bfs", cost_id, -1, new_node->data.cost, calculatePath(*new_node), order, constraints);
            }
            auto existent = std::find_if(visited.begin(), visited.end(),
                [&](node* n) {
                    return n->pos == next;
                }
            );
            if (existent == visited.end() || shouldRevisit((*existent), next, current, constraints)) {
                if (existent != visited.end()) {
                    visited.erase(existent);
                }
                node* new_node = add_node(tree_nodes, current, next, constraints, costValue, -1);
                visited.push_back(new_node);
                queue.push(new_node);

                if (animate) {
                    set_block_data(blocks, *new_node);
                    setBlockColors(blocks, shouldDraw, next);
                }
            }
        }
    }
    return generate_log(start, target, visited_qty, count_tree_nodes(tree_nodes[0]), "bfs", cost_id, -1, -1, {}, order, constraints);
}

std::string dijkstra(point start,
    point target,
    cost_fn cost,
    block blocks[space_size][space_size],
    bool& shouldDraw,
    int cost_id,
    std::set<point> constraints,
    bool animate
) {
    std::vector<node*> tree_nodes = { };

    add_node(tree_nodes, nullptr, start, constraints, 0, 0);

    if (animate) set_block_data(blocks, *tree_nodes[0]);

    int visited_qty = 0;

    std::vector<node*> visited{ tree_nodes[0] };
    std::vector<node*> pq;
    pq.push_back(tree_nodes[0]);

    while (!pq.empty()) {
        auto min = min_element(pq.begin(), pq.end(),
            [&](node* a, node* b) {
                return a->data.cost < b->data.cost;
            }
        );

        node* current = *min;
        pq.erase(min);
        current->data.visited = true;
        visited_qty++;

        if (isGoal(current, target, constraints)) {
            std::cout << "Found target\n";
            if (animate) animatePath(*current, blocks, shouldDraw);
            return generate_log(start, target, visited_qty, count_tree_nodes(tree_nodes[0]), "dijkstra", cost_id, -1, current->data.cost, calculatePath(*current));
        }

        for (int i = 0; i < 4; i++) {
            point next = { current->pos + dirs[i] };

            if (next.x < 0 || next.x >= space_size || next.y < 0 || next.y >= space_size) {
                continue;
            }

            auto existent = std::find_if(visited.begin(), visited.end(),
                [&](node* n) {
                    return n->pos == next;
                }
            );

            float new_cost = current->data.cost + cost(i, current->data.step + 1);

            if (existent == visited.end() || new_cost < (*existent)->data.cost || shouldRevisit(*existent, next, current, constraints)) {
                if (existent != visited.end()) {
                    visited.erase(existent);
                }
                node* next_node = add_node(tree_nodes, current, next, constraints, new_cost, -1);
                pq.push_back(next_node);
                visited.push_back(next_node);

                if (animate) {
                    set_block_data(blocks, *next_node);
                    setBlockColors(blocks, shouldDraw, next);
                }
            }
        }
    }

    return generate_log(start, target, visited_qty, count_tree_nodes(tree_nodes[0]), "dijkstra", cost_id, -1, -1, {});
}

std::vector<node*> greedy_search_cost(
    node* target,
    cost_fn cost,
    block blocks[space_size][space_size],
    bool& shouldDraw
) {
    std::vector<node*> path = {  };
    auto curr = target;
    while (curr != nullptr) {
        path.push_back(curr);
        curr = curr->parent;
    }
    auto first = path.end() - 1;
    (*first)->data.cost = 0;
    for (;first != path.begin();first--) {
        auto next = first - 1;
        point dir = (*first)->pos - (*next)->pos;
        int index = -1;
        for (int j = 0; j < 4; j++) {
            if (dirs[j] == dir) {
                index = j;
                break;
            }
        }
        (*next)->data.cost = (*first)->data.cost + cost(index, (*first)->data.step + 1);
    }

    return path;
}

/// @brief Busca gulosa
/// @param start ponto inicial da busca
/// @param target ponto final da busca
/// @param cost função de custo
/// @param heuristic função de heurística
/// @param cost_id id da função de custo
/// @param heuristic_id id da função de heurística
/// @param blocks parâmetro para animação
/// @param shouldDraw parâmetro para animação 
/// @param animate parâmetro para animação
/// @return std::string log da busca
std::string greedy_search(
    point start,
    point target,
    cost_fn cost,
    heuristic_fn heuristic,
    int cost_id,
    int heuristic_id,
    std::set<point> constraints,
    block blocks[space_size][space_size],
    bool& shouldDraw,
    bool animate
) {
    std::vector<node*> tree_nodes = {};

    add_node(tree_nodes, nullptr, start, constraints, 0, heuristic(start, target));
    if (animate) set_block_data(blocks, *tree_nodes[0]);

    std::vector<node*> visited{ tree_nodes[0] };
    std::vector<node*> open;

    int visited_qty = 0;

    open.push_back(tree_nodes[0]);

    while (!open.empty()) {
        auto min_it = min_element(open.begin(), open.end(),
            [&](node* a, node* b) {
                return a->data.heuristic < b->data.heuristic;
            }
        );

        node* current = *min_it;
        open.erase(min_it);
        visited_qty++;

        if (isGoal(current, target, constraints)) {
            std::cout << "Found target\n";

            auto path = greedy_search_cost(current, cost, blocks, shouldDraw);
            if (animate) {
                animate_greedy_search_cost(path, blocks, shouldDraw);
                animatePath(*current, blocks, shouldDraw);
            }

            return generate_log(start, target, visited_qty, count_tree_nodes(tree_nodes[0]), "greedy_search", cost_id, heuristic_id, current->data.cost, calculatePath(*current));
        }

        for (int i = 0; i < 4; i++) {
            point next = current->pos + dirs[i];

            if (next.x < 0 || next.x >= space_size || next.y < 0 || next.y >= space_size) {
                continue;
            }

            auto existent = std::find_if(visited.begin(), visited.end(),
                [&](node* n) {
                    return n->pos == next;
                }
            );
            if (existent != visited.end()) {
                if (shouldRevisit(*existent, next, current, constraints)) {
                    visited.erase(existent);
                } else {
                    continue;
                }
            }

            auto new_node = add_node(tree_nodes, current, next, constraints, 0, heuristic(next, target));

            open.push_back(new_node);
            visited.push_back(new_node);

            if (animate) {
                set_block_data(blocks, *new_node);
                setBlockColors(blocks, shouldDraw, next);
            }
        }
    }

    return generate_log(start, target, visited_qty, count_tree_nodes(tree_nodes[0]), "greedy_search", cost_id, heuristic_id, -1, {});
}
