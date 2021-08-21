#pragma once

#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <future>
#include <type_traits>

namespace cpp_network {


    class thread_pool
    {
    public:
    
        template<typename Fn, typename ... Args>
        using job_rv = typename std::invoke_result_t<Fn, Args...>;

        thread_pool();


        template<typename Fn, typename ... Args>
        std::future<job_rv<Fn, Args...>> add_work(Fn&& fn, Args&&... args);

        void start(std::size_t num_threads = std::thread::hardware_concurrency());

        void stop();

    private:

        void do_add_work(std::function<void()>&& work);

        std::mutex m_guard;
        std::condition_variable m_cv_sleep_ctrl;
        std::vector<std::thread> m_workers;
        std::deque<std::function<void()>> m_work;

        bool m_running;
    };

    template<typename Fn, typename ... Args>
    std::future<thread_pool::job_rv<Fn, Args...>> thread_pool::add_work(Fn&& fn, Args&&... args)
    {
        using task_t = typename std::packaged_task<job_rv<Fn, Args...>()>;

        // does this HAVE to be a shared_ptr?...
        // std_bind returns a value type, can't we copy that into the lambda
        // in do_add_work below directly?

        auto work = std::make_shared<task_t>(std::bind(std::forward<Fn>(fn), std::forward<Args>(args) ...));
        
        /* Conversion from packaged_task to std::function<void>(), avoids 
           heap allocation due to small object optimization (hopefully)
        */
        do_add_work([work] { (*work)(); });

        return work->get_future();
    }

    // make overload for void return type?
}
