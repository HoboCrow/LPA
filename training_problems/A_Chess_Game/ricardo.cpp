#include <algorithm>
#include <array>

using namespace std;

struct Knight {
    int x, y, moves;
};

int num_pieces, num_visited;
Knight *pieces;
array<array<int, 400>, 400> map;

int check_position(Knight piece, int x, int y) {
    if (map[x][y] == -1) { // se o valor na posição for -1, nunca foi visitado
                           // logo conta como nova posição
        num_visited += 1;
    }
    if (map[x][y] < piece.moves) {
        map[x][y] = piece.moves; // guarda valor de movimento da peça
        // verificar recursivamente para cada posição possivel
        if (piece.moves) {
            piece.moves -= 1;
            check_position(piece, x + 1, y + 2);
            check_position(piece, x - 1, y + 2);
            check_position(piece, x + 1, y - 2);
            check_position(piece, x - 1, y - 2);
            check_position(piece, x + 2, y + 1);
            check_position(piece, x + 2, y - 1);
            check_position(piece, x - 2, y + 1);
            check_position(piece, x - 2, y - 1);
        }
    }
    return 0;
}

int main() {
    // inicializar counter de posições ocupadas
    num_visited = 0;

    // inicializar mapa de posições a -1
    for (int i = 0; i < 400; i++) {
        std::fill(map[i].begin(), map[i].end(), -1);
    }

    // guardar peças
    scanf("%d", &num_pieces);
    pieces = new Knight[num_pieces];
    for (int i = 0; i < num_pieces; i++) {
        scanf("%d %d %d", &pieces[i].x, &pieces[i].y, &pieces[i].moves);
        check_position(pieces[i], pieces[i].x + 200, pieces[i].y + 200);
    }

    // verificar posições de cada peça

    printf("%d\n", num_visited);

    return 0;
}
