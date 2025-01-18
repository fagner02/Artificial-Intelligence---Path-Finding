#include <heuristics.h>

// distancia nula
float heuristic0(point a, point b) {
    return 0.0;
}

// distância euclidiana
float heuristic1(point a, point b) {
    return 10.0 * sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

// distância de manhattan
float heuristic2(point a, point b) {
    return 10.0 * (abs(a.x - b.x) + abs(a.y - b.y));
}