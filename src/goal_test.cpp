#include <goal_test.h>

// testa se um nó satisfaz as condições dadas
bool satisfiesConstraint(node* n, std::set<point> constraints) {
    return constraints.size() == 0 || (n->data.found_goal || constraints.find(n->pos) != constraints.end());
}

bool satisfiesConstraint(point n, node* parent, std::set<point> constraints) {
    bool parentFoundGoal = parent == nullptr ? false : parent->data.found_goal;
    return constraints.size() == 0 || (parentFoundGoal || constraints.find(n) != constraints.end());
}

// testa se um ponto deve ser revisitado
bool shouldRevisit(node* old, point new_node, node* parent, std::set<point> constraints) {
    return !satisfiesConstraint(old, constraints) && satisfiesConstraint(new_node, parent, constraints);
}

bool shouldRevisit(node* old, node* new_node, std::set<point> constraints) {
    return !satisfiesConstraint(old, constraints) && satisfiesConstraint(new_node, constraints);
}

// testa se um nó é o ponto de parada do algoritmo
bool isGoal(node* n, point target, std::set<point> constraints) {
    return (n->pos == target) && satisfiesConstraint(n, constraints);
}

bool isGoal(point n, point target, node* parent, std::set<point> constraints) {
    return (n == target) && satisfiesConstraint(n, parent, constraints);
}
