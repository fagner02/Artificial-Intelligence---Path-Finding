#include <experiments.h>

void experiment1() {
    block blocks[space_size][space_size];
    bool shouldDraw = false;
    fill_blocks(blocks);
    std::ofstream file("log1.csv");
    if (!file.is_open()) {
        std::cout << "Erro ao abrir o arquivo log.csv\n";
        return;
    }
    file << "algorithm,visited_qty,generated_qty,path_size,path,start,target,cost_id,heuristic_id,cost,order,pharmacies\n";

    srand((unsigned)time(NULL));

    for (int i = 0; i < 50; i++) {
        point start = { rand() % 31, rand() % 31 };
        point target = { rand() % 31, rand() % 31 };

        for (int j = 0; j < 4; j++) {
            auto cost = costs[j];
            fill_blocks(blocks);
            file << dijkstra(start, target, cost, blocks, std::ref(shouldDraw), j, {}) << "\n";
            fill_blocks(blocks);
            file << bfs(start, target, cost, blocks, std::ref(shouldDraw), j, {}) << "\n";
            fill_blocks(blocks);
            file << dfs(start, target, cost, j, {}, blocks, std::ref(shouldDraw)) << "\n";
        }
    }
}

void experiment1(
    std::vector<point> start_points,
    std::vector<point> target_points,
    std::function<void(std::string)> toastText
) {
    if (start_points.size() != 50 || target_points.size() != 50) {
        toastText("start_points e target_points devem ter 50 elementos\n");
        return;
    }
    block blocks[space_size][space_size];
    bool shouldDraw = false;
    fill_blocks(blocks);
    std::ofstream file("log1.csv");
    if (!file.is_open()) {
        toastText("Erro ao abrir o arquivo log.csv\n");
        return;
    }
    file << "algorithm,visited_qty,generated_qty,path_size,path,start,target,cost_id,heuristic_id,cost,order,pharmacies\n";
    for (int i = 0; i < 50; i++) {
        point start = start_points[i];
        point target = target_points[i];

        for (int j = 0; j < 4; j++) {
            auto cost = costs[j];
            fill_blocks(blocks);
            file << dijkstra(start, target, cost, blocks, std::ref(shouldDraw), j, {}) << "\n";
            fill_blocks(blocks);
            file << bfs(start, target, cost, blocks, std::ref(shouldDraw), j, {}) << "\n";
            fill_blocks(blocks);
            file << dfs(start, target, cost, j, {}, blocks, std::ref(shouldDraw)) << "\n";
        }
    }
}

void experiment2() {
    block blocks[space_size][space_size];
    bool shouldDraw = false;
    fill_blocks(blocks);
    std::ofstream file1("log2.csv");
    if (!file1.is_open()) {
        std::cout << "Erro ao abrir o arquivo log.csv\n";
        return;
    }
    file1 << "algorithm,visited_qty,generated_qty,path_size,path,start,target,cost_id,heuristic_id,cost,order,pharmacies\n";

    srand((unsigned)time(NULL));

    for (int i = 0; i < 50; i++) {
        point start = { rand() % 31, rand() % 31 };
        point target = { rand() % 31, rand() % 31 };

        for (int j = 0; j < 4; j++) {
            auto cost = costs[j];
            fill_blocks(blocks);
            file1 << dijkstra(start, target, cost, blocks, std::ref(shouldDraw), j, {}) << "\n";
            for (int k = 0; k < 2; k++) {
                fill_blocks(blocks);
                file1 << a_star(start, target, cost, heuristic_fns[k], j, k, {}, blocks, std::ref(shouldDraw)) << "\n";
            }
        }
    }
}

