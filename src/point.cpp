#include <point.h>

// sobrecarga do operador de igualdade(==) entre dois pontos
bool operator==(const point& lhs, const point& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

// sobrecarga do operador de soma(+) entre dois pontos
point operator+(const point& lhs, const point& rhs) {
    return { lhs.x + rhs.x , lhs.y + rhs.y };
}

// sobrecarga do operador de subtração(-) entre dois pontos
point operator-(const point& lhs, const point& rhs) {
    return { lhs.x - rhs.x , lhs.y - rhs.y };
}

// sobrecarga do operador de comparação de desigualdade(!=) entre dois pontos
bool operator!=(const point& lhs, const point& rhs) {
    return lhs.x != rhs.x || lhs.y != rhs.y;
}

// sobrecarga do operador de comparação menir que(<) entre dois pontos
bool operator<(const point& lhs, const point& rhs) {
    return lhs.x < rhs.x || (lhs.x == rhs.x && lhs.y < rhs.y);
}