#ifndef __HAKUMAT_UTILS_H__
#define __HAKUMAT_UTILS_H__

#include <iostream>
#include <sstream>

#include <cassert>
#include <cstring>

#include "matrix.hpp"


namespace hakumat{
  template<class dtype>
  inline void print_mat(Matrix<dtype> a) {
    std::stringstream ss;
    ss << "[";
    for(int i=0; i<a.row; i++){
      if(i!=0)
        ss << " ";
      ss << "[";
      for(int j=0; j<a.col; j++){
        ss << a[i][j];
        if(j!=a.col-1)
          ss << ", ";
      }
      ss << "]";
      if(i!=a.row-1)
        ss << ",\n";
    }
    std::cout << ss.str() << "]\n";
  }
  
  
  //a dot b
  template<class dtype>
  inline Matrix<dtype> matmul(Matrix<dtype> a, Matrix<dtype> b) {
    assert(a.col == b.row);
    int n = a.row , p = a.col, m = b.col;
    dtype sum;
    
    Matrix<dtype> c(n, m);
    for (int i = 0; i < n; i+=1){
      for (int j = 0; j < m; j+=1){
        sum = 0;
        for (int k = 0; k < p; k+=1)
          sum += a[i][k] * b(k,j);
        c[i][j] = sum;
      }
    }
    return c;
  }
  
  
  //a dot b.T
  template<class dtype>
  inline Matrix<dtype> matmul_t(Matrix<dtype> a, Matrix<dtype> b) {
    assert(a.col == b.col);
    int n = a.row , p = a.col, m = b.row;
    dtype sum;
    
    Matrix<dtype> c(n, m);
    for (int i = 0; i < n; i+=1){
      for (int j = 0; j < m; j+=1){
        sum = 0;
        for (int k = 0; k < p; k+=1)
          sum += a[i][k] * b[j][k];
        c[i][j] = sum;
      }
    }
    return c;
  }
  
  //
  template<class dtype>
  inline Matrix<dtype> concat(Matrix<dtype> a, Matrix<dtype> b) {
    assert(a.col == b.col);
    int n1 = a.row , p = a.col, n2 = b.row;
    
    Matrix<dtype> c(n1+n2, p);
    c.setdata(a.raw(), 0, n1*p);
    c.setdata(b.raw(), n1*p, n2*p);
    return c;
  }
  template<class dtype>
  inline Matrix<dtype> concat(Matrix<dtype> a, Matrix<dtype> b, bool col) {
    if(col){
      assert(a.row == b.row);
      int n = a.row , p = a.col, m = b.col;
      
      Matrix<dtype> c(n, p+m);
      for(int i=0; i<n; i+=1){
        memcpy(c[i], a[i], sizeof(dtype)*p);
        memcpy(&c[i][p], b[i], sizeof(dtype)*m);
      }
      return c;
    }else
      return concat(a, b);
  }
}

#endif