void experiment2(
    std::vector<point> start_points,
    std::vector<point> target_points,
    std::function<void(std::string)> toastText
) {
    if (start_points.size() != 50 || target_points.size() != 50) {
        toastText("start_points e target_points devem ter 50 elementos\n");
        return;
    }
    block blocks[space_size][space_size];
    bool shouldDraw = false;
    fill_blocks(blocks);
    std::ofstream file1("log2.csv");
    if (!file1.is_open()) {
        toastText("Erro ao abrir o arquivo log.csv\n");
        return;
    }
    file1 << "algorithm,visited_qty,generated_qty,path_size,path,start,target,cost_id,heuristic_id,cost,order,pharmacies\n";

    for (int i = 0; i < 50; i++) {
        point start = start_points[i];
        point target = target_points[i];

        for (int j = 0; j < 4; j++) {
            auto cost = costs[j];
            fill_blocks(blocks);
            file1 << dijkstra(start, target, cost, blocks, std::ref(shouldDraw), j, {}) << "\n";
            for (int k = 0; k < 2; k++) {
                fill_blocks(blocks);
                file1 << a_star(start, target, cost, heuristic_fns[k], j, k, {}, blocks, std::ref(shouldDraw)) << "\n";
            }
        }
    }
}

void experiment3() {
    block blocks[space_size][space_size];
    bool shouldDraw = false;
    fill_blocks(blocks);
    std::ofstream file2("log3.csv");
    if (!file2.is_open()) {
        std::cout << "Erro ao abrir o arquivo log.csv\n";
        return;
    }

    file2 << "algorithm,visited_qty,generated_qty,path_size,path,start,target,cost_id,heuristic_id,cost,order,pharmacies\n";

    srand((unsigned)time(NULL));

    for (int i = 0; i < 50; i++) {
        point start = { rand() % 31, rand() % 31 };
        point target = { rand() % 31, rand() % 31 };

        for (int j = 0; j < 4; j++) {
            auto cost = costs[j];

            for (int k = 0; k < 2; k++) {
                fill_blocks(blocks);
                file2 << greedy_search(start, target, cost, heuristic_fns[k], j, k, {}, blocks, std::ref(shouldDraw)) << "\n";
                fill_blocks(blocks);
                file2 << a_star(start, target, cost, heuristic_fns[k], j, k, {}, blocks, std::ref(shouldDraw)) << "\n";
            }
        }
    }
}


void experiment3(
    std::vector<point> start_points,
    std::vector<point> target_points,
    std::function<void(std::string)> toastText
) {
    if (start_points.size() != 50 || target_points.size() != 50) {
        toastText("start_points e target_points devem ter 50 elementos\n");
        return;
    }
    block blocks[space_size][space_size];
    bool shouldDraw = false;
    fill_blocks(blocks);
    std::ofstream file2("log3.csv");
    if (!file2.is_open()) {
        toastText("Erro ao abrir o arquivo log.csv\n");
        return;
    }

    file2 << "algorithm,visited_qty,generated_qty,path_size,path,start,target,cost_id,heuristic_id,cost,order,pharmacies\n";

    srand((unsigned)time(NULL));

    for (int i = 0; i < 50; i++) {
        point start = start_points[i];
        point target = start_points[i];

        for (int j = 0; j < 4; j++) {
            auto cost = costs[j];

            for (int k = 0; k < 2; k++) {
                fill_blocks(blocks);
                file2 << greedy_search(start, target, cost, heuristic_fns[k], j, k, {}, blocks, std::ref(shouldDraw)) << "\n";
                fill_blocks(blocks);
                file2 << a_star(start, target, cost, heuristic_fns[k], j, k, {}, blocks, std::ref(shouldDraw)) << "\n";
            }
        }
    }
}

void experiment4() {
    block blocks[space_size][space_size];
    bool shouldDraw = false;
    fill_blocks(blocks);
    std::ofstream file3("log4.csv");
    if (!file3.is_open()) {
        std::cout << "Erro ao abrir o arquivo log.csv\n";
        return;
    }

    file3 << "algorithm,visited_qty,generated_qty,path_size,path,start,target,cost_id,heuristic_id,cost,order,pharmacies\n";

    srand((unsigned)time(NULL));

    for (int i = 0; i < 20; i++) {
        point start = { rand() % 31, rand() % 31 };
        point target = { rand() % 31, rand() % 31 };

        std::vector<int> order = { 0, 1, 2, 3 };
        std::random_shuffle(order.begin(), order.end());
        for (int j = 0; j < 4; j++) {
            auto cost = costs[j];

            fill_blocks(blocks);
            file3 << bfs(start, target, cost, blocks, std::ref(shouldDraw), j, {}, order) << "\n";
            fill_blocks(blocks);
            file3 << dfs(start, target, cost, j, {}, blocks, std::ref(shouldDraw), order) << "\n";
        }
    }
}

