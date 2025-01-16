
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
        target = blocks[target.x][target.y].info.from->pos;
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

void set_block_data(block blocks[space_size][space_size], node n) {
    blocks[n.pos.x][n.pos.y].info.cost = n.data.cost;
    blocks[n.pos.x][n.pos.y].info.heuristic = n.data.heuristic;
    blocks[n.pos.x][n.pos.y].info.step = n.data.step;
    blocks[n.pos.x][n.pos.y].info.from = n.data.from;
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

    std::vector<node*> tree_nodes = { new node{{-1,-1,-1},{}, nullptr, start } };

    std::stack<node*> stack;
    stack.push(tree_nodes[0]);
    generated_qty++;
    tree_nodes[0]->data.cost = 0;
    tree_nodes[0]->data.step = 0;
    tree_nodes[0]->data.from = nullptr;
    set_block_data(blocks, *tree_nodes[0]);

    while (!stack.empty()) {
        node* current = stack.top();
        stack.pop();

        for (int i = 0; i < 4; i++) {
            int dir = order[i];
            point next = { current->pos + dirs[dir] };
            if (next.x < 0 || next.x >= space_size || next.y < 0 || next.y >= space_size) {
                continue;
            }

            float costValue = cost(dir, current->data.step + 1);

            auto existent = std::find_if(tree_nodes.begin(), tree_nodes.end(), [&](node* n) {
                return n->pos == next;
                });

            if (next == target) {
                std::cout << "Found target\n" << current->data.cost + 1 << "\n";
                tree_nodes.push_back(
                    new node{
                        visited_info{-1,-1,-1},
                        {},
                        current,
                    next
                    }
                );
                node* new_node = tree_nodes[tree_nodes.size() - 1];
                new_node->data.cost = current->data.cost + costValue;
                new_node->data.step = current->data.step + 1;
                new_node->data.from = current;
                current->children.push_back(new_node);
                set_block_data(blocks, *new_node);
                visited_qty++;
                generated_qty++;

                if (animate) calculate_path(*tree_nodes[0], *new_node, blocks, shouldDraw);

                for (int i = 0; i < tree_nodes.size(); i++) {
                    std::cout << "Deleting node\n";
                    delete tree_nodes[i];
                }
                return "";
            }
            if (existent == tree_nodes.end()) {
                tree_nodes.push_back(
                    new node{
                        visited_info{-1,-1,-1},
                        {},
                        current,
                    next
                    }
                );
                node* new_node = tree_nodes[tree_nodes.size() - 1];
                current->children.push_back(new_node);
                stack.push(new_node);
                generated_qty++;
                new_node->data.cost = current->data.cost + costValue;
                new_node->data.step = current->data.step + 1;
                new_node->data.from = current;
                set_block_data(blocks, *new_node);
                if (animate) set_block_colors(blocks, next, shouldDraw);
            }

            visited_qty++;
        }
    }

    if (animate) calculate_path(*tree_nodes[0], *tree_nodes[tree_nodes.size() - 1], blocks, shouldDraw);

    return "null";
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
    block blocks[space_size][space_size],
    bool& shouldDraw,
    int cost_id,
    int heuristic_id,
    bool animate,
    std::set<point> goals
) {
    std::vector <node*> tree_nodes = {
        new node{
            visited_info{-1,-1,-1,nullptr},
            {},
            nullptr,
            start
        }
    };
    std::vector<node*> open;
    std::vector<node*> closed;

    int visited_qty = 1;
    int generated_qty = 0;

    tree_nodes[0]->data.cost = 0;
    tree_nodes[0]->data.heuristic = heuristic(start, target);
    tree_nodes[0]->data.step = 0;
    tree_nodes[0]->data.from = nullptr;
    set_block_data(blocks, *tree_nodes[0]);

    open.push_back(tree_nodes[0]);
    generated_qty++;

    while (!open.empty()) {
        auto min = min_element(open.begin(), open.end(), [&](node* a, node* b) {
            return a->data.cost + heuristic(a->pos, target) < b->data.cost + heuristic(b->pos, target);
            }
        );

        node* current = *min;
        open.erase(min);

        closed.push_back(current);
        generated_qty++;

        if (current->pos == target && current->data.found_goal) {
            std::cout << "Found target\n";
            // print_tree(tree_nodes[0]);
            if (animate) calculate_path(*current, blocks, shouldDraw);
            return "";
            //return generate_log(start, target, closed.size(), open.size(), blocks, "a_star", cost_id, heuristic_id, current->data.cost);
        }

        for (int i = 0; i < 4; i++) {
            point next = { current->pos + dirs[i] };
            if (next.x < 0 || next.x >= space_size || next.y < 0 || next.y >= space_size) {
                continue;
            }

            bool haveGoal = current->data.found_goal || goals.find(next) != goals.end();

            auto closed_node = find_if(closed.begin(), closed.end(), [&](node* a) {return a->pos == next;});
            if (closed_node != closed.end()) {
                if (haveGoal && !(*closed_node)->data.found_goal) {
                    open.push_back(*closed_node);
                    closed.erase(closed_node);
                } else {
                    continue;
                }
            }
            visited_qty++;
            float new_cost = current->data.cost + cost(i, current->data.step + 1);

            auto found = find_if(
                open.begin(),
                open.end(),
                [&](node* n) { return n->pos == next && (!haveGoal || (haveGoal && n->data.found_goal));  }
            );

            node* next_node = nullptr;

            if (found == open.end()) {
                tree_nodes.push_back(
                    new node{
                        visited_info{-1,-1,-1, nullptr, haveGoal},
                        {},
                        current,
                        next
                    }
                );
                next_node = tree_nodes[tree_nodes.size() - 1];
                open.push_back(next_node);
                generated_qty++;
                current->children.push_back(next_node);
                next_node->parent = current;
                next_node->data.step = current->data.step + 1;
                next_node->data.cost = new_cost;
                next_node->data.heuristic = heuristic(next, target);
                next_node->data.from = current;
                next_node->data.found_goal = next_node->data.found_goal || haveGoal;
                set_block_data(blocks, *next_node);
                continue;
            } else {
                next_node = *found;
            }
            if (next_node->data.cost > new_cost && (next_node->data.found_goal || !haveGoal)) {
                continue;
            }
            tree_nodes.push_back(
                new node{
                    visited_info{-1,-1,-1, nullptr, haveGoal},
                    {},
                    current,
                    next
                }
            );
            next_node = tree_nodes[tree_nodes.size() - 1];
            current->children.push_back(next_node);
            next_node->parent = current;
            next_node->data.cost = new_cost;
            next_node->data.step = current->data.step + 1;
            next_node->data.heuristic = heuristic(next, target);
            next_node->data.from = current;
            next_node->data.found_goal = next_node->data.found_goal || haveGoal;
            set_block_data(blocks, *next_node);

            if (animate) set_block_colors(blocks, next, shouldDraw);
        }
    }
    // print_tree(tree_nodes[0]);
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

    std::vector<node*> tree_nodes = {
        new node {
            visited_info { -1, -1, -1},
            {},
            nullptr,
            start
        }
    };

    std::queue<node*> queue;
    queue.push(tree_nodes[0]);
    generated_qty++;

    tree_nodes[0]->data.cost = 0;
    tree_nodes[0]->data.step = 0;
    tree_nodes[0]->data.from = nullptr;
    set_block_data(blocks, *tree_nodes[0]);

    while (!queue.empty()) {
        node* current = queue.front();
        queue.pop();

        for (int i = 0; i < 4; i++) {
            int dir = order[i];
            point next = current->pos + dirs[dir];
            if (next.x < 0 || next.x >= space_size || next.y < 0 || next.y >= space_size) {
                continue;
            }

            float costValue = current->data.cost + cost(dir, current->data.step + 1);

            auto existent = std::find_if(tree_nodes.begin(), tree_nodes.end(), [&](node* n) {
                return n->pos == next;
                });
            if (next == target) {
                std::cout << "Found target\n" << current->data.cost + 1 << "\n";

                tree_nodes.push_back(
                    new node{
                        visited_info{-1,-1,-1},
                        {},
                        current,
                    next
                    }
                );

                node* new_node = tree_nodes[tree_nodes.size() - 1];

                new_node->data.cost = costValue;
                new_node->data.step = current->data.step + 1;
                new_node->data.from = current;
                set_block_data(blocks, *new_node);
                if (animate) calculate_path(*tree_nodes[0], *new_node, blocks, shouldDraw);
                return "";
                // return generate_log(start, target, visited_qty, generated_qty, blocks, "bfs", cost_id, -1, new_node->data.cost, order);
            }

            if (existent == tree_nodes.end()) {
                std::cout << "null\n";
                tree_nodes.push_back(
                    new node{
                        visited_info{-1,-1,-1},
                        {},
                        current,
                    next
                    }
                );

                node* new_node = tree_nodes[tree_nodes.size() - 1];
                current->children.push_back(new_node);

                visited_qty++;
                queue.push(new_node);
                generated_qty++;
                new_node->data.cost = costValue;
                new_node->data.step = current->data.step + 1;
                new_node->data.from = current;
                set_block_data(blocks, *new_node);

                if (animate)  set_block_colors(blocks, next, shouldDraw);
            } else {
                std::cout << "nao\n";
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
    std::vector<node*> tree_nodes = {
        new node{
            visited_info{-1,-1,-1},
            {},
            nullptr,
        start
        }
    };
    std::vector<node*> pq;

    tree_nodes[0]->data.cost = 0;
    tree_nodes[0]->data.step = 0;
    tree_nodes[0]->data.from = nullptr;
    set_block_data(blocks, *tree_nodes[0]);

    int visited_qty = 1;
    int generated_qty = 0;

    pq.push_back(tree_nodes[0]);
    generated_qty++;

    while (!pq.empty()) {
        auto min = min_element(pq.begin(), pq.end(), [&](node* a, node* b) {
            return a->data.cost < b->data.cost;
            });

        node* current = *min;
        pq.erase(min);

        if (current->pos == target) {
            std::cout << "Found target\n";
            if (animate) calculate_path(*tree_nodes[0], *current, blocks, shouldDraw);
            return "";
            // return generate_log(start, target, visited_qty, generated_qty, blocks, "dijkstra", cost_id, -1, current->data.cost);
        }

        for (int i = 0; i < 4; i++) {
            point next = { current->pos + dirs[i] };

            if (next.x < 0 || next.x >= space_size || next.y < 0 || next.y >= space_size) {
                continue;
            }

            visited_qty++;

            auto existent = std::find_if(tree_nodes.begin(), tree_nodes.end(), [&](node* n) {
                return n->pos == next;
                });

            float costValue = cost(i, current->data.step + 1);
            float new_cost = current->data.cost + costValue;

            if (existent == tree_nodes.end() || new_cost < (*existent)->data.cost) {
                tree_nodes.push_back(
                    new node{
                        visited_info{-1,-1,-1},
                        {},
                        current,
                    next
                    }
                );

                node* next_node = tree_nodes[tree_nodes.size() - 1];

                next_node->data.cost = new_cost;
                next_node->data.step = current->data.step + 1;
                next_node->data.from = current;
                pq.push_back(next_node);
                set_block_data(blocks, *next_node);
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
    block blocks[space_size][space_size],
    bool& shouldDraw,
    int cost_id,
    int heuristic_id,
    bool animate
) {
    std::vector<node*> tree_nodes = {
        new node{
            visited_info{-1,-1,-1},
            {},
            nullptr,
        start
        }
    };
    std::vector<node*> open;

    int visited_qty = 1;
    int generated_qty = 0;

    tree_nodes[0]->data.cost = 0;
    tree_nodes[0]->data.heuristic = heuristic(start, target);
    tree_nodes[0]->data.from = nullptr;
    set_block_data(blocks, *tree_nodes[0]);

    open.push_back(tree_nodes[0]);
    generated_qty++;

    while (!open.empty()) {
        auto min_it = min_element(open.begin(), open.end(), [&](node* a, node* b) {
            return a->data.heuristic < b->data.heuristic;
            });

        node* current = *min_it;
        open.erase(min_it);

        if (current->pos == target) {
            std::cout << "Found target\n";
            std::vector<node*> path = {  };
            auto curr = current;

            while (curr != nullptr) {
                path.push_back(curr);
                curr = curr->data.from;
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
                set_block_data(blocks, *(*next));
                set_block_colors(blocks, (*next)->pos, shouldDraw);
            }

            if (animate) calculate_path(*tree_nodes[0], *current, blocks, shouldDraw);
            return "";
            // return generate_log(start, target, visited_qty, generated_qty, blocks, "greedy_search", cost_id, heuristic_id, current->data.cost);
        }

        for (int i = 0; i < 4; i++) {
            point next = current->pos + dirs[i];

            if (next.x < 0 || next.x >= space_size || next.y < 0 || next.y >= space_size) {
                continue;
            }

            visited_qty++;

            auto existent = std::find_if(tree_nodes.begin(), tree_nodes.end(), [&](node* n) {
                return n->pos == next;
                });
            if (existent != tree_nodes.end()) {
                continue;
            }

            tree_nodes.push_back(
                new node{
                    visited_info{ -1, -1, -1 },
                    {},
                    current,
                    next
                }
            );
            auto next_node = tree_nodes[tree_nodes.size() - 1];
            next_node->data.heuristic = heuristic(next, target);
            next_node->data.from = current;
            next_node->data.cost = 0;
            set_block_data(blocks, *next_node);

            open.push_back(next_node);
            generated_qty++;
            if (animate) set_block_colors(blocks, next, shouldDraw);
        }
    }

    return "null";
}
