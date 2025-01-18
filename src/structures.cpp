#include <structures.h>

/// @brief preenche os blocos com as informações iniciais
/// @param blocks parâmetro para animação
/// @param constraints pontos representando as farmácias(ou restrições)
/// @param start ponto inicial da busca
/// @param target ponto final da busca
void fill_blocks(
    block blocks[space_size][space_size],
    std::set<point> constraints,
    point start,
    point target
) {
    // itera sobre toda a grade de blocos
    for (int i = 0; i < space_size; i++) {
        for (int j = 0; j < space_size; j++) {
            // inicializa as informações do bloco
            blocks[i][j].info.cost = -1; // custo inicial
            blocks[i][j].info.heuristic = -1; // heuristica inicial
            blocks[i][j].info.from = nullptr; // nenhum bloco anterior
            blocks[i][j].info.label = ""; // sem rótulo inicial
            blocks[i][j].info.is_path = false; // não faz parte do caminho inicialmente
            // verifica se o bloco é uma restrição
            if (constraints.find(point{ i, j }) != constraints.end()) {
                blocks[i][j].info.label = "G";
            }
            // verifica se o bloco é o ponto inicial
            if (start == point{ i,j }) {
                blocks[i][j].info.label = "S";
            }
            // verifica se o bloco é o ponto final
            if (target == point{ i,j }) {
                blocks[i][j].info.label = "T";
            }
        }
    }
}

/// @brief preenche os dados de um bloco com as informações de um nó
/// @param blocks parâmetro para animação
/// @param n nó cujas informações serão utilizadas para preencher o bloco
void set_block_data(block blocks[space_size][space_size], node n) {
    // atualiza as informações do bloco de acordo com os dados do nó n
    blocks[n.pos.x][n.pos.y].info.cost = n.data.cost;
    blocks[n.pos.x][n.pos.y].info.heuristic = n.data.heuristic;
    blocks[n.pos.x][n.pos.y].info.step = n.data.step;
    blocks[n.pos.x][n.pos.y].info.from = n.data.from;
}