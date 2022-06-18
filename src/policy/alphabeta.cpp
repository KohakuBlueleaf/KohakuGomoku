#include <ctime>
#include <cmath>
#include "alphabeta.hpp"


template<class Iter>
inline size_t argmax(Iter first, Iter last){
  return std::distance(first, std::max_element(first, last));
};


/*
Declaration of AlphaBeta
*/
//Evaluate state with alphabeta pruning
int AlphaBeta::eval(State *state, int depth, int alpha, int beta){
  GAME_STATE now_res = state->check_res();
  //return value of this state if the game is end or meet the depth
  if(now_res == LOSE){
    delete state;
    return MINF;
  }
  if(now_res == DRAW){
    delete state;
    return 0;
  }
  if(depth == 0){
    int score = state->eval();
    delete state;
    return score;
  }
  
  //AlphaBeta Pruning
  for(auto move: state->legal_actions){
    //negative max
    int score = -eval(state->next_state(move), depth-1, -beta, -alpha);
    alpha = max(score, alpha);
    if(alpha >= beta){
      delete state;
      return alpha;
    }
  }
  
  delete state;
  return alpha;
};


//Run AlphaBeta pruning and get best move
Point AlphaBeta::get_move(State *state, int depth){
  Point best_action = Point(-1, -1);
  int alpha = MINF;

  auto all_moves = state->legal_actions;
  for(Point move: all_moves){
    int score = -eval(state->next_state(move), depth-1, MINF, -alpha);
    if(score > alpha){
      best_action = move;
      alpha = score;
    }
  }

  return best_action;
};