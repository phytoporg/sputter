#include <iostream>
#include <string>
#include <array>
#include <algorithm>
#include <stdexcept>
#include <cstdio>

#include <gtest/gtest.h>

#include <sputter/system/time.h>
#include <sputter/net/reliableudpsession.h>

using namespace sputter;

TEST(create_sessions, udpsession_tests)
{
    const uint32_t SessionId = 0x1234;
    net::ReliableUDPSession session(SessionId, "127.0.0.1", 5001, 5002);
}

TEST(session_ports, udpsession_tests)
{
    const uint32_t SessionId = 0x1234;
    const int LocalPort = 5001;
    const int RemotePort = 5002;
    net::ReliableUDPSession session(SessionId, "127.0.0.1", LocalPort, RemotePort);
    EXPECT_EQ(session.GetPort(), LocalPort);
}

TEST(session_send_receive, udpsession_tests)
{
    const uint32_t SessionId = 0x1234;
    const int ClientPort = 5001;
    const int ServerPort = 5002;
    net::ReliableUDPSession clientSession(SessionId, "127.0.0.1", ClientPort, ServerPort);
    net::ReliableUDPSession serverSession(SessionId, "127.0.0.1", ServerPort, ClientPort);

    clientSession.Tick();
    serverSession.Tick();
    system::SleepMs(16);

    const char* pDataToSend = "hey sup";
    const size_t Sent = clientSession.EnqueueSendData(pDataToSend, sizeof(pDataToSend));
    EXPECT_EQ(Sent, sizeof(pDataToSend));

    system::SleepMs(16);
    clientSession.Tick();
    serverSession.Tick();

    char pReadBuffer[256] = {};
    const size_t Received = serverSession.TryReadData(pReadBuffer, sizeof(pReadBuffer));
    EXPECT_EQ(Received, sizeof(pDataToSend));
}