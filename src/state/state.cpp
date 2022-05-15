#include <vector>
#include <array>
#include <bitset>
#include <algorithm>
#include "state.hpp"

const int next_player[3] = {0,2,1}; 
std::vector<Point> move_list;

void move_list_init(){
  for(int i=0; i<SIZE; ++i)
    for(int j=0; j<SIZE; ++j)
      move_list.push_back(Point(i,j));
}

static bool check_5(Board_Min board){
  for(int i=0; i<SIZE-4; i+=1){
    if((board[i]
        & board[i+1]
        & board[i+2]
        & board[i+3]
        & board[i+4]).to_ulong())
      return true;

    if((board[i]
        & (board[i+1]>>1)
        & (board[i+2]>>2)
        & (board[i+3]>>3)
        & (board[i+4]>>4)).to_ulong())
      return true;

    if((board[i]
        & (board[i+1]<<1)
        & (board[i+2]<<2)
        & (board[i+3]<<3)
        & (board[i+4]<<4)).to_ulong())
      return true;
  }

  for(int i=0; i<SIZE; i+=1){
    for(int j=0; j<SIZE-4; j+=1){
      if(((board[i]>>j)&=0b11111) == 0b11111)
        return true;
    }
  }
  return false;
}
static int count_3_op(Board_Min board, Board_Min avail){
  int res = 0;
  for(int i=0; i<SIZE-3; i+=1){
    res += (
      avail[i] & board[i+1] & board[i+2] & board[i+3]
    ).count();
    res += (
      board[i] & board[i+1] & board[i+2] & avail[i+3]
    ).count();

    res += (
      avail[i]
      & (board[i+1]>>1)
      & (board[i+2]>>2)
      & (board[i+3]>>3)
    ).count();
    res += (
      board[i]
      & (board[i+1]>>1)
      & (board[i+2]>>2)
      & (avail[i+3]>>3)
    ).count();

    res += (
      avail[i]
      & (board[i+1]<<1)
      & (board[i+2]<<2)
      & (board[i+3]<<3)
    ).count();
    res += (
      board[i]
      & (board[i+1]<<1)
      & (board[i+2]<<2)
      & (avail[i+3]<<3)
    ).count();
    for(int j=0; j<SIZE; j+=1){
      res += (((board[j]>>i)&=0b0111) == 0b0111);
    }
  }
  res += ((board[0]&=0b1110) == 0b1110);
  return res;
}


State::State(Board board, int player): board(board), player(player){
  this->get_legal_actions();
};

void State::get_legal_actions(void){
  std::vector<Point> actions;
  Board_Min point;
  bool initial = true;
  for(auto p: move_list){
    if(board[0][p.x][p.y]==0){
      initial = false;
      for(auto p_off: ROUND){
        int x = p.x+p_off[0];
        int y = p.y+p_off[1];
        if(x<0 || y<0 || x>=SIZE || y>=SIZE || point[x][y] || board[0][x][y]==0)
          continue;
        actions.push_back(Point(x, y));
        point[x][y] = 1;
      }
    }
  }

  if(actions.empty() && initial)
    actions.push_back(Point(SIZE/2, SIZE/2));
  
  legal_actions = actions;
};

//get next state after a move
State& State::next_state(Point move){
  Board new_board = this->board;
  new_board[this->player][move.x][move.y] = 1;
  new_board[0][move.x][move.y] = 0;

  State *next = new State();
  next->board = new_board;
  next->player = next_player[player];
  
  Board_Min point;
  std::vector<Point> actions;
  for(Point action:legal_actions)
    if(action!=move){
      actions.push_back(action);
      point[action.x][action.y] = 1;
    }
  
  for(auto p_off: ROUND){
    int x = move.x+p_off[0];
    int y = move.y+p_off[1];
    if(x<0 || y<0 || x>=SIZE || y>=SIZE || point[x][y] || board[0][x][y]==0)
      continue;
    actions.push_back(Point(x, y));
    point[x][y] = 1;
  }
  next->legal_actions = actions;
  return *next;
}

GAME_STATE State::check_res(){
  if(res!=UNKNOWN)
    return res;
  Board_Min next = board[next_player[this->player]];
  if(check_5(next)){
    res = LOSE;
    return LOSE;
  }
  
  if(this->legal_actions.empty()){
    res = DRAW;
    return DRAW;
  }
  res = NONE;
  return NONE;
}

int State::eval(){
  Board_Min self = board[this->player];
  if(check_5(self))
    return 10;

  Board_Min opnt = board[next_player[this->player]];
  Board_Min avail = board[0];

  return count_3_op(self, avail)-count_3_op(opnt, avail);
}