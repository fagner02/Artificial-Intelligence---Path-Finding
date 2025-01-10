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

void calculate_path(point start, point target, block blocks[space_size][space_size], point next, bool& shouldDraw) {
    std::vector<point> path = { target };

    while (target.x != start.x || target.y != start.y) {
        std::cout << target.x << "," << target.y << "\n";
        point last = target;
        bool assigned = false;
        for (int i = 0; i < 4; i++) {
            point next = { last.x + dirs[i].x, last.y + dirs[i].y };
            if (next.x < 0 || next.x >= space_size || next.y < 0 || next.y >= space_size) {
                continue;
            }
            if (!assigned) {
                target = next;
                assigned = true;
            } else {
                if (blocks[next.x][next.y].info.cost < blocks[target.x][target.y].info.cost) {
                    target = next;
                }
            }
        }
        path.push_back(target);

        for (int y = 0; y < space_size; y++) {
            for (int x = 0; x < space_size; x++) {
                point p = { x, y };
                auto it = std::find(path.begin(), path.end(), p);
                if (it != path.end()) {
                    blocks[y][x].shape.setFillColor(sf::Color(100, 100, 200));
                } else {
                    blocks[y][x].shape.setFillColor(sf::Color(100, 200, 100));
                }
            }
        }
        shouldDraw = true;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
