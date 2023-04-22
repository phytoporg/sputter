#include <array>

#include <gtest/gtest.h>

#include <sputter/system/time.h>
#include <sputter/net/reliableudpsession.h>
#include <sputter/log/log.h>

using namespace sputter;

TEST(create_sessions, udpsession_tests)
{
    const uint32_t SessionId = 0x1234;
    net::ReliableUDPSession session(SessionId, 5001, "127.0.0.1", 5002);
}

TEST(session_ports, udpsession_tests)
{
    const uint32_t SessionId = 0x1234;
    const int LocalPort = 5001;
    const int RemotePort = 5002;
    net::ReliableUDPSession session(SessionId, LocalPort, "127.0.0.1", RemotePort);
    EXPECT_EQ(session.GetPort(), LocalPort);
}

TEST(session_send_receive, udpsession_tests)
{
    log::EnableZone(log::LogZone::Net);
    log::SetLogVerbosity(log::LogVerbosity::Info);

    // TODO: this test should be using ReliableUDPServer and ReliableUDPClient
    const uint32_t SessionId = 0x1234;
    const int ClientPort = 5001;
    const int ServerPort = 5002;
    net::ReliableUDPSession clientSession(SessionId, ClientPort, "127.0.0.1", ServerPort);
    net::ReliableUDPSession serverSession(SessionId, ServerPort, "127.0.0.1", ClientPort);

    const char pDataToSend[] = "hey sup";
    const size_t Sent = clientSession.EnqueueSendData(pDataToSend, sizeof(pDataToSend));
    EXPECT_EQ(Sent, sizeof(pDataToSend));

    system::SleepMs(16);
    clientSession.Tick();
    serverSession.Tick();

    char pReadBuffer[256] = {};
    const size_t Received = serverSession.TryReadData(pReadBuffer, sizeof(pReadBuffer));
    EXPECT_EQ(Received, sizeof(pDataToSend));
    EXPECT_EQ(memcmp(pDataToSend, pReadBuffer, sizeof(pDataToSend)), 0);
}

TEST(session_echo, udpsession_tests)
{
    log::EnableZone(log::LogZone::Net);
    log::SetLogVerbosity(log::LogVerbosity::Info);

    // TODO: this test should be using ReliableUDPServer and ReliableUDPClient
    const uint32_t SessionId = 0x1234;
    const int ClientPort = 5001;
    const int ServerPort = 5002;
    net::ReliableUDPSession clientSession(SessionId, ClientPort, "127.0.0.1", ServerPort);
    net::ReliableUDPSession serverSession(SessionId, ServerPort, "127.0.0.1", ClientPort);

    const char pDataToSend[] = "hey sup";
    const size_t ClientSent = clientSession.EnqueueSendData(pDataToSend, sizeof(pDataToSend));
    EXPECT_EQ(ClientSent, sizeof(pDataToSend));

    system::SleepMs(16);
    clientSession.Tick();
    serverSession.Tick();

    char pServerReadBuffer[256] = {};
    const size_t ServerReceived = serverSession.TryReadData(pServerReadBuffer, sizeof(pServerReadBuffer));
    EXPECT_EQ(ServerReceived, sizeof(pDataToSend));
    EXPECT_EQ(memcmp(pDataToSend, pServerReadBuffer, sizeof(pDataToSend)), 0);

    const size_t ServerSent = serverSession.EnqueueSendData(pServerReadBuffer, ServerReceived);
    EXPECT_EQ(ServerSent, ServerReceived);

    system::SleepMs(16);
    serverSession.Tick();
    clientSession.Tick();

    char pClientReceiveBuffer[256] = {};
    const size_t ClientReceived = clientSession.TryReadData(pClientReceiveBuffer, sizeof(pClientReceiveBuffer));
    EXPECT_EQ(ClientReceived, ServerSent);
    EXPECT_EQ(memcmp(pClientReceiveBuffer, pDataToSend, sizeof(pDataToSend)), 0);
}
