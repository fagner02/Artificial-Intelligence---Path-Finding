#include <print.h>

void print_path(std::vector<point>& path, block visited[space_size][space_size]) {
    std::cout << "\x1b[H\x1b[J" << std::flush;
    for (int y = space_size - 1; y > -1; y--) {
        for (int x = 0; x < space_size; x++) {
            point p = { x, y };
            auto it = std::find(path.begin(), path.end(), p);
            if (it != path.end()) {
                std::cout << "\033[32;1m" << std::setw(4) << visited[x][y].info.cost << "\033[0m" << std::setw(4) << " | ";
            } else {
                std::cout << std::setw(4) << visited[x][y].info.cost << std::setw(4) << " | ";
            }
        }
        std::cout << "\n";
    }
}

void print(point target, point next, point prev, block visited[space_size][space_size]) {
    std::cout << "\x1b[H\x1b[J" << std::flush;
    for (int i = 0; i < space_size; i++) {
        for (int j = 0; j < space_size; j++) {
            if (i == next.x && j == next.y) {
                std::cout << "\033[1;36m" << std::setw(4) << "X" << "\033[0m" << std::setw(4) << " | ";
            } else if (i == prev.x && j == prev.y) {
                std::cout << "\033[1;31m" << std::setw(4) << "P" << "\033[0m" << std::setw(4) << " | ";
            } else if (i == target.x && j == target.y) {
                std::cout << std::setw(4) << "T" << std::setw(4) << " | ";
            } else if (visited[i][j].info.cost == -1) {
                std::cout << std::setw(4) << visited[i][j].info.cost << std::setw(4) << " | ";
            } else {
                std::cout << "\033[1;32m" << std::setw(4) << visited[i][j].info.cost << "\033[0m" << std::setw(4) << " | ";
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

void set_path_block_colors(
    block blocks[space_size][space_size],
    std::vector<node> path,
    bool& shouldDraw
) {
    for (int y = 0; y < space_size; y++) {
        for (int x = 0; x < space_size; x++) {
            point p = { x, y };
            auto it = std::find_if(path.begin(), path.end(), [&](node a) { return a.pos == p;});
            auto yindex = y;
            if (it != path.end()) {
                blocks[x][yindex].shape.setFillColor(sf::Color(100, 100, 200));
            } else if (blocks[x][yindex].info.cost != -1) {
                blocks[x][yindex].shape.setFillColor(sf::Color(100, 200, 100));
            } else {
                blocks[x][yindex].shape.setFillColor(sf::Color(100, 100, 100));
            }
        }
    }
    shouldDraw = true;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

void calculate_path(node start, node target, block blocks[space_size][space_size], bool& shouldDraw) {
    std::vector<node> path = {  };

    while (true) {
        path.push_back(target);
        set_path_block_colors(blocks, path, shouldDraw);
        if (target.data.from == nullptr) {
            break;
        }
        target = *(target.data.from);
    }
}

void  set_block_colors(
    block blocks[space_size][space_size],
    point next,
    bool& shouldDraw
) {
    for (int i = 0; i < space_size; i++) {
        for (int j = 0; j < space_size; j++) {
            if (i == next.x && j == next.y) {
                blocks[i][j].shape.setFillColor(sf::Color(255, 0, 0));
            } else if (blocks[i][j].info.cost != -1) {
                blocks[i][j].shape.setFillColor(sf::Color(100, 200, 100));
            } else {
                blocks[i][j].shape.setFillColor(sf::Color(100, 100, 100));
            }
        }
    }
    shouldDraw = true;

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

}

