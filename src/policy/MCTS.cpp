#include <tuple>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "MCTS.hpp"

template<class Iter>
inline size_t argmax(Iter first, Iter last){
  return std::distance(first, std::max_element(first, last));
}


#define EXPAND_THERSHOLD 500
#define C 0.7

/*
declartion of MCTS::Node
*/
MCTS::Node::Node(State *state):
state(state){
  this->w = this->n = 0;
};
MCTS::Node::~Node(){
  delete this->state;
}

float MCTS::Node::ucb(int t, float n, float w){
  return w/n + C*sqrt(log(t)/n);
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
      value = playout(this->state, true);
      if(n >= EXPAND_THERSHOLD)
        expand();
    }else{
      value = -next_child().eval();
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
  int t = 0;
  for(Node* child: this->childs){
    t += child->n;
    if(child->n == 0)
      return *child;
  }

  std::vector<float> ucb1;
  for(Node* child: this->childs){
    ucb1.push_back(
      -(child->w/child->n) + C*sqrt(log(t)/child->n)
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
  for(int i=0; i<times; ++i){
    root->eval();
  }
  
  std::vector<float> n_list;
  for(Node* child: root->childs){
    n_list.push_back((-child->w / (child->n+1)));
  }
  
  auto actions = root->state->legal_actions;
  return actions[argmax(std::begin(n_list), std::end(n_list))];
}