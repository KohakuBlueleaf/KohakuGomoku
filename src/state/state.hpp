#pragma once

#include <vector>
#include <array>
#include <bitset>
#include <algorithm>
#include "../config.hpp"

#define MINF -2147483647
#define INF 2147483647

extern const int next_player[3]; 

enum SPOT_STATE {
  EMPTY = 0,
  BLACK = 1,
  WHITE = 2
};
enum GAME_STATE {
  UNKNOWN,
  LOSE,
  DRAW,
  NONE
};


//use bitset to build board
typedef std::array<std::bitset<SIZE>, SIZE> Board_Min;
typedef std::array<Board_Min, 3> Board;
struct Point {
  int x, y;
	Point() : Point(0, 0) {}
	Point(float x, float y) : x(x), y(y) {}
	bool operator==(const Point& rhs) const {
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const Point& rhs) const {
		return !operator==(rhs);
	}
	Point operator+(const Point& rhs) const {
		return Point(x + rhs.x, y + rhs.y);
	}
	Point operator-(const Point& rhs) const {
		return Point(x - rhs.x, y - rhs.y);
	}
};


extern std::vector<Point> move_list;
extern void move_list_init();

const int ROUND[8][2] = {
  {1,0},
  {-1,0},
  {0,1},
  {0,-1},
  {1,1},
  {-1,1},
  {1,-1},
  {-1,-1},
};

class State{
private:
  Board board;
  int player;
  GAME_STATE res = UNKNOWN;
  
  //get all legal actions by board[0](empty place)
  void get_legal_actions(void);

public:
  std::vector<Point> legal_actions;
  State(){};
  State(Board board, int player);

  //get next state after a move
  State& next_state(Point move);
  GAME_STATE check_res();
  int eval();
};

class FullState{
private:
  Board board;
  int player;
  GAME_STATE res = UNKNOWN;
  
  //get all legal actions by board[0](empty place)
  void get_legal_actions(void);

public:
  std::vector<Point> legal_actions;
  FullState(){};
  FullState(Board board, int player);

  //get next state after a move
  FullState& next_state(Point move);
  GAME_STATE check_res();
};