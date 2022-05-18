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
// so 5~20 is a better choice for EXPAND_TRESHOLD
// and the EPOCH limit is also for avoiding run out of memory
// 
// A reference of the memory usage:
// Epoch limit: 100
// iters/epoch: 1000
// expand threshold: 1
// memory usage: 80~150MB
#define EXPAND_THRESHOLD 1
#define EPOCH 10000

// Theoretically the C should be sqrt(2)
// but sqrt(2)/2 is the best for this use case.
// Maybe the real "Uncertainty" of our algorithm is not like others.
#define C 0.707

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
      size_t now_child_n;
      bool expand_flag;
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
  int count_nodes();
private:
  Node* root;
};