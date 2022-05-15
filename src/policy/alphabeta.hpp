#pragma once
#include <tuple>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "../state/state.hpp"

#define max(a,b) ((a>b)?a:b)
#define min(a,b) ((a<b)?a:b)

#define MINF -2147483647
#define INF 2147483647

template<class Iter>
inline size_t argmax(Iter first, Iter last);

class AlphaBeta{
public:
  static int eval(State *state, int depth, int alpha, int beta);
  static Point get_move(State *state, int depth);
};