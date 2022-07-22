#include <iostream>
#include <sstream>
#include <fstream>
#include <tuple>

#include "state/state.hpp"
#include "policy/alphabeta.hpp"
#include "policy/MCTS.hpp"


#define DEPTH 4
#define MCTS_ITER 2500


State* init_board(){
  Board init;
  for(int i=0; i<SIZE; i+=1){
    for(int j=0; j<SIZE; j+=1){
      init[0][i][j] = 1;
      init[1][i][j] = init[2][i][j] = 0;
    }
  }
  
  return new State(init, 1);
}


int main(){
  srand(RANDOM_SEED);
  State *now = init_board();
  now->legal_actions.pop_back();
  now = now->next_state(Point(rand()%SIZE, rand()%SIZE));
  std::vector<std::tuple<std::vector<int>, int>> data;
  
  //randomly move for more different play
  /*
  for(int i=0; i<7; i+=1){
    Point move; int val;
    std::tie(move, val) = AlphaBeta::get_move_with_val(&now, DEPTH);
    //std::cout << now.print_board() << '\n';
    //std::cout << val << '\n';
    auto actions = now.legal_actions;
    data.push_back({now.encode(), val});
    now = *now.next_state(actions[rand()%actions.size()]);
  }//*/
  
  
  while(now->check_res() == NONE){
    Point move; int val;
    MCTS *mcts = new MCTS(now);
    std::tie(move, val) = mcts->get_move_with_val(MCTS_ITER);
    //std::cout << now->print_board() << '\n';
    //std::cout << val << " " << move.x << "," << move.y << '\n';
    data.push_back({now->encode(), val});
    now = now->next_state(move);
    delete mcts;
  }
  auto winner = (3-now->player == 1) ? 1 : -1;
  //std::cout << now->print_board() << '\n';
  
  
  std::stringstream ss;
  int ply = 1;
  for(auto s: data){
    std::vector<int> board; int val;
    std::tie(board, val) = s;
    
    ss << ply << ' ';
    ss << val << ' ';
    ss << winner << ' ';
    for(auto i: board)
      ss << i;
    ss << ',';
    winner *= -1;
    ply++;
  }
  std::cout << ss.str() << std::endl;
}