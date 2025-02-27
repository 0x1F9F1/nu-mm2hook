#pragma once
#include "mm2_common.h"
#include "mm2_level.h"

#include <modules\city.h>
#include <modules\mmcityinfo.h>

namespace MM2
{
    // Forward declarations
    class sdlPage16;

    // External declarations
    extern class gfxTexture;
    extern class Stream;
    extern class lvlLevel;

    namespace $
    {
        declhook(0x6299A8, _Type<cityTimeWeatherLighting[16]>, timeWeathers);

        namespace sdlCommon
        {
            declhook(0x45CBC0, _Func<bool>, BACKFACE);
            declhook(0x448090, _Func<void>, UpdateLighting);
        }
        namespace sdlPage16
        {
            declhook(0x45A4E0, _MemberFunc<void>, $$ctor);

            declhook(0x448330, _MemberFunc<void>, Draw);

            declhook(0x45A560, _MemberFunc<void>, ArcMap);
            declhook(0x45A760, _MemberFunc<void>, WallMap);
            declhook(0x45A900, _MemberFunc<bool>, PointInPerimeter);

            declhook(0x45A9F0, _MemberFunc<void>, GetCentroid);
            declhook(0x45D110, _MemberFunc<int>, GetPerimeterCount);
            declhook(0x45D120, _MemberFunc<int>, GetPerimeterVertexIndex);
            declhook(0x45D140, _MemberFunc<const MM2::Vector3 &>, GetCodedVertex);
            declhook(0x45D160, _MemberFunc<float>, GetFloat);
            declhook(0x45D170, _MemberFunc<MM2::gfxTexture *>, GetTexture);

            declhook(0x450880, _Func<uint>, GetShadedColor$1);
            declhook(0x450910, _Func<uint>, GetShadedColor$2);

            declhook(0x45BF90, _Func<MM2::sdlPage16 *>, LoadBinary);
        }
    }

    typename typedef void(*SDLIteratorCB)(const void *, int, int, int, const ushort *, void *);

    class sdlCommon {
    public:
        static hook::Type<int> sm_RoomCount;
        static hook::Type<Vector3> sm_CamPos;
        static hook::Type<uint> sm_LightTable;

        AGE_API static void UpdateLighting(void) {
            $::sdlCommon::UpdateLighting();
        }

        AGE_API static bool BACKFACE(const Vector3 &vec1, const Vector3 &vec2) {
            return $::sdlCommon::BACKFACE(&vec1, &vec2);
        }
    };

    class sdlPage16 {
        char unk_00;
        char PerimeterCount;
        char unk_02;
        char unk_03;
        Vector3 *CodedVertices;
        float *Floats;
        int unk_0C;
        unsigned short *PerimeterVertices;
        unsigned short *Attributes;
        gfxTexture **Textures;
        uint unk_1C;
    public:
        AGE_API sdlPage16(int p1, int p2) {
            $::sdlPage16::$$ctor(this, p1, p2);
        }

        AGE_API static sdlPage16 * LoadBinary(Stream *stream) {
            return $::sdlPage16::LoadBinary(stream);
        }

        AGE_API void Draw(int lod, uint baseColor) const {
            $::sdlPage16::Draw(this, lod, baseColor);
        }

        AGE_API void ArcMap(float *p1, const ushort *p2, int p3, int p4, int p5) const {
            $::sdlPage16::ArcMap(this, p1, p2, p3, p4, p5);
        }

        AGE_API void WallMap(float *p1, const ushort *p2, float p3, int p4, int p5) const {
            $::sdlPage16::WallMap(this, p1, p2, p3, p4, p5);
        }

        AGE_API bool PointInPerimeter(float p1, float p2) const {
            return $::sdlPage16::PointInPerimeter(this, p1, p2);
        }

        AGE_API void GetCentroid(Vector3 &p1) const {
            $::sdlPage16::GetCentroid(this, &p1);
        }

        AGE_API int GetPerimeterCount(void) const {
            return $::sdlPage16::GetPerimeterCount(this);
        }

        AGE_API int GetPerimeterVertexIndex(int p1) const {
            return $::sdlPage16::GetPerimeterVertexIndex(this, p1);
        }

        AGE_API const Vector3 & GetCodedVertex(int p1) const {
            return $::sdlPage16::GetCodedVertex(this, p1);
        }

        AGE_API float GetFloat(int p1) const {
            return $::sdlPage16::GetFloat(this, p1);
        }

        AGE_API gfxTexture * GetTexture(int p1) const {
            return $::sdlPage16::GetTexture(this, p1);
        }

        /* these are originally private, but they're pretty helpful */

        AGE_API static uint GetShadedColor(uint p1, uint p2) {
            return $::sdlPage16::GetShadedColor$1(p1, p2);
        }

        AGE_API static uint GetShadedColor(uint p1, uint p2, uint p3) {
            return $::sdlPage16::GetShadedColor$2(p1, p2, p3);
        }
    };

    template<>
    void luaAddModule<module_city>(LuaState L) {
        luaBind<cityLevel>(L);

        luaBind<mmCityInfo>(L);
        luaBind<mmCityList>(L);

        luaBind<mmRaceData>(L);
        luaBind<mmInfoBase>(L);
        luaBind<mmPlayerData>(L);
        luaBind<dgStatePack>(L);
        luaBind<mmStatePack>(L);
        luaBind<mmPositions>(L);
    }
}