#include "src/socket.hpp"
int main()
{
    Socket client(8080, "127.0.0.1");
    return 0;
}