void experiment4(
    std::vector<point> start_points,
    std::vector<point> target_points,
    std::vector<std::vector<int>> orders,
    std::function<void(std::string)> toastText
) {
    if (start_points.size() != 20 || target_points.size() != 20 || orders.size() != 20) {
        toastText("start_points, target_points e orders devem ter 20 elementos\n");
        return;
    }
    block blocks[space_size][space_size];
    bool shouldDraw = false;
    fill_blocks(blocks);
    std::ofstream file3("log4.csv");
    if (!file3.is_open()) {
        toastText("Erro ao abrir o arquivo log.csv\n");
        return;
    }

    file3 << "algorithm,visited_qty,generated_qty,path_size,path,start,target,cost_id,heuristic_id,cost,order,pharmacies\n";

    for (int i = 0; i < 20; i++) {
        point start = start_points[i];
        point target = target_points[i];

        std::vector<int> order = orders[i];
        for (int j = 0; j < 4; j++) {
            auto cost = costs[j];

            fill_blocks(blocks);
            file3 << bfs(start, target, cost, blocks, std::ref(shouldDraw), j, {}, order) << "\n";
            fill_blocks(blocks);
            file3 << dfs(start, target, cost, j, {}, blocks, std::ref(shouldDraw), order) << "\n";
        }
    }
}

void experiment5() {
    block blocks[space_size][space_size];
    bool shouldDraw = false;
    fill_blocks(blocks);
    std::ofstream file4("log5.csv");
    if (!file4.is_open()) {
        std::cout << "Erro ao abrir o arquivo log.csv\n";
        return;
    }

    file4 << "algorithm,visited_qty,generated_qty,path_size,path,start,target,cost_id,heuristic_id,cost,order,pharmacies\n";

    srand((unsigned)time(NULL));

    for (int i = 0; i < 25; i++) {
        point start = { rand() % 31, rand() % 31 };
        point target = { rand() % 31, rand() % 31 };

        std::vector<int> order = { 0, 1, 2, 3 };

        std::set<point> constraints = { };
        while (constraints.size() < 4) {
            constraints.insert({ rand() % 31, rand() % 31 });
        }

        for (int j = 0; j < 4; j++) {
            auto cost = costs[j];
            for (int k = 0; k < 2;k++) {
                fill_blocks(blocks);
                file4 << a_star(start, target, cost, heuristic_fns[k], j, k, constraints, blocks, std::ref(shouldDraw)) << "\n";
            }
        }
    }
}

void experiment5(
    std::vector<point> start_points,
    std::vector<point> target_points,
    std::vector<std::vector<int>> orders,
    std::vector<std::set<point>> constraints,
    std::function<void(std::string)> toastText
) {
    if (start_points.size() != 25 || target_points.size() != 25 || orders.size() != 25 || constraints.size() != 25) {
        toastText("start_points, target_points, orders e constraints devem ter 25 elementos\n");
        return;
    }
    block blocks[space_size][space_size];
    bool shouldDraw = false;
    fill_blocks(blocks);
    std::ofstream file4("log5.csv");
    if (!file4.is_open()) {
        toastText("Erro ao abrir o arquivo log.csv\n");
        return;
    }

    file4 << "algorithm,visited_qty,generated_qty,path_size,path,start,target,cost_id,heuristic_id,cost,order,pharmacies\n";

    for (int i = 0; i < 25; i++) {
        point start = start_points[i];
        point target = target_points[i];

        std::vector<int> order = orders[i];
        std::set<point> constraint = constraints[i];

        for (int j = 0; j < 4; j++) {
            auto cost = costs[j];
            for (int k = 0; k < 2;k++) {
                fill_blocks(blocks);
                file4 << a_star(start, target, cost, heuristic_fns[k], j, k, constraint, blocks, std::ref(shouldDraw)) << "\n";
            }
        }
    }
}
