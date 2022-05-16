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

// theoretically this param should be 1
// but you need to consider the memory comsumption
// and surprisngly 300 is the best option(even better than 1)
// may caused by the playout/expand strategy and the time limit
#define EXPAND_THRESHOLD 300
#define ITERATION_LIMIT 1000

// theoretically the C should be sqrt(2) but sqrt(2)/2 is the best one
// maybe the real "Uncertainty" of our algorithm is not like others.
// I also don't know why
#define C 0.7071067811865476

template<class Iter>
inline size_t argmax(Iter first, Iter last);
inline float ucb(float w, float n, int t);


class MCTS{
public:
  class Node{
    public:
      State *state;
      float w, n;
      int child_n;
      std::vector<Node*> childs;

      Node(State *state);
      ~Node();

      static int playout(State *state, bool root);
      float eval();
      void expand();
      Node& next_child();
  };

  MCTS(State* state);
  ~MCTS();

  Point get_move(int times);

private:
  Node* root;
};