#include "tcp_connection.hpp"

#include <type_traits>

namespace cpp_network {

    tcp_connection::tcp_connection(tcp_socket& s)
        : m_socket(s.release())
    {
        m_has_request_flag.clear();
        m_request_data.reserve(1024);
        m_request_data.resize(1024);
    }

    void tcp_connection::close()
    {
        m_socket.close();
    }

    bool tcp_connection::fetch_request()
    {
        m_request_data.resize(m_request_data.capacity());

        try {
            const int result = m_socket.recv(reinterpret_cast<char*>(m_request_data.data()), m_request_data.size());
            if (result != 0) {
                m_request_data.resize(result);
                m_has_request_flag.test_and_set();
                return true;
            }
            else {
                // connection has closed.
                close();
            }
        }
        catch (const std::system_error&) {
            throw;
       }

        return false;
    }

    bool tcp_connection::is_closed() const noexcept
    {
        return m_socket.is_valid();
    }

    bool tcp_connection::has_request() const volatile noexcept
    {
        return m_has_request_flag.test();
    }

    void tcp_connection::clear_request()
    {
        m_has_request_flag.clear();
    }

    void tcp_connection::swap(tcp_connection& target)
    {
        m_socket = std::move(target.m_socket);
        m_request_data = std::move(target.m_request_data);
        m_has_request_flag.clear();
    }

    std::span<std::byte> tcp_connection::get_request_data() noexcept
    {
        return std::span<std::byte>(m_request_data);
    }
}