
#include <path_finding.h>

/// @brief Busca em profundidade
/// @param start ponto inicial da busca
/// @param target ponto final da busca
/// @param cost função de custo
/// @param cost_id id da função de custo
/// @param constraints posições das farmácias
/// @param blocks parâmetro para animação
/// @param shouldDraw parâmetro para animação
/// @param order ordem de busca 
/// @param animate parâmetro para animação
/// @return std::string log da busca
std::string dfs(
    point start,
    point target,
    cost_fn cost,
    int cost_id,
    std::set<point> constraints,
    block blocks[space_size][space_size],
    bool& shouldDraw,
    std::vector<int> order,
    bool animate
) {
    int visited_qty = 0; // contador de nós visitados

    std::vector<node*> tree_nodes = { }; // vetor de nós da árvore de busca

    add_node(tree_nodes, nullptr, start, constraints, 0, -1); // adiciona o nó inicial à árvore de busca

    std::set<node*> visited{ tree_nodes[0] }; // conjunto de nós visitados
    std::stack<node*> stack; // pilha para realizar a busca em profundidade
    stack.push(tree_nodes[0]); // adiciona o nó inicial à pilha

    if (animate) set_block_data(blocks, *tree_nodes[0]); // configura o bloco para animação

    while (!stack.empty()) { // loop até que a pilha esteja vazia
        node* current = stack.top(); // pega o nó do topo da pilha
        stack.pop(); // remove o nó do topo da pilha
        visited_qty++; // incrementa o contador de nós visitados

        // explora as quatro direções possíveis a partir do nó atual
        for (int i = 0; i < 4; i++) { 
            int dir = order[i]; // pega a direção atual 
            point next = { current->pos + dirs[dir] }; // calcula a próxima ponto

            // verifica se a próxima ponto está dentro do espaço de busca
            if (next.x < 0 || next.x >= space_size || next.y < 0 || next.y >= space_size) {
                continue;
            }
            // calcula o custo da próxima passo
            float costValue = current->data.cost + cost(dir, current->data.step + 1);

            // verifica se a próxima ponto é o objetivo
            if (isGoal(next, target, current, constraints)) {
                std::cout << "Found target\n";
                auto new_node = add_node(tree_nodes, current, next, {}, costValue, -1); // adiciona o nó do objetivo

                if (animate) {
                    set_block_data(blocks, *new_node);
                    animatePath(*new_node, blocks, shouldDraw);
                }
                // gera o log da busca e retorna para o arquivo csv
                return generate_log(start, target, visited_qty, count_tree_nodes(tree_nodes[0]), "dfs", cost_id, -1, new_node->data.cost, calculatePath(*new_node), order, constraints);
            }

            // verifica se a próxima ponto já foi visitado
            auto existent = std::find_if(visited.begin(), visited.end(),
                [&](node* n) {
                    return n->pos == next; 
                }
            );

            // verifica se a próxima ponto deve ser revisitado
            if (existent == visited.end() || shouldRevisit((*existent), next, current, constraints)) {
                if (existent != visited.end()) {
                    visited.erase(existent); // remove o nó da lista de visitados
                }
                // adiciona o novo nó
                node* new_node = add_node(tree_nodes, current, next, constraints, costValue, -1);

                stack.push(new_node); // adiciona o novo nó à pilha
                visited.insert(new_node); // adiciona o novo nó à lista de visitados

                // atualiza o bloco para animação
                if (animate) {
                    set_block_data(blocks, *new_node);
                    setBlockColors(blocks, shouldDraw, next); 
                }
            }
        }
    }

    // anima o caminho final
    if (animate) animatePath(*tree_nodes[tree_nodes.size() - 1], blocks, shouldDraw);

    // gera o log da busca e retorna para o arquivo csv
    return generate_log(start, target, visited_qty, count_tree_nodes(tree_nodes[0]), "dfs", cost_id, -1, -1, calculatePath(*tree_nodes[tree_nodes.size() - 1]), order);
}

