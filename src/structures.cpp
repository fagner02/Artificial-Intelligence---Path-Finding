#include <structures.h>

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

void fill_blocks(
    block blocks[space_size][space_size],
    std::set<point> goals,
    point start,
    point target
) {
    for (int i = 0; i < space_size; i++) {
        for (int j = 0; j < space_size; j++) {
            blocks[i][j].info.cost = -1;
            blocks[i][j].info.heuristic = -1;
            blocks[i][j].info.from = nullptr;
            if (goals.find(point{ i, j }) != goals.end()) {
                blocks[i][j].info.label = "G";
            }
            if (start == point{ i,j }) {
                blocks[i][j].info.label = "S";
            }
            if (target == point{ i,j }) {
                blocks[i][j].info.label = "T";
            }
        }
    }
}