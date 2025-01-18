#include <set>
#include <node.h>

#pragma once

bool satisfiesConstraint(node* n, std::set<point> constraints);

bool satisfiesConstraint(point n, node* parent, std::set<point> constraints);

bool shouldRevisit(node* old, point new_node, node* parent, std::set<point> constraints);

bool shouldRevisit(node* old, node* new_node, std::set<point> constraints);

bool isGoal(node* n, point target, std::set<point> constraints);

bool isGoal(point n, point target, node* parent, std::set<point> constraints);