#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <cassert>
#include "config.hpp"

const char al[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

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

class GomokuBoard {
public:
  enum SPOT_STATE {
    EMPTY = 0,
    BLACK = 1,
    WHITE = 2
  };
  std::array<std::array<int, SIZE>, SIZE> board;
  int empty_count;
  int cur_player;
  bool done;
  int winner;
private:
  int get_next_player(int player) const {
    return 3 - player;
  }
  bool is_spot_on_board(Point p) const {
    return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
  }
  int get_disc(Point p) const {
    return board[p.x][p.y];
  }
  void set_disc(Point p, int disc) {
    board[p.x][p.y] = disc;
  }
  bool is_disc_at(Point p, int disc) const {
    if (!is_spot_on_board(p))
      return false;
    if (get_disc(p) != disc)
      return false;
    return true;
  }
  bool is_spot_valid(Point center) const {
    if (get_disc(center) != EMPTY)
      return false;
    return true;
  }
  
public:
  GomokuBoard() {
    reset();
  }
  void reset() {
    for (int i = 0; i < SIZE; i++) {
      for (int j = 0; j < SIZE; j++) {
        board[i][j] = EMPTY;
      }
    }
    cur_player = BLACK;
    empty_count = SIZE*SIZE;
    done = false;
    winner = -1;
  }
  bool put_disc(Point p) {
    if(!is_spot_valid(p)) {
      winner = get_next_player(cur_player);
      done = true;
      return false;
    }
    set_disc(p, cur_player);
    empty_count--;
    // Check Win
    if (checkwin(cur_player)) {
      done = true;
      winner = cur_player;
    }
    if (empty_count == 0) {
      done = true;
      winner = EMPTY;
    }

    // Give control to the other player.
    cur_player = get_next_player(cur_player);
    return true;
  }
  bool checkwin(int disc){
    for (int i = 0; i < SIZE; i++) {
      for (int j = 0; j < SIZE; j++) {
        if (is_disc_at(Point(i, j), disc)){
          bool iswin = true;
          if (i + 4 < SIZE) {
            for(int k = 0; k < 5; k++)
              if (!is_disc_at(Point(i+k, j), disc)) {
                iswin = false;
                break;
              }
            if (iswin) return true;
          }
          iswin = true;
          if (j + 4 < SIZE) {
            for(int k = 0; k < 5; k++)
              if (!is_disc_at(Point(i, j+k), disc)) {
                iswin = false;
                break;
              }
            if (iswin) return true;
          }
          iswin = true;
          if (i + 4 < SIZE && j + 4 < SIZE) {
            for(int k = 0; k < 5; k++)
              if (!is_disc_at(Point(i+k, j+k), disc)) {
                iswin = false;
                break;
              }
            if (iswin) return true;
          }
          iswin = true;
          if (i - 4 >= 0 && j + 4 < SIZE) {
            for(int k = 0; k < 5; k++)
              if (!is_disc_at(Point(i-k, j+k), disc)) {
                iswin = false;
                break;
              }
            if (iswin) return true;
          }
        }
      }
    }
    return false;
  }
  std::string encode_player(int state) {
    if (state == BLACK) return "O";
    if (state == WHITE) return "X";
    return "Draw";
  }
  std::string encode_spot(int x, int y) {
    if (is_spot_valid(Point(x, y))) return " ";
    if (board[x][y] == BLACK) return "O";
    if (board[x][y] == WHITE) return "X";
    return " ";
  }

  std::string encode_header(){
    std::stringstream header;
    header << "┌───╥";
    for(int i=0; i<SIZE; i++){
      header << "───";
      if(i!=SIZE-1)
        header << "┬";
    }
    header << "╥───┐\n";
    header << "│   ║";
    for (int i=0; i<SIZE; i++){
      if(i<9) header << ' ';
      header << i+1 << ' ';
      if(i != SIZE-1) header << "│";
    }
    header << "║   │\n";
    return header.str();
  }

  std::string encode_footer(){
    std::stringstream footer;
    footer << "│   ║";
    for (int i=0; i<SIZE; i++){
      if(i<9) footer << ' ';
      footer << i+1 << ' ';
      if(i != SIZE-1) footer << "│";
    }
    footer << "║   │\n";
    footer << "└───╨";
    for(int i=0; i<SIZE; i++){
      footer << "───";
      if(i!=SIZE-1)
        footer << "┴";
    }
    footer << "╨───┘\n";
    return footer.str();
  }

  std::string encode_spliter(){
    std::stringstream spliter;
    spliter << "├───╫";
    for(int i=0; i<SIZE; i++){
      spliter << "───";
      if(i!=SIZE-1)
        spliter << "┼";
    }
    spliter << "╫───┤\n";
    return spliter.str();
  }
  std::string encode_d_spliter(){
    std::stringstream spliter;
    spliter << "╞═══╬";
    for(int i=0; i<SIZE; i++){
      spliter << "═══";
      if(i!=SIZE-1)
        spliter << "╪";
    }
    spliter << "╬═══╡\n";
    return spliter.str();
  }

  std::string encode_output(bool fail=false) {
    int i, j;
    std::stringstream ss;
    std::string spliter=encode_spliter();

    ss << "Timestep #" << (SIZE*SIZE-empty_count+1) << "\n";
    if (fail) {
      ss << "Winner is " << encode_player(winner) << " (Opponent performed invalid move)\n";
    } else if (done) {
      ss << "Winner is " << encode_player(winner) << "\n";
    } else {
      ss << encode_player(cur_player) << "'s turn\n";
    }

    ss << encode_header();
    ss << encode_d_spliter();

    for (i = 0; i < SIZE; i++) {
      ss << "│ " << al[i] << " ║";
      for (j = 0; j < SIZE; j++) {
        ss << " " << encode_spot(i, j);
        if(j != SIZE-1)
          ss << " │";
      }
      ss << " ║ " << al[i] << " │";
      ss << '\n';
      if(i!=SIZE-1)
        ss << spliter;
    }
    ss << encode_d_spliter();
    ss << encode_footer();
    return ss.str();
  }
  std::string encode_state() {
    int i, j;
    std::stringstream ss;
    ss << cur_player << "\n";
    for (i = 0; i < SIZE; i++) {
      for (j = 0; j < SIZE-1; j++) {
        ss << board[i][j] << " ";
      }
      ss << board[i][j] << "\n";
    }
    return ss.str();
  }
};

const std::string file_log = "gamelog.txt";
const std::string file_state = "state";
const std::string file_action = "action";

void launch_executable(std::string filename) {

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
  //filename may has path in it. 
  //like "./build/player.exe" or ".\player.exe"
  size_t pos;
  std::string command = "start /min " + filename + " " + file_state + " " + file_action;
  if((pos = filename.rfind("/"))!=std::string::npos || (pos = filename.rfind("\\"))!=std::string::npos)
    filename = filename.substr(pos+1, std::string::npos);
  std::string kill = "timeout /t " + std::to_string(timeout) + " > NUL && taskkill /im " + filename + " > NUL 2>&1";
  system(command.c_str());
  system(kill.c_str());
#elif __linux__
  std::string command = "timeout " + std::to_string(timeout) + "s " + filename + " " + file_state + " " + file_action;
  system(command.c_str());
#elif __APPLE__
  // May require installing the command by:
  // brew install coreutils
  std::string command = "gtimeout " + std::to_string(timeout) + "s " + filename + " " + file_state + " " + file_action;
  system(command.c_str());
#endif
}

int main(int argc, char** argv) {
  assert(argc == 3);
  std::ofstream log("gamelog.txt");
  std::string player_filename[3];
  player_filename[1] = argv[1];
  player_filename[2] = argv[2];
  std::cout << "Player Black File: " << player_filename[GomokuBoard::BLACK] << std::endl;
  std::cout << "Player White File: " << player_filename[GomokuBoard::WHITE] << std::endl;
  GomokuBoard game;
  std::string data;
  data = game.encode_output();
  std::cout << data;
  log << data;
  while (!game.done) {
    // Output current state
    data = game.encode_state();
    std::ofstream fout(file_state);
    fout << data;
    fout.close();
    // Run external program
    launch_executable(player_filename[game.cur_player]);
    // Read action
    std::ifstream fin(file_action);
    Point p(-1, -1);
    int total = 0;
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    system("cls");
#else
    system("clear");
#endif
    while (true) {
      int x, y;
      if (!(fin >> x)) break;
      if (!(fin >> y)) break;
      p.x = x; p.y = y;
      //std::cout << "Read: " << x << ", " << y << std::endl;
      total ++;
    }
    fin.close();

    std::cout << "Put: " << al[p.x] << p.y+1 << "\n";
    log << "Put: " << al[p.x] << p.y+1 << "\n";
    std::cout << "Depth: " << total << std::endl;
    // Reset action file
    if (remove(file_action.c_str()) != 0)
      std::cerr << "Error removing file: " << file_action << "\n";
    // Take action
    if (!game.put_disc(p)) {
      // If action is invalid.
      data = game.encode_output(true);
      std::cout << data;
      log << data;
      break;
    }
    data = game.encode_output();
    std::cout << data;
    log << data << std::endl;
  }
  log.close();
  // Reset state file
  if (remove(file_state.c_str()) != 0)
    std::cerr << "Error removing file: " << file_state << "\n";
  return 0;
}