/// @brief Busca A*
/// @param start ponto inicial da busca
/// @param target ponto final da busca
/// @param cost função de custo
/// @param constraints restrições
/// @param cost_id id da função de custo
/// @param heuristic_id id da função de heurística
/// @param constraints posições das farmácias
/// @param blocks parâmetro para animação
/// @param shouldDraw parâmetro para animação 
/// @param animate parâmetro para animação
/// @return std::string log da busca
std::string a_star(
    point start,
    point target,
    cost_fn cost,
    heuristic_fn heuristic,
    int cost_id,
    int heuristic_id,
    std::set<point> constraints,
    block blocks[space_size][space_size],
    bool& shouldDraw,
    bool animate
) {
    std::vector <node*> tree_nodes = { }; // vetor de nós da árvore de busca
    add_node(tree_nodes, nullptr, start, constraints, 0, heuristic(start, target)); // adiciona o nó inicial à árvore de busca 
    if (animate) set_block_data(blocks, *tree_nodes[0]); // configura o bloco para animação

    std::vector<node*> open; // lista de nós a serem explorados
    std::vector<node*> visited; // lista de nós já explorados 

    open.push_back(tree_nodes[0]); // adiciona o nó inicial à lista de nós abertos

    int visited_qty = 0; /// contador de nós visitados

    while (!open.empty()) { // loop até que a lista de nós a serem explorados esteja vazia
        auto min = min_element(open.begin(), open.end(), [&](node* a, node* b) {
            // encontra o nó com o menor custo total na lista de nós a serem explorados
            return a->data.cost + heuristic(a->pos, target) < b->data.cost + heuristic(b->pos, target);
            }
        );

        node* current = *min; // nó atual sendo explorado
        open.erase(min); // remove o nó atual da lista de abertos
        visited_qty++; // incrementa o contador de nós visitados
        visited.push_back(current); // adiciona o nó atual à lista de visitados

        // verifica se o nó atual é o objetivo
        if (current->pos == target && current->data.found_goal) {
            std::cout << "Found target\n";
            // print_tree(tree_nodes[0]);
            if (animate) animatePath(*current, blocks, shouldDraw); // anima o caminho até o objetivo
            std::vector<node> path = calculatePath(*current); // calcula o caminho do inicio ao objetivo    
            // gera o log da busca e retorna para o arquivo csv
            return generate_log(start, target, visited.size(), open.size(), "a_star", cost_id, heuristic_id, current->data.cost, path, { 0,1,2,3 }, constraints);
        }

        // explora as quatro direções possíveis a partir do nó atual
        for (int i = 0; i < 4; i++) {
            point next = { current->pos + dirs[i] }; // calcula a próxima ponto
            // verifica se a próxima ponto está dentro do espaço de busca
            if (next.x < 0 || next.x >= space_size || next.y < 0 || next.y >= space_size) {
                continue;
            }

            // verifica se a próxima ponto já foi visitada
            auto visited_node = find_if(visited.begin(), visited.end(), [&](node* a) {return a->pos == next;});
            if (visited_node != visited.end()) {
                // decide se deve revisitar o nó já visitado
                if (shouldRevisit(*visited_node, next, current, constraints)) {
                    visited.erase(visited_node);
                } else {
                    continue;
                }
            }
            // calcula o custo da próxima passo
            float new_cost = current->data.cost + cost(i, current->data.step + 1);

            // verifica se a próxima ponto está na lista de abertos
            auto found = find_if(open.begin(), open.end(),
                [&](node* n) {
                    return n->pos == next;
                }
            );

            node* next_node = nullptr;

            // adiciona ou atualiza o próxima nó na lista de abertos
            if (found == open.end() || (shouldRevisit(*found, next, current, constraints))) {
                next_node = add_node(tree_nodes, current, next, constraints, new_cost, heuristic(next, target));

                if (found != open.end()) {
                    open.erase(found); // remove o nó existente da lista de abertos
                }
                open.push_back(next_node); // adiciona o novo nó à lista de abertos
                
                if (animate) {
                    set_block_data(blocks, *next_node);
                    setBlockColors(blocks, shouldDraw, next);
                }

                continue;
            } else {
                next_node = *found;
            }

            // verifica se o custo do nó existente é maior que o custo do novo nó
            if (next_node->data.cost > new_cost && !(shouldRevisit(next_node, next, current, constraints))) {
                continue;
            }

            // atualiza o custo do nó existente com o novo custo e heurística
            next_node = add_node(tree_nodes, current, next, constraints, new_cost, heuristic(next, target));

            // atualiza o bloco para animação
            if (animate) {
                set_block_data(blocks, *next_node);
                setBlockColors(blocks, shouldDraw, next);
            }
        }
    }
    // print_tree(tree_nodes[0]);

    // gera o log da busca e retorna para o arquivo csv
    return generate_log(start, target, visited.size(), open.size(), "a_star", cost_id, heuristic_id, -1, {}, { 0,1,2,3 }, constraints);
}

