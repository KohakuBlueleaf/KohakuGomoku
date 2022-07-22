#ifndef __HAKUMAT_MATRIX_H__
#define __HAKUMAT_MATRIX_H__

#include <iostream>
#include <vector>
#include <tuple>
#include <array>

#include <cstdlib>
#include <cstring>
#include <cassert>


namespace hakumat{
  //basic Matrix class
  template<class dtype>
  class Matrix{
    private:
      dtype* _data;
      
    public:
      int row, col;
      
      explicit Matrix(){
        _data = NULL;
        row = 0; col = 0;
      };
      explicit Matrix(int row, int col): row(row), col(col){
        _data = (dtype*)(calloc(row * col, sizeof(dtype)));
      };
      explicit Matrix(int row, int col, bool random): row(row), col(col){
        _data = (dtype*)(malloc(row * col * sizeof(dtype)));
        for(int i=0; i<row*col; i+=1)
          _data[i] = random ? rand() : 0;
      };
      explicit Matrix(int row, int col, int range): row(row), col(col){
        _data = (dtype*)(malloc(row * col * sizeof(dtype)));
        for(int i=0; i<row*col; i+=1)
          _data[i] = rand()%range;
      };
      inline void del(){
        free(_data);
      };
      
      inline std::tuple<unsigned, unsigned> shape() {
        return {row, col};
      }
      
      inline dtype* raw(){
        return _data;
      }
      inline void setdata(const dtype* data, unsigned index, unsigned size){
        memcpy(&_data[index], data, sizeof(dtype)*size);
      }
      
      inline void operator=(Matrix<dtype> rhs){
        if(row+col)del();
        row = rhs.row; col = rhs.col; _data = rhs._data;
      }
      inline void operator<=(Matrix<dtype> rhs){
        if(row+col)del();
        row = rhs.row; col = rhs.col; 
        _data = (dtype*)(calloc(row * col, sizeof(dtype)));
        memcpy(_data, rhs._data, row * col * sizeof(dtype));
      }
      inline void operator=(dtype *data){
        assert(sizeof(data) == row*col*sizeof(dtype));
        free(_data); _data = data;
      }
      template<int r, int c>
      inline void operator=(const dtype (&data)[r][c]){
        row = r; col = c; free(_data);
        _data = (dtype*)(calloc(row * col, sizeof(dtype)));
        for(int i=0; i<row; i+=1){
          setdata(&data[i][0], i*col, col);
        }
      }
      inline dtype* operator[](unsigned r){
        return &_data[r*col];
      }
      inline dtype& operator()(unsigned r, unsigned c) {
        return _data[r*col + c];
      }
      inline dtype operator()(unsigned r, unsigned c) const {
        return _data[r*col + c];
      }
      
      
      template<class T>
      inline Matrix operator+(T rhs){
        Matrix output(row, col, false);
        for(int i=0; i<row*col; i+=1)
          output._data[i] = _data[i] + rhs;
        return output;
      }
      template<class T>
      inline void operator+=(T rhs){
        for(int i=0; i<row*col; i+=1)
          _data[i] += rhs;
      }
      template<class T>
      inline Matrix operator+(Matrix<T> rhs){
        assert(col==rhs.col && row==rhs.row);
        Matrix output(row, col, false);
        
        for(int i=0; i<row*col; i+=1)
          output._data[i] = _data[i] + rhs._data[i];
        return output;
      }
      template<class T>
      inline void operator+=(Matrix<T> rhs){
        assert(col==rhs.col && row==rhs.row);
        for(int i=0; i<row*col; i+=1)
          _data[i] += rhs._data[i];
      }
      
      template<class T>
      inline Matrix operator-(T rhs){
        Matrix output(row, col, false);
        for(int i=0; i<row; i+=1)
          for(int j=0; j<col; j+=1)
            output(i,j) = _data[i*col + j] - rhs;
        return output;
      }
      template<class T>
      inline void operator-=(T rhs){
        for(int i=0; i<row; i+=1)
          for(int j=0; j<col; j+=1)
            _data[i*col + j] -= rhs;
      }
      template<class T>
      inline Matrix operator-(Matrix<T> rhs){
        assert(col==rhs.col && row==rhs.row);
        Matrix output(row, col, false);
        
        for(int i=0; i<row*col; i+=1)
          output._data[i] = _data[i] - rhs._data[i];
        return output;
      }
      template<class T>
      inline void operator-=(Matrix<T> rhs){
        assert(col==rhs.col && row==rhs.row);
        for(int i=0; i<row*col; i+=1)
          _data[i] -= rhs._data[i];
      }
      
      template<class T>
      inline Matrix operator*(T rhs){
        Matrix output(row, col, false);
        for(int i=0; i<row; i+=1)
          for(int j=0; j<col; j+=1)
            output(i,j) = _data[i*col + j] * rhs;
        return output;
      }
      template<class T>
      inline void operator*=(T rhs){
        for(int i=0; i<row; i+=1)
          for(int j=0; j<col; j+=1)
            _data[i*col + j] *= rhs;
      }
      
      template<class T>
      inline Matrix operator/(T rhs){
        Matrix output(row, col, false);
        for(int i=0; i<row; i+=1)
          for(int j=0; j<col; j+=1)
            output(i,j) = _data[i*col + j] / rhs;
        return output;
      }
      template<class T>
      inline void operator/=(T rhs){
        for(int i=0; i<row; i+=1)
          for(int j=0; j<col; j+=1)
            _data[i*col + j] /= rhs;
      }
      
      inline Matrix operator>>(int rhs){
        Matrix output(row, col, false);
        for(int i=0; i<row; i+=1)
          for(int j=0; j<col; j+=1)
            output(i,j) = _data[i*col + j] >> rhs;
        return output;
      }
      inline void operator>>=(int rhs){
        for(int i=0; i<row; i+=1)
          for(int j=0; j<col; j+=1)
            _data[i*col + j] >>= rhs;
      }
      inline Matrix operator<<(int rhs){
        Matrix output(row, col, false);
        for(int i=0; i<row; i+=1)
          for(int j=0; j<col; j+=1)
            output(i,j) = _data[i*col + j] << rhs;
        return output;
      }
      inline void operator<<=(int rhs){
        for(int i=0; i<row; i+=1)
          for(int j=0; j<col; j+=1)
            _data[i*col + j] <<= rhs;
      }
  };
};

#endif