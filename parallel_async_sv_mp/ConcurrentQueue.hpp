#ifndef CONCURRENT_QUEUE_HPP
#define CONCURRENT_QUEUE_HPP

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

template <typename T>
class ConcurrentQueue
{
 public:

  bool empty()
  {
    return m_queue.empty();
  }

  T pop() 
  {
    unique_lock<mutex> mlock(m_mutex);
    while (m_queue.empty())
    {
      cond_v.wait(mlock);
    }
    auto val = m_queue.front();
    m_queue.pop();
    return val;
  }

  void pop(T& item)
  {
    unique_lock<mutex> mlock(m_mutex);
    if((!m_queue.empty()) || (cond_v.wait_for(mlock, chrono::milliseconds(1)) == cv_status::timeout))      
    if(!m_queue.empty())
    {
      item = m_queue.front();
      m_queue.pop();
    } 
  }

  void push(const T& item)
  {
    unique_lock<mutex> mlock(m_mutex);
    m_queue.push(item);
    mlock.unlock();
    cond_v.notify_one();
  }

  ConcurrentQueue()=default;
  ConcurrentQueue(const ConcurrentQueue&) = delete;            // disable copying
  ConcurrentQueue& operator=(const ConcurrentQueue&) = delete; // disable assignment
  
 private:
  queue<T> m_queue;
  mutex m_mutex;
  condition_variable cond_v;
};

#endif