#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>

#include "NNUE/nnue.hpp"
#include "state/state.hpp"
#include "timer/timer.hpp"


#define N 10000000


int main(){
  NNUE::initialize();
  srand(hires_time());
  std::cout << std::setprecision(2) << std::fixed;
  
  Timer t;
  Board test;
  for(int i=0; i<SIZE; i++){
    for(int j=0; j<SIZE; j++){
      test[rand()%3][i][j] = 1;
    }
  }
  
  NNUEState a(test, 1);
  
  t.start();
  for(int i=0; i<N; i+=1){
    a.eval();
  }
  auto t1 = t.duration();
  
  std::cout << N/t1 << std::endl;
}