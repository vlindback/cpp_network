#include "tcp_server.hpp"


#include "thread_pool.hpp"


#include <iostream>
#include <random>
#include <set>
#include <algorithm>
#include <numeric>

std::set<int> make_sorted_random(const size_t num_elems)
{
    std::set<int> retval;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, num_elems - 1);

    std::generate_n(std::inserter(retval, retval.end()), num_elems, [&]() { return dis(gen); });

    return retval;
}

int main()
{
    cpp_network::tcp_server server(80, [](cpp_network::tcp_connection& c) {

        std::span data = c.get_request_data();
        __debugbreak();
        c.close();
    });

    server.start();
}