/// @brief Busca em largura
/// @param start ponto inicial da busca
/// @param target ponto final da busca
/// @param cost função de custo
/// @param blocks parâmetro para animação
/// @param shouldDraw parâmetro para animação 
/// @param cost_id id da função de custo
/// @param constraints posições das farmácias
/// @param order ordem de busca
/// @param animate parâmetro para animação
/// @return std::string log da busca
std::string bfs(
    point start,
    point target,
    cost_fn cost,
    block blocks[space_size][space_size],
    bool& shouldDraw,
    int cost_id,
    std::set<point> constraints,
    std::vector<int> order,
    bool animate
) {
    std::vector<node*> tree_nodes = { }; // vetor de nós da árvore de busca

    add_node(tree_nodes, nullptr, start, constraints, 0, 0); // adiciona o nó inicial à árvore de busca

    int visited_qty = 0; // contador de nós visitados 

    std::vector<node*> visited{ tree_nodes[0] }; // lista de nós visitados
    std::queue<node*> queue; // fila de nós a serem explorados
    queue.push(tree_nodes[0]); // adiciona o nó inicial a fila

    if (animate) set_block_data(blocks, *tree_nodes[0]); // configura o bloco para animação

    // enquanto a fila não estiver vazia
    while (!queue.empty()) {
        node* current = queue.front(); // pega o nó atual
        queue.pop(); // remove o nó atual da fila
        visited_qty++; // incrementa o contador de nós visitados

        // explora as quatro direções possíveis a partir do nó atual
        for (int i = 0; i < 4; i++) {
            int dir = order[i]; // pega a direção baseada na ordem
            point next = current->pos + dirs[dir]; // calcula o próximo ponto

            // verifica se o próximo ponto está dentro do espaço
            if (next.x < 0 || next.x >= space_size || next.y < 0 || next.y >= space_size) {
                continue;
            }

            // calcula o custo
            float costValue = current->data.cost + cost(dir, current->data.step + 1);

            // verifica se o próximo ponto é o objetivo
            if (isGoal(next, target, current, constraints)) {
                std::cout << "Found target\n" << current->data.cost + 1 << "\n";

                node* new_node = add_node(tree_nodes, current, next, {}, costValue, -1); // adiciona o nó à árvore 
                if (animate) { 
                    set_block_data(blocks, *new_node);
                    animatePath(*new_node, blocks, shouldDraw);
                }

                // gera o log da busca e retorna para o arquivo csv
                return generate_log(start, target, visited_qty, count_tree_nodes(tree_nodes[0]), "bfs", cost_id, -1, new_node->data.cost, calculatePath(*new_node), order, constraints);
            }

            // verifica se o próximo ponto já foi visitado
            auto existent = std::find_if(visited.begin(), visited.end(), 
                [&](node* n) {
                    return n->pos == next; 
                }
            );

            if (existent == visited.end() || shouldRevisit((*existent), next, current, constraints)) {
                if (existent != visited.end()) {
                    visited.erase(existent);  // remove o nó existente se revisitar
                }
                node* new_node = add_node(tree_nodes, current, next, constraints, costValue, -1); // adiciona novo nó
                visited.push_back(new_node); // adiciona o novo nó à lista de visitados
                queue.push(new_node); // adiciona o novo nó à fila

                // atualiza o bloco para animação
                if (animate) {
                    set_block_data(blocks, *new_node);
                    setBlockColors(blocks, shouldDraw, next);
                }
            }
        }
    }

    // gera o log e retorna para o arquivo csv
    return generate_log(start, target, visited_qty, count_tree_nodes(tree_nodes[0]), "bfs", cost_id, -1, -1, {}, order, constraints);
}

