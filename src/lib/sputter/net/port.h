#pragma once

#include <cstdint>
#include <string>

namespace sputter { namespace net { class UDPPort {
    public:
        UDPPort();
        ~UDPPort();
    
        bool open(uint16_t port);
        // Close is automatically called in the destructor but can be called manually here.
        void close();
    
        bool send(const void *data, int dataSize, const std::string &address, uint16_t port);
        int receive(void *data, int dataSize, std::string &address, uint16_t &port);
    
    private:
        int socketHandle;
    };
}}

