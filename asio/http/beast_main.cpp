#include "websocketServer.h"
int main()
{
    net::io_context ioc;
    WebSocketServer server(ioc, 10086);
    server.StartAccept();
    return 0;
}