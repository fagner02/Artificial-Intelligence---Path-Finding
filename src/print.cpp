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
/// @brief imprime a grade de blocos no terminal
/// a função limpa a tela e exibe a grade, colorindo os diferentes tipos de blocos, como:
///    o ponto de destino é mostrado com a letra "T".
///    o próximo ponto é destacado com a letra "X" em azul.
///    o ponto anterior é destacado com a letra "P" em vermelho.
///    os blocos com custo não calculado são exibidos com o valor "-1".
///    os blocos visitados e com custo calculado são exibidos em verde.
/// @param target ponto de destino
/// @param next próximo ponto a ser visitado
/// @param prev ponto anterior
/// @param visited grade de blocos com os dados de custo e heurística
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

// usado para animar o caminho
void animatePath(node target, block blocks[space_size][space_size], bool& shouldDraw) {
    std::vector<node> path = calculatePath(target);

    for (auto& node : path) {
        blocks[node.pos.x][node.pos.y].info.cost = node.data.cost;
        blocks[node.pos.x][node.pos.y].info.is_path = true;
        setBlockColors(blocks, shouldDraw);
    }
}

// usado para animar o caminho
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

/// @brief gera um log contendo informações sobre a execução do algoritmo de busca
/// a função cria uma string que descreve o desempenho do algoritmo, incluindo o número de nós visitados,
/// gerados, o caminho encontrado, o custo final e outros parâmetros de execução.
/// @param start ponto inicial da busca
/// @param target ponto final da busca
/// @param visited_qty quantidade de nós visitados durante a busca
/// @param generated_qty quantidade de nós gerados durante a busca
/// @param algorithm algoritmo de busca utilizado
/// @param cost_id id da função de custo utilizado
/// @param heuristic_id id da função de heuristica utilizado
/// @param cost função de custo
/// @param path caminho encontrado
/// @param order ordem de busca
/// @param constraints posições das farmácias
/// @return string formatada contendo os dados do log
std::string generate_log(
    point start,
    point target,
    int visited_qty,
    int generated_qty,
    std::string algorithm,
    int cost_id,
    int heuristic_id,
    float cost,
    std::vector<node> path,
    std::vector<int> order,
    std::set<point> constraints
) {
    std::stringstream ss;
    ss << algorithm << "," << visited_qty << "," << generated_qty << "," << path.size() << ",";
    if (path.size() == 0) {
        ss << "null,";
    } else {
        ss << "[";
        for (auto p : path) {
            ss << "[" << p.pos.x << " " << p.pos.y << "]";
        }
        ss << "],";
    }

    ss << "[" << start.x << " " << start.y << "], [" << target.x << " " << target.y << "],";
    ss << cost_id << "," << heuristic_id << ",";

    if (cost == -1) {
        ss << "null,";
    } else {
        ss << cost << ",";
    }

    ss << "[" << order[0];
    for (int i = 1; i < 4; i++) {
        ss << " " << order[i];
    }
    ss << "],";
    ss << "[";
    for (auto& p : constraints) {
        ss << "[" << p.x << " " << p.y << "] ";
    }
    ss << "]";
    return ss.str();
}

// para animar a busca gulosa
void animate_greedy_search_cost(std::vector<node*> path, block blocks[space_size][space_size], bool& shouldDraw) {
    for (auto it = path.begin(); it != path.end(); it++) {
        set_block_data(blocks, *(*it));
        setBlockColors(blocks, shouldDraw, (*it)->pos);
    }
}