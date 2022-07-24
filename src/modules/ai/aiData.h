#pragma once
#include <mm2_common.h>

namespace MM2
{
    // Forward declarations
    class aiRaceData;

    // External declarations


    // Class definitions

    class aiRaceData {
    private:
        byte _buffer[0xC4];
    public:
        aiRaceData(void)                                    DONOTCALL;
        aiRaceData(const aiRaceData &&)                     DONOTCALL;

        virtual ~aiRaceData(void) DONOTCALL;
    };

    ASSERT_SIZEOF(aiRaceData, 0xC8);
}