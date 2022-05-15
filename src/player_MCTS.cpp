#include <iostream>
#include <fstream>

#include "state/state.hpp"
#include "policy/MCTS.hpp"

#define ITER 1000


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
  srand(time(NULL));
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
  
  MCTS mcts(&root);
  // Keep updating the output until getting killed.
  while(true) {
    auto move = mcts.get_move(ITER);
    fout << move.x << " " << move.y << std::endl;
    fout.flush();
  }
}

int main(int, char** argv) {
  std::ifstream fin(argv[1]);
  std::ofstream fout(argv[2]);

  move_list_init();
  read_board(fin);
  write_valid_spot(fout);

  fin.close();
  fout.close();
  return 0;
}
