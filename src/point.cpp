#include <point.h>

bool operator==(const point& lhs, const point& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

point operator+(const point& lhs, const point& rhs) {
    return { lhs.x + rhs.x , lhs.y + rhs.y };
}

point operator-(const point& lhs, const point& rhs) {
    return { lhs.x - rhs.x , lhs.y - rhs.y };
}

bool operator!=(const point& lhs, const point& rhs) {
    return lhs.x != rhs.x || lhs.y != rhs.y;
}

bool operator<(const point& lhs, const point& rhs) {
    return lhs.x < rhs.x || (lhs.x == rhs.x && lhs.y < rhs.y);
}