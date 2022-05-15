#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <array>
#include "state/state.hpp"


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
  int x;
  // Keep updating the output until getting killed.
  while(true) {
    // Choose a random spot.
    x = (rand() % root.legal_actions.size());
    auto move = root.legal_actions[x];
    fout << move.x << " " << move.y << std::endl;
    
    // Remember to flush the output to ensure the last action is written to file.
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
