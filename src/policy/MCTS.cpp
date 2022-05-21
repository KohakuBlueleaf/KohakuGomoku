#include <tuple>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <vector>
#include <queue>
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
  this->w = this->n = this->child_n = this->now_child_n = 0;
  this->expand_flag = false;
};

MCTS::Node::~Node(){
  delete this->state;
  for(Node* child: this->childs){
    delete child;
  }
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
    if(!this->expand_flag){
      /*
      This MCTS playout/expand strategy is different
      When this node is choosed, it playout "once" and return.
      And when this node has been choosed more than the Threshold, it expand.
      So a node can run playout 1~threshold times, 
      depands on how many times it be choosed.
      Due to this strategy(maybe), 
      the best C of UCB1 can be diffrent from normal.
      */
      value = playout(this->state, true);
#if EXPAND_THRESHOLD > 1
      if(n >= EXPAND_THRESHOLD)
        this->expand_flag = true;
#else
      this->expand_flag = true;
#endif

    }else{
      if(now_child_n != this->state->legal_actions.size()){
        expand();
        value = -this->childs.back()->eval();
      }else{
        value = -next_child().eval();
      }
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
  this->childs.push_back(
    new Node(
      &this->state->next_state(
        this->state->legal_actions[this->now_child_n]
      )
    )
  );

  this->now_child_n += 1;
};

MCTS::Node& MCTS::Node::next_child(){
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
int MCTS::count_nodes(){
  int all = root->childs.size();
  std::queue<std::vector<Node*>> queue;
  queue.push(root->childs);

  while(!queue.empty()){
    auto now = queue.front(); queue.pop();
    all += now.size();
    for(auto nexts: now){
      queue.push(nexts->childs);
    }
  };
  
  return all;
}