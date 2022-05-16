#include <tuple>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "MCTS.hpp"

template<class Iter>
inline size_t argmax(Iter first, Iter last){
  return std::distance(first, std::max_element(first, last));
}
inline float ucb(float w, float n, int t){
  return w/n + C*sqrt(log(t)/n);
}

/*
declartion of MCTS::Node
*/
MCTS::Node::Node(State *state):
state(state){
  this->w = this->n = this->child_n = 0;
};
MCTS::Node::~Node(){
  delete this->state;
}

int MCTS::Node::playout(State *state, bool root){
  GAME_STATE res = state->check_res();
  if(res==NONE){
    int index = rand()%state->legal_actions.size();
    int val = -playout(&state->next_state(state->legal_actions[index]), false);
    if(root)
      return val;
    else{
      delete state;
      return val;
    }
  }
  delete state;
  return res==LOSE ? -1 : 0;
};
float MCTS::Node::eval(){
  float value;
  GAME_STATE res = this->state->check_res();
  
  if(res == NONE){
    if(childs.empty()){
      /*
      This MCTS playout/expand strategy is different
      when this node is choosed, it playout "once" and return.
      until this node been choosed more than the Thershold, it expand.
      so a node can run playout from 1~threshold Times, 
      depands on how many times it be choosed.
      due to this strategy, 
      the best C of UCB1 and the threshold are diffrent from normal.
      */
      value = playout(this->state, true);
#if EXPAND_THRESHOLD > 1
      if(n >= EXPAND_THRESHOLD)
        expand();
#else
      expand();
#endif
    }else{
      value = -next_child().eval();
      this->child_n += 1;
    }
  }else{
    value = res==LOSE ? -1 : 0;
  }
  this->w += value;
  this->n += 1;
  return value;
}

void MCTS::Node::expand(){
  std::vector<Node*> temp;
  for(auto move: state->legal_actions)
    temp.push_back(new Node(&state->next_state(move)));
  
  this->childs = temp;
};

MCTS::Node& MCTS::Node::next_child(){
  for(Node* child: this->childs){
    if(child->n == 0)
      return *child;
  }

  std::vector<float> ucb1;
  for(Node* child: this->childs){
    ucb1.push_back(
      ucb(-child->w, child->n, this->child_n)
    );
  }

  return *childs[argmax(std::begin(ucb1), std::end(ucb1))];
};


/*
declartion of MCTS
*/
MCTS::MCTS(State* state){
  root = new MCTS::Node(state);
  root->expand();
}
MCTS::~MCTS(){
  delete root;
}
Point MCTS::get_move(int times){
  for(int i=0; i<times; ++i)
    root->eval();
  
  std::vector<float> n_list;
  for(Node* child: root->childs){
    n_list.push_back(
      -child->w/child->n
    );
  }
  
  auto actions = root->state->legal_actions;
  return actions[argmax(std::begin(n_list), std::end(n_list))];
}