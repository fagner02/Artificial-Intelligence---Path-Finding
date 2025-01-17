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
            file << dijkstra(start, target, cost, blocks, std::ref(shouldDraw), j) << "\n";
            fill_blocks(blocks);
            file << bfs(start, target, cost, blocks, std::ref(shouldDraw), j) << "\n";
            fill_blocks(blocks);
            file << dfs(start, target, cost, blocks, std::ref(shouldDraw), j) << "\n";
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
            file1 << dijkstra(start, target, cost, blocks, std::ref(shouldDraw), j) << "\n";
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
            file3 << bfs(start, target, cost, blocks, std::ref(shouldDraw), j, order) << "\n";
            fill_blocks(blocks);
            file3 << dfs(start, target, cost, blocks, std::ref(shouldDraw), j, order) << "\n";
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
