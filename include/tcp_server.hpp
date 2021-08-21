#pragma once

#include "tcp_socket.hpp"
#include "tcp_connection.hpp"
#include "thread_pool.hpp"

#include <thread>
#include <functional>
#include <cstdint>

namespace cpp_network {

    class tcp_server
    {
        public:

            using request_handler = std::function<void(tcp_connection& c)>;

            tcp_server(std::uint16_t port_number, request_handler rq);
            ~tcp_server();

            void start();

            void stop();

        private:

            void update();

            request_handler             m_request_handler;
            thread_pool                 m_thread_pool;
            std::deque<tcp_connection>  m_connections;
            std::vector<size_t>         m_indexes_to_close;
            tcp_socket                  m_socket;
            std::jthread                m_thread;
            std::stop_source            m_stop_source;
            std::uint16_t               m_port_number;
            bool                        m_running;
    };

}