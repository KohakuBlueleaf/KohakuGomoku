#pragma once
#include <string>


template<class Iterable>
inline size_t argmax(Iterable x){
  return std::distance(
    std::begin(x), 
    std::max_element(
      std::begin(x), 
      std::end(x),
    ),
  );
};