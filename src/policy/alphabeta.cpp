#include <ctime>
#include <cmath>
#include <tuple>
#include <utility>
#include "alphabeta.hpp"
#include "../utils/utils.hpp"


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
    alpha = std::max(score, alpha);
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


//for selfplay
std::tuple<Point, int> AlphaBeta::get_move_with_val(State *state, int depth){
  Point best_action = Point(-1, -1);
  int alpha = MINF;

  auto all_moves = state->legal_actions;
  for(Point move: all_moves){
    State* next = state->next_state(move);
    if(next->check_res() == LOSE){
      return {move, VALMAX};
    }
    int score = -eval(next, depth-1, MINF, -alpha);
    if(score > alpha){
      best_action = move;
      alpha = score;
    }
  }
  
  if(best_action.x == -1)
    best_action = all_moves[rand()%all_moves.size()];
  return {best_action, std::max(std::min(alpha, VALMAX), VALMIN)};
};