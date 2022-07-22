#pragma once
#include <vector>
#include <iostream>
#include <utility>

#include "../hakumat/hakumat.hpp"

#include "linear1.hpp"
#include "linear2.hpp"
#include "linear3.hpp"
#include "linear4.hpp"


#define SCALE_A 4096
#define SCALE_W_bit 11
#define SCALE_F_bit 4


using namespace hakumat;
using namespace std;


namespace NNUE{
  inline Matrix<int> w2m, w3m, w4m, b2m, b3m, b4m;
  
  inline void initialize(){
    w2m = w2; b2m = b2;
    w3m = w3; b3m = b3;
    w4m = w4; b4m = b4;
  };
  
  inline Matrix<int> clipped_relu(Matrix<int> a, int upperbound, int lowerbound){
    int n = a.row , p = a.col;
    Matrix<int> output(n, p);
    
    for (int i = 0; i < n; i+=1){
      for (int j = 0; j < p; j+=1){
        output(i, j) = min(max(a(i, j), lowerbound), upperbound);
      }
    }
    return output;
  }
  
  inline int predict(Matrix<int> x){
    Matrix<int> temp;
    temp = clipped_relu(x, SCALE_A, 0);
    temp = matmul_t(temp, w2m);
    temp += b2m;
    temp >>= SCALE_W_bit;
    temp = clipped_relu(temp, SCALE_A, 0);
    
    temp = matmul_t(temp, w3m);
    temp += b3m;
    temp >>= SCALE_W_bit;
    temp = clipped_relu(temp, SCALE_A, 0);
    
    temp = matmul_t(temp, w4m);
    temp += b4m;
    
    temp >>= SCALE_F_bit;
    int ans = temp[0][0];
    temp.del();
    return ans;
  };
};