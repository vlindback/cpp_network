#include "thread_pool.hpp"

namespace cpp_network {

    thread_pool::thread_pool()
        : m_running(false)
    {
        
    }

    void thread_pool::start(std::size_t num_threads)
    {
        m_running = true;
        m_workers.reserve(num_threads);

        for (size_t i = 0; i < num_threads; ++i)
        {
            m_workers.emplace_back([this]()
                {
                    while (m_running)
                    {
                        std::function<void()> work;
                        {
                            std::unique_lock ul(m_guard);
                            {
                                m_cv_sleep_ctrl.wait(ul, [this]()
                                    {
                                        if (!m_running)
                                        {
                                            return true;
                                        }
                                        /* Wake up worker if there's work or if we stopped running. */
                                        return !m_work.empty();
                                    });

                                if (!m_work.empty())
                                {
                                    work = std::move(m_work.front());
                                    m_work.pop_front();
                                }
                            }
                        }

                        if (work != nullptr)
                        {
                            work();
                        }
                       
                   }});
        }   
    }


    void thread_pool::stop()
    {
        m_running = false;

        m_cv_sleep_ctrl.notify_all();

        for (auto& worker : m_workers)
        {
            if (worker.joinable())
            {
                worker.join();
            }
        }
    }

    void thread_pool::do_add_work(std::function<void()>&& work)
    {
        std::unique_lock guard(m_guard);
        m_work.emplace_back(std::move(work));
        m_cv_sleep_ctrl.notify_one();
    }
}