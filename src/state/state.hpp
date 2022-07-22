#pragma once

#include <string>
#include <vector>
#include <array>
#include <tuple>
#include <bitset>
#include <algorithm>
#include "../config.hpp"
#include "../hakumat/hakumat.hpp"


#define MINF -2147483647
#define INF 2147483647

#define op3_scaler 700
#define op3d_scaler 1300

#define VALMAX 262144
#define VALMIN -262144


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


//use bitset to build board
typedef std::bitset<SIZE> Row;
typedef std::array<Row, SIZE> Board_Min;
typedef std::array<Board_Min, 3> Board;

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
  GAME_STATE res = UNKNOWN;

public:
  Board board;
  int player;
  
  std::vector<Point> legal_actions;
  State(){};
  State(Board board, int player);
  
  std::vector<int> encode();
  std::string print_board();
  
  void get_legal_actions(void);
  State* next_state(Point move);
  GAME_STATE check_res();
  int eval();
  int eval_norm();
};


class NNUEState: public State{
private:
  GAME_STATE res = UNKNOWN;
  hakumat::Matrix<int> l1;
  
public:
  NNUEState();
  NNUEState(Board board, int player);
  ~NNUEState();
  
  NNUEState* next_state(Point move);
  int eval();
};