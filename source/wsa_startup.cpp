
#include <WinSock2.h>

namespace {
    class wsa_startup {
    public:
        wsa_startup() {
            WSADATA wsaData;
            WORD wVersionRequested = MAKEWORD(2, 2);

            /* ignore error code, it will throw an exception later if it failed*/ 
            WSAStartup(wVersionRequested, &wsaData);
        }

    } wsa2_startup;
}