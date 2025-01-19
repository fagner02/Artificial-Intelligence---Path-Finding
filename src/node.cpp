#include <node.h>

// função para contar o número de nós em uma árvore
int count_tree_nodes(node* root) {
    std::vector<node*> stack = { root }; // pilha para realizar a travessia da árvore
    int count = 0;

    // enquanto houver nós na pilha, processada cada nó
    while (!stack.empty()) {
        node* current = stack.back(); // pega o nó no topo da pilha e incrementa o contador
        stack.pop_back();
        count++;

        // adiciona os filhos do nó atual na pilha
        for (node* child : current->children) {
            stack.push_back(child);
        }
    }
    return count; // retorna o número total de nós encontrados
}

void print_tree(node* root, int level) {
    if (root == nullptr) {
        return; // se o nó raiz for nulo, retorna
    }

    for (int i = 0; i < level; ++i) { // imprime a identação de acordo com o nível atual
        std::cout << "|   ";
    }

    // imprime a posição do nó atual
    std::cout << "Node: (" << root->pos.x << ", " << root->pos.y << ")\n";
    for (node* child : root->children) { // chama recursivamente a função para cada filho do nó atual
        print_tree(child, level + 1);
    }
}

// função para adicionar um novo nó à árvore
node* add_node(std::vector<node*>& tree_nodes, node* parent, point pos, std::set<point> constraints, float cost, float heuristic) {
    tree_nodes.push_back( // cria um novo nó e adiciona ele ao vetor de nós da árvore
        new node{
            visited_info{
                cost, // custo do nó
                heuristic, // heurística do nó
                parent == nullptr ? 0 : parent->data.step + 1, // calcula o número de passos
                parent // aponta para o nó pai
            },
            {}, // inicializa a lista de filhos vazia
            parent, // define o nó pai
            pos // define a posição do novo nó
        }
    );
    auto new_node = tree_nodes[tree_nodes.size() - 1]; // pega o ponteiro para o novo nó adicionado
    bool parentFoundGoal = parent == nullptr ? false : parent->data.found_goal; // verifica se o nó pai encontrou o objetivo
    new_node->data.found_goal = parentFoundGoal || satisfiesConstraint(new_node, constraints); // define se o novo nó encontrou o objetivo

    // se o nó pai não for nulo, adiciona o novo nó como filho do pai
    if (parent != nullptr) {
        parent->children.push_back(new_node);
    }
    return new_node; // retorna o ponteiro para o novo nó 
}