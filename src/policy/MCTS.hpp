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

class MCTS{
  public:
  class Node{
    public:
      State *state;
      float w, n;
      std::vector<Node*> childs;

      Node(State *state);
      ~Node();

      static float ucb(int t, float n, float w);
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