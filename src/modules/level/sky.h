#pragma once
#include <modules\node\cullable.h>

namespace MM2
{
    // Forward declarations
    class lvlSky;

    // External declarations
    extern class modStatic;
    extern class modShader;

    // Class definitions
    class lvlSky : public asCullable
    {
    private:
        modStatic* Model;
        modShader** Shaders;
        float HatYOffset;
        float YMultiplier;
        float RotationRate;
        float CurrentRotation;
        int TimeWeatherType;
        int ShaderCount;
        bool DrawEnabled;
        ColorARGB FogColors[16];
        short FogNearClip[16];
        short FogFarClip[16];
    public:
        int getShaderSet()
        {
            return TimeWeatherType;
        }

        std::tuple<byte, byte, byte, byte> getFogColor(int index) {
            if (index < 0 || index >= 16)
                return std::make_tuple((byte)0, (byte)0, (byte)0, (byte) 0);

            auto color = FogColors[index];
            return std::make_tuple(color.r, color.g, color.b, color.a);
        }

        inline short getFogNearClip(int index) {
            return (index < 0 || index >= 16) ? 0 : this->FogNearClip[index];
        }

        inline short getFogFarClip(int index) {
            return (index < 0 || index >= 16) ? 0 : this->FogFarClip[index];
        }

        inline float getRotation() {
            return this->CurrentRotation;
        }

        void setFogNearClip(int index, short clip) {
            if (index < 0 || index >= 16)
                return;
            this->FogNearClip[index] = clip;
        }

        void setFogFarClip(int index, short clip) {
            if (index < 0 || index >= 16)
                return;
            this->FogFarClip[index] = clip;
        }

        void setFogColor(int index, byte r, byte g, byte b, byte a) {
            if (index < 0 || index >= 16)
                return;

            auto myColor = &this->FogColors[index];
            myColor->a = a;
            myColor->r = r;
            myColor->g = g;
            myColor->b = b;
        }

        void setRotation(float rotation)
        {
            this->CurrentRotation = rotation;
        }
    public:

        /*
            lvlSky Virtuals
        */
        AGE_API virtual void Reset()                        { hook::Thunk<0x464C90>::Call<void>(this); }
        AGE_API virtual void Update()                       { hook::Thunk<0x464F90>::Call<void>(this); }
        AGE_API virtual void Draw()                         { hook::Thunk<0x464FB0>::Call<void>(this); }
        AGE_API virtual void DrawWithNoZState()             { hook::Thunk<0x4650B0>::Call<void>(this); }
        AGE_API virtual void DrawHat()                      { hook::Thunk<0x465140>::Call<void>(this); }
        

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<lvlSky, asCullable>("lvlSky")
                .addVariable("TimeWeatherType", &lvlSky::TimeWeatherType)
                .addVariable("DrawEnabled", &lvlSky::DrawEnabled)
                .addVariable("CurrentRotation", &lvlSky::CurrentRotation)
                .addVariable("RotationSpeed", &lvlSky::RotationRate)
                .addVariable("RotationRate", &lvlSky::RotationRate)
                .addVariable("YMultiplier", &lvlSky::YMultiplier)
                .addVariable("HatYOffset", &lvlSky::HatYOffset)
                .addFunction("GetFogColor", &getFogColor)
                .addFunction("GetFogNearClip", &getFogNearClip)
                .addFunction("GetFogFarClip", &getFogFarClip)
                .addFunction("SetFogColor", &setFogColor)
                .addFunction("SetFogNearClip", &setFogNearClip)
                .addFunction("SetFogFarClip", &setFogFarClip)
                .endClass();
        }
    };

    ASSERT_SIZEOF(lvlSky, 0xA8);

    // Lua initialization

}