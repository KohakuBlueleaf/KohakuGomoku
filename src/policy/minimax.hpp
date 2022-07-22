#pragma once
#include "../state/state.hpp"


class MiniMax{
public:
  static int eval(State *state, int depth);
  static Point get_move(State *state, int depth);
};