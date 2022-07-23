#if _WIN32
    #include "platform/time_windows.cpp"
#elif __linux__
    #include "platform/time_linux.cpp"
#else
    static_assert(false, "Unsupported platform! Expected Windows or Linux.");
#endif
