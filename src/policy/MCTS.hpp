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

// 100~500 is better expand thershold for this case(15*15 gomoku 10sec)
#define EXPAND_THERSHOLD 300

// theoretically the C should be sqrt(2) but sqrt(2)/2 is the best one
// maybe the real "Uncertainty" of our algorithm is not like others.
// I actually don't know why
#define C 0.7 

template<class Iter>
inline size_t argmax(Iter first, Iter last);
inline float ucb(float w, float n, int t);

class MCTS{
public:
  class Node{
    public:
      State *state;
      float w, n, child_n;
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