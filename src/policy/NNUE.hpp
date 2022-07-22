#pragma once
#include <tuple>
#include "../state/state.hpp"


template<class Iter>
inline size_t argmax(Iter first, Iter last);

class AlphaBetaNNUE{
public:
  static int eval(NNUEState *state, int depth, int alpha, int beta);
  static Point get_move(NNUEState *state, int depth);
};