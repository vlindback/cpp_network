#include "tcp_socket.hpp"

#include <system_error>

namespace cpp_network {

    tcp_socket::tcp_socket(int family)
        : m_socket(socket(family, SOCK_STREAM, IPPROTO_TCP))
    {
        if (!is_valid()) {
            system_error("socket");
        }
    }
    
    tcp_socket::tcp_socket(SOCKET socket) noexcept
        : m_socket(socket)
    {
    }

    tcp_socket::~tcp_socket()
    {
        close();
    }

    tcp_socket::tcp_socket(tcp_socket&& s) noexcept
        : m_socket(INVALID_SOCKET)
    {
        std::swap(m_socket, s.m_socket);
    }

    tcp_socket& tcp_socket::operator=(tcp_socket&& s) noexcept
    {
        /* Supports moving to itself */
        auto temp = s.m_socket;
        s.m_socket = INVALID_SOCKET;
        close();
        m_socket = temp;

        return *this;
    }

    bool tcp_socket::is_valid() const noexcept
    {
        return m_socket != INVALID_SOCKET;
    }

    void tcp_socket::set_blocking(bool do_block)
    {
        u_long on = (do_block) ? 1L : 0L;
        const int result = ioctlsocket(m_socket, FIONBIO, &on);
        if (result == SOCKET_ERROR) {
            system_error("ioctlsocket");
        }
    }

    void tcp_socket::bind(unsigned short port)
    {
        sockaddr_in servaddr = {};
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(port);

        const int result = ::bind(m_socket, reinterpret_cast<sockaddr*>(&servaddr), sizeof(servaddr));
        if (result == SOCKET_ERROR) {
            system_error("bind");
        }
    }

    void tcp_socket::listen(int backlog)
    {
        const int result = ::listen(m_socket, backlog);
        if (result == SOCKET_ERROR) {
            system_error("listen");
        }
    }

    void tcp_socket::close()
    {
        if (is_valid()) {

            const int result = closesocket(m_socket);
            if (result == SOCKET_ERROR) {
                system_error("closesocket");
            }
            m_socket = INVALID_SOCKET;
        }
    }

    int tcp_socket::recv(char* buffer, int len)
    {
        const int result = ::recv(m_socket, buffer, len, 0);
        if (result == SOCKET_ERROR) {
            system_error("recv");
        }
        return result;
    }

    int tcp_socket::do_recvfrom(char* buffer, int buffer_len, sockaddr* from_address, int* address_len)
    {
        const int result = ::recvfrom(m_socket, buffer, buffer_len, 0, from_address, address_len);
        if (result == SOCKET_ERROR) {
            system_error("recvfrom");
        }
        return result;
    }

    tcp_socket tcp_socket::do_accept(sockaddr* client_address, int* len)
    {
        SOCKET result = ::accept(m_socket, client_address, len);
        if (result == INVALID_SOCKET) {

            const int error = WSAGetLastError();
            if (error != WSAEWOULDBLOCK) {
                system_error("accept", error);
            }
        }
        return result;
    }

    [[noreturn]] void tcp_socket::system_error(const char* func, int error)
    {
        throw std::system_error(
            std::error_code(error, std::system_category()),
            func
        );
    }

    SOCKET tcp_socket::release()
    {
        SOCKET s = m_socket;
        m_socket = INVALID_SOCKET;
        return s;
    }
}