/// @brief Busca de custo uniforme(Dijkstra)
/// @param start ponto inicial da busca
/// @param target ponto final da busca
/// @param cost função de custo
/// @param blocks parâmetro para animação
/// @param shouldDraw parâmetro para animação 
/// @param cost_id id da função de custo
/// @param constraints posições das farmácias
/// @param animate parâmetro para animação
/// @return std::string log da busca
std::string dijkstra(
    point start,
    point target,
    cost_fn cost,
    block blocks[space_size][space_size],
    bool& shouldDraw,
    int cost_id,
    std::set<point> constraints,
    bool animate
) {
    std::vector<node*> tree_nodes = { }; // vetor de nós da árvore

    add_node(tree_nodes, nullptr, start, constraints, 0, 0); // adiciona o nó inicial à árvore

    if (animate) set_block_data(blocks, *tree_nodes[0]); // configura o bloco para animação

    int visited_qty = 0; // contador de nós visitados

    std::vector<node*> visited{ tree_nodes[0] }; // lista de nós visitados 
    std::vector<node*> pq; // fila de prioridade(vetor)
    pq.push_back(tree_nodes[0]); // adiciona o nó inicial à fila

    // enquanto a fila não estiver vazia
    while (!pq.empty()) {
        auto min = min_element(pq.begin(), pq.end(),
            [&](node* a, node* b) {
                return a->data.cost < b->data.cost;
            }
        ); // encontra um nó com o menor custo

        // remove o nó com o menor valor da lista de nós abertos e marca como atual
        node* current = *min;
        pq.erase(min);
        current->data.visited = true; // marca como visitado
        visited_qty++; // incrementa o contador

        if (isGoal(current, target, constraints)) { // verifica se é o objetivo
            std::cout << "Found target\n";
            if (animate) animatePath(*current, blocks, shouldDraw);

            // gera o log da busca e retorna para o arquivo csv
            return generate_log(start, target, visited_qty, count_tree_nodes(tree_nodes[0]), "dijkstra", cost_id, -1, current->data.cost, calculatePath(*current));
        }

        // explora as quatro direções possíveis a partir do nó atual
        for (int i = 0; i < 4; i++) {
            point next = { current->pos + dirs[i] }; // calcula próximo ponto

            // verifica se o próximo ponto está dentro do espaço
            if (next.x < 0 || next.x >= space_size || next.y < 0 || next.y >= space_size) {
                continue;
            }

            auto existent = std::find_if(visited.begin(), visited.end(),
                [&](node* n) {
                    return n->pos == next;
                }
            ); // verifica se o próximo ponto já foi visitado

            // calcula o novo custo
            float new_cost = current->data.cost + cost(i, current->data.step + 1);

            if (existent == visited.end() || new_cost < (*existent)->data.cost || shouldRevisit(*existent, next, current, constraints)) {
                if (existent != visited.end()) {
                    visited.erase(existent); // remove nó existente se revisitar
                }
                node* next_node = add_node(tree_nodes, current, next, constraints, new_cost, -1); // adiciona novo nó
                pq.push_back(next_node); // adiciona nó à fila
                visited.push_back(next_node); // adiciona nó à lista de visitados

                // atualiza o bloco para animação
                if (animate) { 
                    set_block_data(blocks, *next_node);
                    setBlockColors(blocks, shouldDraw, next);
                }
            }
        }
    }

    // gera o log da busca e retorna para o arquivo csv
    return generate_log(start, target, visited_qty, count_tree_nodes(tree_nodes[0]), "dijkstra", cost_id, -1, -1, {});
}

/// @brief Busca gulosa utilizando uma função de custo 
/// @param target ponto final da busca
/// @param cost função de custo
/// @param blocks parâmetro para animação
/// @param shouldDraw parâmetro para animação
/// @return std::vector<node*> caminho encontrado
std::vector<node*> greedy_search_cost(
    node* target,
    cost_fn cost,
    block blocks[space_size][space_size],
    bool& shouldDraw
) {
    std::vector<node*> path = {  }; // vetor de nós no caminho
    auto curr = target; // nó atual
    while (curr != nullptr) {
        path.push_back(curr); // adiciona nó ao caminho
        curr = curr->parent; // atualiza nó atual
    }
    auto first = path.end() - 1;
    (*first)->data.cost = 0; // define o custo inicial como 0

    //calcula custos para o caminho
    for (;first != path.begin();first--) {
        auto next = first - 1;
        point dir = (*first)->pos - (*next)->pos;
        int index = -1;
        for (int j = 0; j < 4; j++) {
            if (dirs[j] == dir) {
                index = j;
                break;
            }
        }

        // calcula custo para o próximo nó
        (*next)->data.cost = (*first)->data.cost + cost(index, (*first)->data.step + 1);
    }

    return path; // retorna o caminho
}

