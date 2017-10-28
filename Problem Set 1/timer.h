#ifndef GPU_TIMER_H__
#define GPU_TIMER_H__

#include <chrono>

struct GpuTimer
{
  std::chrono::high_resolution_clock::time_point start;
  std::chrono::high_resolution_clock::time_point stop;

  void Start()
  {
    start = std::chrono::high_resolution_clock::now();
  }

  void Stop()
  {
    stop = std::chrono::high_resolution_clock::now();
  }

  float Elapsed()
  {
    return static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count());
  }
};

#endif  /* GPU_TIMER_H__ */
