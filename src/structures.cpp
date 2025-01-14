#include <structures.h>

bool operator==(const point& lhs, const point& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

void fill_blocks(
    block blocks[space_size][space_size]
) {
    for (int i = 0; i < space_size; i++) {
        for (int j = 0; j < space_size; j++) {
            blocks[i][j].info.cost = -1;
            blocks[i][j].info.heuristic = -1;
            blocks[i][j].info.from = { -1, -1 };
        }
    }
}