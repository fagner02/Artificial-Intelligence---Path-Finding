#include <node.h>

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

void print_tree(node* root, int level) {
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
    bool parentFoundGoal = parent == nullptr ? false : parent->data.found_goal;
    new_node->data.found_goal = parentFoundGoal || satisfiesConstraint(new_node, constraints);
    if (parent != nullptr) {
        parent->children.push_back(new_node);
    }
    return new_node;
}