#ifndef __TIMER_H__
#define __TIMER_H__

#include <chrono>


class Timer{
  private:
    using clock_type = std::chrono::high_resolution_clock;
    using second_type = std::chrono::duration<double, std::chrono::seconds::period>;

    std::chrono::time_point<clock_type> m_startTime;
  public:
    Timer(): m_startTime(clock_type::now()) {}

    inline void start(){
      m_startTime = clock_type::now();
    }

    inline double reset(){
      double dt = duration();
      m_startTime = clock_type::now();
      return dt;
    }

    inline double duration() const{
      return std::chrono::duration_cast<second_type>(clock_type::now() - m_startTime).count();
    }
};


inline long long hires_time(){
  auto now = std::chrono::high_resolution_clock::now();
  auto now_ns = std::chrono::time_point_cast<std::chrono::nanoseconds>(now);

  auto value = now_ns.time_since_epoch();
  return value.count();
}

#endif