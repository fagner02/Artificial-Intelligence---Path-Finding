#include <print.h>

void printPath(std::vector<point>& path, block visited[space_size][space_size]) {
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

std::vector<node> calculatePath(node target) {
    std::vector<node> path = {  };

    while (true) {
        path.emplace_back(target);
        if (target.parent == nullptr) {
            break;
        }
        target = *target.parent;
    }
    return path;
}

void animatePath(node target, block blocks[space_size][space_size], bool& shouldDraw) {
    std::vector<node> path = calculatePath(target);

    for (auto& node : path) {
        blocks[node.pos.x][node.pos.y].info.cost = node.data.cost;
        blocks[node.pos.x][node.pos.y].info.is_path = true;
        setBlockColors(blocks, shouldDraw);
    }
}

void  setBlockColors(
    block blocks[space_size][space_size],
    bool& shouldDraw,
    point next
) {
    for (int i = 0; i < space_size; i++) {
        for (int j = 0; j < space_size; j++) {
            if (i == next.x && j == next.y) {
                blocks[i][j].shape.setFillColor(sf::Color(255, 0, 0));
            } else if (blocks[i][j].info.label == "G") {
                blocks[i][j].shape.setFillColor(sf::Color(0, 100, 0));
            } else if (blocks[i][j].info.label == "S") {
                blocks[i][j].shape.setFillColor(sf::Color(10, 10, 105));
            } else if (blocks[i][j].info.label == "T") {
                blocks[i][j].shape.setFillColor(sf::Color(230, 150, 15));
            } else if (blocks[i][j].info.is_path) {
                blocks[i][j].shape.setFillColor(sf::Color(100, 100, 200));
            } else if (blocks[i][j].info.cost != -1) {
                blocks[i][j].shape.setFillColor(sf::Color(100, 200, 100));
            } else {
                blocks[i][j].shape.setFillColor(sf::Color(100, 100, 100));
            }
        }
    }
    shouldDraw = true;

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

}

