#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <bitset>
#include <algorithm>

#include "state.hpp"
#include "../NNUE/nnue.hpp"


std::vector<Point> move_list;


// get all the point on the board
void move_list_init(){
  for(int i=0; i<SIZE; ++i)
    for(int j=0; j<SIZE; ++j)
      move_list.push_back(Point(i,j));
}


// check if 5 discs on a row
// use a tircky method to check if n-disc on a row
// first use bitset to store the board(empty, pl1, pl2)
//
// horizontally: xxxxx11111xxxxx >> 5 == xxxxxxxxxx11111
//
// vertically:
// xx1xxx1xxxxxxxx
// xx1xxxxx1xxxxxx
// xx1xxxxx1xxxxxx
// xx1xxxxxxxxxxxx
// xx1xxxxx1xxxxxx
// & = xx1xxxxxxxxxxxx
//
// cross:
// xx1xxxxxx           xx1xxxxxx
// xxx1xxxxx          xxx1xxxxx
// xxxx1xxxx  ->     xxxx1xxxx
// xxxxx1xxx        xxxxx1xxx
// xxxxxx1xx       xxxxxx1xx
static bool check_5(Board_Min board){
  for(int i=0; i<SIZE-4; i+=1){
    if((board[i]
        & board[i+1]
        & board[i+2]
        & board[i+3]
        & board[i+4]).any())
      return true;

    if((board[i]
        & (board[i+1]>>1)
        & (board[i+2]>>2)
        & (board[i+3]>>3)
        & (board[i+4]>>4)).any())
      return true;

    if((board[i]
        & (board[i+1]<<1)
        & (board[i+2]<<2)
        & (board[i+3]<<3)
        & (board[i+4]<<4)).any())
      return true;
    
    for(int j=0; j<SIZE; j+=1){
      if(((board[j]>>i)&=0b11111) == 0b11111)
        return true;
    }
  }
  return false;
}


static bool check_4(Board_Min board, Board_Min avail){
  for(int i=0; i<SIZE-4; i+=1){
    if((avail[i]
        & board[i+1]
        & board[i+2]
        & board[i+3]
        & board[i+4]).any())
      return true;
    if((board[i]
        & board[i+1]
        & board[i+2]
        & board[i+3]
        & avail[i+4]).any())
      return true;

    if((avail[i]
        & (board[i+1]>>1)
        & (board[i+2]>>2)
        & (board[i+3]>>3)
        & (board[i+4]>>4)).any())
      return true;
    if((board[i]
        & (board[i+1]>>1)
        & (board[i+2]>>2)
        & (board[i+3]>>3)
        & (avail[i+4]>>4)).any())
      return true;

    if((avail[i]
        & (board[i+1]<<1)
        & (board[i+2]<<2)
        & (board[i+3]<<3)
        & (board[i+4]<<4)).any())
      return true;
    if((board[i]
        & (board[i+1]<<1)
        & (board[i+2]<<2)
        & (board[i+3]<<3)
        & (avail[i+4]<<4)).any())
      return true;
    for(int j=0; j<SIZE; j+=1){
      if(
        ((board[j]>>i)&=0b11110) == 0b11110
        && ((avail[j]>>i)&=0b00001) == 0b00001
      )return true;

      if(
        ((board[j]>>i)&=0b01111) == 0b01111
        && ((avail[j]>>i)&=0b10000) == 0b10000
      )return true;
    }
  }
  return false;
}


static int count_4(Board_Min board, Board_Min avail){
  int res = 0;
  for(int i=0; i<SIZE-4; i+=1){
    res += (
      avail[i] & board[i+1] & board[i+2] & board[i+3] & board[i+4]
    ).count();
    res += (
      board[i] & board[i+1] & board[i+2] & board[i+3] & avail[i+4]
    ).count();

    res += (avail[i]
        & (board[i+1]>>1)
        & (board[i+2]>>2)
        & (board[i+3]>>3)
        & (board[i+4]>>4)).count();
    res += (board[i]
        & (board[i+1]>>1)
        & (board[i+2]>>2)
        & (board[i+3]>>3)
        & (avail[i+4]>>4)).count();

    res += (avail[i]
        & (board[i+1]<<1)
        & (board[i+2]<<2)
        & (board[i+3]<<3)
        & (board[i+4]<<4)).count();
    res += (board[i]
        & (board[i+1]<<1)
        & (board[i+2]<<2)
        & (board[i+3]<<3)
        & (avail[i+4]<<4)).count();

    for(int j=0; j<SIZE; j+=1){
      res += (
        ((board[j]>>i)&=0b11110) == 0b11110
        && ((avail[j]>>i)&=0b00001) == 0b00001
      );

      res += (
        ((board[j]>>i)&=0b01111) == 0b01111
        && ((avail[j]>>i)&=0b10000) == 0b10000
      );
    }
    if(res > 1)
      return res;
  }
  return res;
}


