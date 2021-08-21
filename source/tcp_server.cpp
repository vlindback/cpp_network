#include "tcp_server.hpp"

namespace cpp_network {

    tcp_server::tcp_server(std::uint16_t port_number, request_handler rq)
        : m_request_handler(rq)
        , m_port_number(port_number)
        , m_running(false)
    {
    }

    tcp_server::~tcp_server()
    {
        stop();
    }

    void tcp_server::start()
    {
        m_running = true;

        m_socket = tcp_socket(AF_INET);
        m_socket.set_blocking(false);
        m_socket.bind(m_port_number);
        m_thread_pool.start();

        m_thread = std::jthread(&tcp_server::update, this);
    }

    void tcp_server::stop()
    {
        if (m_running) {

            m_stop_source.request_stop();

            if (m_thread.joinable()) {
                m_thread.join();
            }

            m_running = false;
            m_thread_pool.stop();
            m_socket.close();
        }
    }

    // IDEA
    // instead of having _one connection list_
    // we can have multiple, and have an updater thread for each of them.
    // increase paralellization during high load? test!

    void tcp_server::update()
    {
        /* this runs in a separate thread from main. */

        std::stop_token stoken = m_thread.get_stop_token();
        m_socket.listen();

        while (!stoken.stop_requested()) {

            sockaddr_in address;
            tcp_socket socket = m_socket.accept(address);
            if (socket.is_valid()) {
                m_connections.emplace_back(socket);
            }

            for (size_t idx = 0; idx < m_connections.size(); ++idx) {

                tcp_connection& c = m_connections[idx];

                if (!c.has_request()) {
                    if (c.fetch_request()) {
                        /* add work to process the request. */
                        m_thread_pool.add_work([&rq=m_request_handler, &c]() {
                            rq(c);
                            c.clear_request();
                        });
                    }
                    else if (c.is_closed()) {
                        m_indexes_to_close.push_back(idx);
                    }
                }
            }

            if (m_indexes_to_close.empty() == false) {

                for (size_t idx = 0; idx < m_indexes_to_close.size(); ++idx) {

                    //m_connections[m_indexes_to_close[idx]] = std::move(m_connections.back());
                    m_connections[m_indexes_to_close[idx]].swap(m_connections.back());
                    m_connections.pop_back();
                }
            }

        }
    }
}

#ifdef _WIN32
#include "wsa_startup.cpp"
#endif