
#include <path_finding.h>

std::string generate_log(
    point start,
    point target,
    int visited_qty,
    int generated_qty,
    std::string algorithm,
    int cost_id,
    int heuristic_id,
    float cost,
    std::vector<node> path,
    std::vector<int> order = { 0, 1, 2, 3 }
) {
    std::stringstream ss;
    ss << algorithm << "," << visited_qty << "," << generated_qty << "," << path.size() << ",";
    if (path.size() == 0) {
        ss << "null,";
    } else {
        for (auto p : path) {
            ss << "[" << p.pos.x << " " << p.pos.y << "]";
        }
        ss << "],";
    }

    ss << "[" << start.x << " " << start.y << "], [" << target.x << " " << target.y << "],";
    ss << cost_id << "," << heuristic_id << ",";

    if (cost == -1) {
        ss << "null,";
    } else {
        ss << cost << ",";
    }

    ss << "[" << order[0];
    for (int i = 1; i < 4; i++) {
        ss << " " << order[i];
    }
    ss << "]";
    return ss.str();
}

bool satisfiesConstraint(node* n, node* parent, std::set<point> constraints) {
    bool parentFoundGoal = parent == nullptr ? false : parent->data.found_goal;
    return constraints.size() == 0 || (parentFoundGoal || constraints.find(n->pos) != constraints.end());
}

bool satisfiesConstraint(point n, node* parent, std::set<point> constraints) {
    bool parentFoundGoal = parent == nullptr ? false : parent->data.found_goal;
    return constraints.size() == 0 || (parentFoundGoal || constraints.find(n) != constraints.end());
}

node* add_node(std::vector<node*>& tree_nodes, node* parent, point pos, std::set<point> constraints, float cost, float heuristic) {
    tree_nodes.push_back(
        new node{
            visited_info{
                cost,
                heuristic,
                parent == nullptr ? 0 : parent->data.step + 1,
                parent
            },
            {},
            parent,
            pos
        }
    );
    auto new_node = tree_nodes[tree_nodes.size() - 1];
    new_node->data.found_goal = satisfiesConstraint(new_node, parent, constraints);
    if (parent != nullptr) {
        parent->children.push_back(new_node);
    }
    return new_node;
}

void set_block_data(block blocks[space_size][space_size], node n) {
    blocks[n.pos.x][n.pos.y].info.cost = n.data.cost;
    blocks[n.pos.x][n.pos.y].info.heuristic = n.data.heuristic;
    blocks[n.pos.x][n.pos.y].info.step = n.data.step;
    blocks[n.pos.x][n.pos.y].info.from = n.data.from;
}

int count_tree_nodes(node* root) {
    std::vector<node*> stack = { root };
    int count = 0;

    while (!stack.empty()) {
        node* current = stack.back();
        stack.pop_back();
        count++;

        for (node* child : current->children) {
            stack.push_back(child);
        }
    }
    return count;
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
    int visited_qty = 0;

    std::vector<node*> tree_nodes = { };

    add_node(tree_nodes, nullptr, start, {}, 0, -1);

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

            if (next == target) {
                std::cout << "Found target\n";
                auto new_node = add_node(tree_nodes, current, next, {}, costValue, -1);

                if (animate) {
                    set_block_data(blocks, *new_node);
                    animate_path(*new_node, blocks, shouldDraw);
                }

                return generate_log(start, target, visited_qty, count_tree_nodes(tree_nodes[0]), "dfs", cost_id, -1, new_node->data.cost, calculate_path(*new_node), order);
            }

            auto existent = std::find_if(visited.begin(), visited.end(),
                [&](node* n) {
                    return n->pos == next;
                }
            );
            if (existent == visited.end()) {
                node* new_node = add_node(tree_nodes, current, next, {}, costValue, -1);

                stack.push(new_node);
                visited.insert(new_node);

                if (animate) {
                    set_block_data(blocks, *new_node);
                    set_block_colors(blocks, next, shouldDraw);
                }
            }
        }
    }

    if (animate) animate_path(*tree_nodes[tree_nodes.size() - 1], blocks, shouldDraw);

    return generate_log(start, target, visited_qty, count_tree_nodes(tree_nodes[0]), "dfs", cost_id, -1, -1, calculate_path(*tree_nodes[tree_nodes.size() - 1]), order);
}

