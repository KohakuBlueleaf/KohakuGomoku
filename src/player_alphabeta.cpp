#include <iostream>
#include <fstream>

#include "state/state.hpp"
#include "policy/alphabeta.hpp"


State root;
void read_board(std::ifstream& fin) {
  Board board;
  int player;
  fin >> player;

  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      board[0][i][j] = board[1][i][j] = board[2][i][j] = 0;
      int temp; fin >> temp;
      board[temp][i][j] = 1;
    }
  }

  root = State(board, player);
}

void write_valid_spot(std::ofstream& fout) {
  auto moves = root.legal_actions;
  for(auto move:moves){
    if(root.next_state(move).check_res() == LOSE){
      fout << move.x << " " << move.y << std::endl;
      fout.flush();
      return;
    }
  }
  if(moves.empty())
    return;
  // Keep updating the output until getting killed.
  int depth = 1;
  while (true){
    auto move = AlphaBeta::get_move(&root, depth);
    if(move.x != -1 && move.y != -1){
      fout << move.x << " " << move.y << std::endl;
      fout.flush();
    }
    depth += 1;
  }
}

int main(int, char** argv) {
  srand(RANDOM_SEED);
  std::ifstream fin(argv[1]);
  std::ofstream fout(argv[2]);

  move_list_init();
  read_board(fin);
  write_valid_spot(fout);

  fin.close();
  fout.close();
  return 0;
}