/// @brief Busca gulosa
/// @param start ponto inicial da busca
/// @param target ponto final da busca
/// @param cost função de custo
/// @param heuristic função de heurística
/// @param cost_id id da função de custo
/// @param heuristic_id id da função de heurística
/// @param blocks parâmetro para animação
/// @param shouldDraw parâmetro para animação 
/// @param animate parâmetro para animação
/// @return std::string log da busca
std::string greedy_search(
    point start,
    point target,
    cost_fn cost,
    heuristic_fn heuristic,
    int cost_id,
    int heuristic_id,
    std::set<point> constraints,
    block blocks[space_size][space_size],
    bool& shouldDraw,
    bool animate
) {
    std::vector<node*> tree_nodes = {}; // vetor de nós da árvore

    add_node(tree_nodes, nullptr, start, constraints, 0, heuristic(start, target)); // adiciona o nó inicial à árvore
    if (animate) set_block_data(blocks, *tree_nodes[0]); // configura o bloco para animação

    // lista de nós visitados e nós abertos
    std::vector<node*> visited{ tree_nodes[0] }; 
    std::vector<node*> open;

    int visited_qty = 0; // contador de nós visitados

    open.push_back(tree_nodes[0]); // adiciona o nó inicial à lista de nós abertos

    while (!open.empty()) { // loop até que a lista de nós abertos esteja vazia
        auto min_it = min_element(open.begin(), open.end(), 
            [&](node* a, node* b) {
                return a->data.heuristic < b->data.heuristic;
            }
        ); // encontra o nó com o menor valor heurístico

        // remove o nó com o menor valor da lista de nós abertos e marca como atual
        node* current = *min_it;
        open.erase(min_it);
        visited_qty++;

        // verifica se o nó atual é o objetivo
        if (isGoal(current, target, constraints)) {
            std::cout << "Found target\n";

            // calcula o caminho percorrido e realiza a animação
            auto path = greedy_search_cost(current, cost, blocks, shouldDraw);
            if (animate) {
                animate_greedy_search_cost(path, blocks, shouldDraw);
                animatePath(*current, blocks, shouldDraw);
            }

            // gera o log da busca e retorna para o arquivo csv
            return generate_log(start, target, visited_qty, count_tree_nodes(tree_nodes[0]), "greedy_search", cost_id, heuristic_id, current->data.cost, calculatePath(*current));
        }

        // explora as quatro direções possíveis a partir do nó atual
        for (int i = 0; i < 4; i++) {
            point next = current->pos + dirs[i];

            // verifica se o próximo ponto está dentro do espaço
            if (next.x < 0 || next.x >= space_size || next.y < 0 || next.y >= space_size) {
                continue;
            }

            // verifica se o próximo ponto já foi visitado
            auto existent = std::find_if(visited.begin(), visited.end(),
                [&](node* n) {
                    return n->pos == next;
                }
            );

            // se o ponto já foi visitado, verifica se deve ser revisitado
            if (existent != visited.end()) {
                if (shouldRevisit(*existent, next, current, constraints)) {
                    visited.erase(existent);
                } else {
                    continue;
                }
            }
            
            // adiciona o novo nó à árvore de busca e à lista de visitados
            auto new_node = add_node(tree_nodes, current, next, constraints, 0, heuristic(next, target));

            open.push_back(new_node);
            visited.push_back(new_node);

            // atualiza o bloco para animação
            if (animate) {
                set_block_data(blocks, *new_node);
                setBlockColors(blocks, shouldDraw, next);
            }
        }
    }

    // gera o log da busca e retorna para o arquivo csv
    return generate_log(start, target, visited_qty, count_tree_nodes(tree_nodes[0]), "greedy_search", cost_id, heuristic_id, -1, {});
}
