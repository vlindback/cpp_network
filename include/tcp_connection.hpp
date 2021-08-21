#pragma once

#include "tcp_socket.hpp"

#include <memory>
#include <vector>
#include <mutex>
#include <optional>
#include <atomic>
#include <span>

namespace cpp_network {

    class tcp_connection 
    {
        public:

            tcp_connection(tcp_socket& s);
            
            tcp_connection(tcp_connection&&) noexcept = default;

            tcp_connection& operator=(tcp_connection&&) noexcept = default;

            void close();

            bool fetch_request();

            bool is_closed() const noexcept;

            bool has_request() const volatile noexcept;

            void clear_request();

            void swap(tcp_connection& target);

            std::span<std::byte> get_request_data() noexcept;

        private:
            tcp_socket m_socket;
            std::vector<std::byte> m_request_data;
            std::atomic_flag m_has_request_flag;
    };

}