static int count_3_op(Board_Min board, Board_Min avail){
  int res = 0;
  for(int i=0; i<SIZE-2; i+=1){
    Row avail_check;
    Row avail_check_d;
    Row target;

    // vertically
    avail_check = avail_check_d = Row(0);
    target = board[i] & board[i+1] & board[i+2];

    if(i>0 && i<SIZE-3)
      avail_check_d = avail[i-1] & avail[i+3];
    if(i>1)
      avail_check |= avail[i-1] & avail[i-2];
    if(i<SIZE-4)
      avail_check |= avail[i+3] & avail[i+4];

    res += (avail_check & target).count()*op3_scaler;
    res += (avail_check_d & target).count()*op3d_scaler;

    // cross l-up to r-bottom
    avail_check = avail_check_d = Row(0);
    target = board[i] & (board[i+1]>>1) & (board[i+2]>>2);

    if(i>0 && i<SIZE-3)
      avail_check_d = (avail[i-1]<<1) & (avail[i+3]>>3);
    if(i>1)
      avail_check |= (avail[i-1]<<1) & (avail[i-2]<<2);
    if(i<SIZE-4)
      avail_check |= (avail[i+3]>>3) & (avail[i+4]>>4);

    res += (avail_check & target).count()*op3_scaler;
    res += (avail_check_d & target).count()*op3d_scaler;

    // cross r-up to l-bottom
    avail_check = avail_check_d = Row(0);
    target = board[i] & (board[i+1]<<1) & (board[i+2]<<2);

    if(i>0 && i<SIZE-3)
      avail_check_d = (avail[i-1]>>1) & (avail[i+3]<<3);
    if(i>1)
      avail_check |= (avail[i-1]>>1) & (avail[i-2]>>2);
    if(i<SIZE-4)
      avail_check |= (avail[i+3]<<3) & (avail[i+4]<<4);

    res += (avail_check & target).count()*op3_scaler;
    res += (avail_check_d & target).count()*op3d_scaler;

    // horizontally
    for(int j=0; j<SIZE; j+=1){
      bool avail_check = false, avail_check_d = false;
      bool target = (((board[j]>>i)&=0b111) == 0b111);

      if(i>0 && i<SIZE-3)
        avail_check_d = avail[j][SIZE-i]&&avail[j][SIZE-4-i];
      if(i>1)
        avail_check |= avail[j][SIZE-i]&&avail[j][SIZE+1-i];
      if(i<SIZE-4)
        avail_check |= avail[j][SIZE-4-i]&&avail[j][SIZE-5-i];
        
      res += (avail_check & target)*op3_scaler;
      res += (avail_check_d & target)*op3d_scaler;
    }
  }
  return res;
}


/*
Declaration of State
*/
State::State(Board board, int player): board(board), player(player){
  this->get_legal_actions();
};


//Get legal actions around the existed discs
void State::get_legal_actions(void){
  std::vector<Point> actions;
  Board_Min point;
  bool initial = true;
  
  // only use the point that around the disc on the board
  for(auto p: move_list){
    if(board[0][p.x][p.y] == 0){
      initial = false;
      for(auto p_off: ROUND){
        int x = p.x+p_off[0];
        int y = p.y+p_off[1];
        if(x<0 || y<0 || x>=SIZE || y>=SIZE || point[x][y] || !board[0][x][y])
          continue;
        actions.push_back(Point(x, y));
        point[x][y] = 1;
      }
    }
  }
  
  // initial state(only use the central as legal action)
  if(actions.empty() && initial)
    actions.push_back(Point(SIZE/2, SIZE/2));
  
  legal_actions = actions;
};


//encode for NNUE training dataset
std::vector<int> State::encode(){
  std::vector<int> encoded_board;
  for(int i=0; i<SIZE; i+=1)
    for(int j=0; j<SIZE; j+=1)
      encoded_board.push_back(board[player][i][j]);
  for(int i=0; i<SIZE; i+=1)
    for(int j=0; j<SIZE; j+=1)
      encoded_board.push_back(board[3-player][i][j]);
  return encoded_board;
}


//for debug
std::string State::print_board(){
  std::stringstream ss;
  
  for(int i=0; i<SIZE; i+=1){
    for(int j=0; j<SIZE; j+=1){
      ss << (board[0][i][j] ? "．" : (board[1][i][j]? "Ｏ": "Ｘ"));
    }
    ss << '\n';
  }
  return ss.str();
}


