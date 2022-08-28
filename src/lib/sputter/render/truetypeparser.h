#pragma once

namespace sputter { namespace assets {
    struct BinaryData;
}}

namespace sputter { namespace render {
    class TrueTypeParser
    {
    public:
        TrueTypeParser(const assets::BinaryData& dataToParse);
        bool IsGood();

        // TODO: things to retrieve after parsing

    private:
        bool                      m_isGood;
    };
}}