#pragma once

#include <cstdint>
#include <cstdio>

namespace sputter { namespace net {
    class ReliableUDPSession;
    class ReliableUDPServer
    {
    public:
        ReliableUDPServer();
        ~ReliableUDPServer();

        bool Bind(int port);
        bool Listen();
        bool Accept(int* pClientIdOut);

        ssize_t Write(int clientId, char* pBuffer, size_t bufferSize);
        ssize_t Read(int clientId, char* pBuffer, size_t bufferSize);

        void Close();

    private:
        ReliableUDPSession* m_pSession = nullptr;
        int                 m_serverSocket = -1;
    };
}}