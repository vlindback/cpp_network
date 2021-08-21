#pragma once

#include <WinSock2.h>
#include <ws2tcpip.h>

#include <vector>
#include <memory>

namespace cpp_network {


    template<typename Address>
    constexpr int family_from_address_t;

    template<>
    constexpr int family_from_address_t<sockaddr_in> = AF_INET;

    template<>
    constexpr int family_from_address_t<sockaddr_in6> = AF_INET6;


    class tcp_socket
    {
        public:
            constexpr tcp_socket() noexcept = default;
            explicit tcp_socket(int family);
            tcp_socket(SOCKET socket) noexcept;

            ~tcp_socket();

            tcp_socket(tcp_socket&&) noexcept;
            tcp_socket& operator=(tcp_socket&&) noexcept;

            tcp_socket(const tcp_socket&) = delete;
            tcp_socket& operator=(const tcp_socket&) = delete;

            bool is_valid() const noexcept;

            void set_blocking(bool do_block);

            void bind(unsigned short port);
            void listen(int backlog = SOMAXCONN);
            void close();

            int recv(char* buffer, int len);

            template<typename Address>
            int recvfrom(char* buffer, int len, Address& client_address);

            template<typename Address>
            tcp_socket accept(Address& client_address);

            SOCKET release();

        private:
            
            tcp_socket do_accept(sockaddr* client_address, int* len);

            int do_recvfrom(char* buffer, int buffer_len, sockaddr* from_address, int* address_len);

            [[noreturn]] void system_error(const char* func, int error = WSAGetLastError());

            SOCKET m_socket = INVALID_SOCKET;

    };

    template<typename Address>
    int tcp_socket::recvfrom(char* buffer, int len, Address& client_address)
    {
        int len = sizeof(Address);
        return do_recvfrom(buffer, len, reinterpret_cast<sockaddr*>(&client_address), &len);
    }

    template<typename Address>
    tcp_socket tcp_socket::accept(Address& client_address)
    {
        int len = sizeof(Address);
        return do_accept(reinterpret_cast<sockaddr*>(&client_address), &len);
    }
}