void print_tree(node* root, int level = 0) {
    if (root == nullptr) {
        return;
    }

    for (int i = 0; i < level; ++i) {
        std::cout << "|   ";
    }

    std::cout << "Node: (" << root->pos.x << ", " << root->pos.y << ")\n";
    for (node* child : root->children) {
        print_tree(child, level + 1);
    }
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
            if (animate) animate_path(*current, blocks, shouldDraw);
            std::vector<node> path = calculate_a_star_path(*current);
            return generate_log(start, target, visited.size(), open.size(), "a_star", cost_id, heuristic_id, current->data.cost, path);
        }

        for (int i = 0; i < 4; i++) {
            point next = { current->pos + dirs[i] };

            if (next.x < 0 || next.x >= space_size || next.y < 0 || next.y >= space_size) {
                continue;
            }

            bool haveGoal = satisfiesConstraint(next, current, constraints);

            auto visited_node = find_if(visited.begin(), visited.end(), [&](node* a) {return a->pos == next;});
            if (visited_node != visited.end()) {
                if (haveGoal && !(*visited_node)->data.found_goal) {
                    open.push_back(*visited_node);
                    visited.erase(visited_node);
                } else {
                    continue;
                }
            }

            float new_cost = current->data.cost + cost(i, current->data.step + 1);

            auto found = find_if(open.begin(), open.end(),
                [&](node* n) {
                    return n->pos == next && (!haveGoal || (haveGoal && n->data.found_goal));
                }
            );

            node* next_node = nullptr;

            if (found == open.end()) {
                next_node = add_node(tree_nodes, current, next, constraints, new_cost, heuristic(next, target));

                open.push_back(next_node);

                if (animate) {
                    set_block_data(blocks, *next_node);
                    set_block_colors(blocks, next, shouldDraw);
                }

                continue;
            } else {
                next_node = *found;
            }
            if (next_node->data.cost > new_cost && (next_node->data.found_goal || !haveGoal)) {
                continue;
            }
            next_node = add_node(tree_nodes, current, next, constraints, new_cost, heuristic(next, target));

            if (animate) {
                set_block_data(blocks, *next_node);
                set_block_colors(blocks, next, shouldDraw);
            }
        }
    }
    // print_tree(tree_nodes[0]);
    return generate_log(start, target, visited.size(), open.size(), "a_star", cost_id, heuristic_id, -1, {}, {});
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
    std::vector<node*> tree_nodes = { };

    add_node(tree_nodes, nullptr, start, {}, 0, 0);

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

            if (next == target) {
                std::cout << "Found target\n" << current->data.cost + 1 << "\n";

                node* new_node = add_node(tree_nodes, current, next, {}, costValue, -1);
                if (animate) {
                    set_block_data(blocks, *new_node);
                    animate_path(*new_node, blocks, shouldDraw);
                }
                return generate_log(start, target, visited_qty, count_tree_nodes(tree_nodes[0]), "bfs", cost_id, -1, new_node->data.cost, calculate_path(*new_node), order);
            }
            auto existent = std::find_if(visited.begin(), visited.end(),
                [&](node* n) {
                    return n->pos == next;
                }
            );
            if (existent == visited.end()) {
                node* new_node = add_node(tree_nodes, current, next, {}, costValue, -1);
                visited.push_back(new_node);
                queue.push(new_node);

                if (animate) {
                    set_block_data(blocks, *new_node);
                    set_block_colors(blocks, next, shouldDraw);
                }
            }
        }
    }
    return generate_log(start, target, visited_qty, count_tree_nodes(tree_nodes[0]), "bfs", cost_id, -1, -1, {}, order);
}

std::string dijkstra(point start,
    point target,
    cost_fn cost,
    block blocks[space_size][space_size],
    bool& shouldDraw,
    int cost_id,
    bool animate
) {
    std::vector<node*> tree_nodes = { };

    add_node(tree_nodes, nullptr, start, {}, 0, 0);

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

        if (current->pos == target) {
            std::cout << "Found target\n";
            if (animate) animate_path(*current, blocks, shouldDraw);
            return generate_log(start, target, visited_qty, count_tree_nodes(tree_nodes[0]), "dijkstra", cost_id, -1, current->data.cost, calculate_path(*current));
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

            if (existent == visited.end() || new_cost < (*existent)->data.cost) {
                node* next_node = add_node(tree_nodes, current, next, {}, new_cost, -1);
                pq.push_back(next_node);
                visited.push_back(next_node);

                if (animate) {
                    set_block_data(blocks, *next_node);
                    set_block_colors(blocks, next, shouldDraw);
                }
            }
        }
    }

    return generate_log(start, target, visited_qty, count_tree_nodes(tree_nodes[0]), "dijkstra", cost_id, -1, -1, {});
}

void animate_greedy_search_cost(std::vector<node*> path, block blocks[space_size][space_size], bool& shouldDraw) {
    for (auto it = path.begin(); it != path.end(); it++) {
        set_block_data(blocks, *(*it));
        set_block_colors(blocks, (*it)->pos, shouldDraw);
    }
}

std::vector<node*> greedy_search_cost(node* target, cost_fn cost, block blocks[space_size][space_size], bool& shouldDraw) {
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
/// @return 
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

        if (current->pos == target) {
            std::cout << "Found target\n";

            auto path = greedy_search_cost(current, cost, blocks, shouldDraw);
            if (animate) {
                animate_greedy_search_cost(path, blocks, shouldDraw);
                animate_path(*current, blocks, shouldDraw);
            }

            return generate_log(start, target, visited_qty, count_tree_nodes(tree_nodes[0]), "greedy_search", cost_id, heuristic_id, current->data.cost, calculate_path(*current));
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
                continue;
            }

            auto new_node = add_node(tree_nodes, current, next, constraints, 0, heuristic(next, target));

            open.push_back(new_node);
            visited.push_back(new_node);

            if (animate) {
                set_block_data(blocks, *new_node);
                set_block_colors(blocks, next, shouldDraw);
            }
        }
    }

    return generate_log(start, target, visited_qty, count_tree_nodes(tree_nodes[0]), "greedy_search", cost_id, heuristic_id, -1, {});
}
