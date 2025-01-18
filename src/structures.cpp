#include <structures.h>

void fill_blocks(
    block blocks[space_size][space_size],
    std::set<point> constraints,
    point start,
    point target
) {
    for (int i = 0; i < space_size; i++) {
        for (int j = 0; j < space_size; j++) {
            blocks[i][j].info.cost = -1;
            blocks[i][j].info.heuristic = -1;
            blocks[i][j].info.from = nullptr;
            blocks[i][j].info.label = "";
            blocks[i][j].info.is_path = false;
            if (constraints.find(point{ i, j }) != constraints.end()) {
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

void set_block_data(block blocks[space_size][space_size], node n) {
    blocks[n.pos.x][n.pos.y].info.cost = n.data.cost;
    blocks[n.pos.x][n.pos.y].info.heuristic = n.data.heuristic;
    blocks[n.pos.x][n.pos.y].info.step = n.data.step;
    blocks[n.pos.x][n.pos.y].info.from = n.data.from;
}