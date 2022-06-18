#pragma once
#include "../state/state.hpp"


template<class Iter>
inline size_t argmax(Iter first, Iter last);

class AlphaBeta{
public:
  static int eval(State *state, int depth, int alpha, int beta);
  static Point get_move(State *state, int depth);
};