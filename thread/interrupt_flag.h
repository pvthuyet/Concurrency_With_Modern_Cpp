#pragma one
#include <thread>
#include <future>
#include <condition_variable>
#include <exception>
#include <chrono>

namespace tvp
{
  class interrupt_flag
  {
  private:
    std::atomic<bool> m_flag;
    std::condition_variable* m_thread_cond;
    std::condition_variable_any* m_thread_cond_any;
    std::mutex m_set_clear_mutex;
  public:
    interrupt_flag() : m_thread_cond(nullptr), m_thread_cond_any(nullptr)
    {}
    void set()
    {
      m_flag.store(true, std::memory_order_relaxed);
      std::lock_guard<std::mutex> lk(m_set_clear_mutex);
      if (m_thread_cond)
      {
        m_thread_cond->notify_all();
      }
      else if (m_thread_cond_any)
      {
        m_thread_cond_any->notify_all();
      }
    }
    
    bool is_set() const
    {
      return m_flag.load(std::memory_order_relaxed);
    }
    
    void set_condition_variable(std::condition_variable& cv)
    {
      std::lock_guard<std::mutex> lk(m_set_clear_mutex);
      m_thread_cond = &cv;
    }
    
    void clear_condition_variable()
    {
      std::lock_guard<std::mutex> lk(m_set_clear_mutex);
      m_thread_cond = 0;
    }
  };
}
