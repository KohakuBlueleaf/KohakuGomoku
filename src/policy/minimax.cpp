#include <ctime>
#include <cmath>

#include "minimax.hpp"


//Evaluate state with MiniMax
int MiniMax::eval(State *state, int depth){
  GAME_STATE now_res = state->check_res();
  if(now_res == LOSE){
    delete state;
    return MINF;
  }
  if(now_res==DRAW){
    delete state;
    return 0;
  }
  if(depth==0){
    int score = state->eval();
    delete state;
    return score;
  }

  int best = MINF;
  for(auto move: state->legal_actions){
    //negative max
    int score = -eval(state->next_state(move), depth-1);
    if (score>best)
      best = score;
  }
  delete state;
  return best;
};


//Run MiniMax and get best move
Point MiniMax::get_move(State *state, int depth){
  Point best_action = Point(-1, -1);
  int best_score = MINF;

  for(Point move: state->legal_actions){
    int score = -eval(state->next_state(move), depth-1);
    if(score>best_score){
      best_action = move;
      best_score = score;
    }
  }

  return best_action;
};