//Get next state with a move
State* State::next_state(Point move){
  Board new_board = this->board;

  // place the disc
  new_board[this->player][move.x][move.y] = 1;
  new_board[0][move.x][move.y] = 0;

  // create next state
  State *next = new State();
  next->board = new_board;
  next->player = 3-player;
  
  // get the legal actions for next state
  Board_Min point;
  std::vector<Point> actions;
  for(Point action:legal_actions){
    if(action != move){
      actions.push_back(action);
      point[action.x][action.y] = 1;
    }
  }
  
  for(auto p_off: ROUND){
    int x = move.x+p_off[0];
    int y = move.y+p_off[1];
    if(x<0 || y<0 || x>=SIZE || y>=SIZE || point[x][y] || !board[0][x][y])
      continue;
    actions.push_back(Point(x, y));
    point[x][y] = 1;
  }
  next->legal_actions = actions;
  
  return next;
}


//Check if this game is and or not
GAME_STATE State::check_res(){

  if (this->res != UNKNOWN)
    return this->res;
    
  Board_Min next = this->board[3-this->player];
  if (check_5(next)){
    this->res = LOSE;
  }else if (this->legal_actions.empty()){
    this->res = DRAW;
  }else{
    this->res = NONE;
  }
  return this->res;
}


//The State-Value function of this state
int State::eval(){
  Board_Min self = board[this->player];
  Board_Min avail = board[0];
  
  if(check_5(self) || check_4(self, avail))
    return 31111;

  Board_Min opnt = board[3-this->player];
  if(count_4(opnt, avail) > 1)
    return -31111;

  return count_3_op(self, avail)-count_3_op(opnt, avail);
}


/*
Declaration of NNUEState,
NNUEState is the State class for NNUE algo.
*/
NNUEState::NNUEState(){
  //l1 is the output of NNUE linear layer 1
  l1 = hakumat::Matrix<int>(1, 500);
}
NNUEState::NNUEState(Board board, int player): State(board, player){
  l1 = hakumat::Matrix<int>(1, 500);
  
  //totally initialize, calc the l1 output from scratch
  for(int i=0; i<250; i+=1)
    l1[0][i] = b1[0][i];
  for(int i=0; i<250; i+=1)
    l1[0][i+250] = b1[0][i];
  
  for(int i=0; i<SIZE; i+=1)
    for(int j=0; j<SIZE; j+=1)
      if(board[player][i][j])
        for(int k=0; k<250; k+=1)
          l1[0][k] += w1[i*SIZE+j][k];
  
  for(int i=0; i<SIZE; i+=1)
    for(int j=0; j<SIZE; j+=1)
      if(board[3-player][i][j])
        for(int k=0; k<250; k+=1)
          l1[0][k+250] += w1[i*SIZE+j][k];
};
NNUEState::~NNUEState(){
  l1.del();
}


NNUEState* NNUEState::next_state(Point move){
  Board new_board = this->board;
  
  // place the disc
  new_board[this->player][move.x][move.y] = 1;
  new_board[0][move.x][move.y] = 0;
  
  // create next state
  NNUEState *next = new NNUEState();
  next->board = new_board;
  next->player = 3-player;
  
  // get the legal actions for next state
  Board_Min point;
  std::vector<Point> actions;
  for(Point action:legal_actions){
    if(action != move){
      actions.push_back(action);
      point[action.x][action.y] = 1;
    }
  }
  
  for(auto p_off: ROUND){
    int x = move.x+p_off[0];
    int y = move.y+p_off[1];
    if(x<0 || y<0 || x>=SIZE || y>=SIZE || point[x][y] || !board[0][x][y])
      continue;
    actions.push_back(Point(x, y));
    point[x][y] = 1;
  }
  next->legal_actions = actions;
  
  //update l1 output
  const int move_index = move.x*SIZE + move.y;
  next->l1.setdata(&this->l1[0][250], 0, 250);
  for(int i=0; i<250; i+=1)
    next->l1[0][i+250] = this->l1[0][i] + w1[move_index][i];
  
  return next;
}

//since check_5 check_4 count_4 is fast enough
//Add these functions can avoid some bad move
int NNUEState::eval(){
  Board_Min self = board[this->player];
  Board_Min avail = board[0];
  
  if(check_5(self) || check_4(self, avail))
    return VALMAX;

  Board_Min opnt = board[3-this->player];
  if(count_4(opnt, avail) > 1)
    return VALMIN;

  int val = NNUE::predict(this->l1);
  return min(max(val, VALMIN), VALMAX);
}