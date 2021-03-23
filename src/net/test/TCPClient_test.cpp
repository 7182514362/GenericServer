#include <iostream>

using namespace std;

#include "Log.h"
#include "TCPClient.h"
#include "InetAddress.h"

int main()
{
    LOG_SET_LOGLEVEL(generic::LogLevel::TRACE);
    LOG_SHOW_TIMESTAMP(true);

    generic::InetAddress serverAddr("127.0.0.1", 2048, false);

    generic::TCPClient client(serverAddr);

    client.connect();

    string msg;
    while (1)
    {
        getline(cin, msg);
        if (msg.size())
            client.send(msg);
    }

    return 0;
}
