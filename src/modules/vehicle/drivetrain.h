#pragma once
#include <modules\vehicle.h>

namespace MM2
{
    // Forward declarations
    class vehDrivetrain;

    // External declarations
    extern class vehCarSim;
    extern class vehEngine;
    extern class vehWheel;
    extern class vehTransmission;

    // Class definitions
    class vehDrivetrain : public asNode {
    private:
        vehCarSim *m_CarSimPtr;
        vehEngine *m_AttachedEngine;
        vehTransmission *m_AttachedTransmission;
        int WheelCount;
        vehWheel *Wheels[4];
        float DynamicAmount;
        float unknown60;
        float AngInertia;
        float BrakeDynamicCoef;
        float BrakeStaticCoef;
    private:
        //lua helpers
        bool addWheelLua(vehWheel* wheel) {
            return this->AddWheel(wheel) == TRUE;
        }
    public:
        AGE_API BOOL AddWheel(vehWheel *wheel)             { return hook::Thunk<0x4D9E50>::Call<BOOL>(this, wheel); }
        
        AGE_API void CopyVars(vehDrivetrain *copyFrom)     { hook::Thunk<0x4D9DE0>::Call<void>(this, copyFrom); }

        AGE_API void Attach()                              { hook::Thunk<0x4D9E20>::Call<void>(this); }
        AGE_API void Detach()                              { hook::Thunk<0x4D9E40>::Call<void>(this); }

        AGE_API void Init(vehCarSim *carSim)               { hook::Thunk<0x4D9DD0>::Call<void>(this, carSim); }
        
        
        /*
            asNode virtuals
        */

        AGE_API void Reset() override                      { hook::Thunk<0x4D9E00>::Call<void>(this); }
        AGE_API void Update() override                     { hook::Thunk<0x4D9E90>::Call<void>(this); }
        AGE_API void FileIO(datParser &parser) override
                                                           { hook::Thunk<0x4DA570>::Call<void>(this); }
        AGE_API char * GetClassName() override             { return hook::Thunk<0x4DA600>::Call<char *>(this); }

        /*
            vehDrivetrain
        */

        int GetWheelCount() const {
            return this->WheelCount;
        }

        bool IsAttached() const {
            return this->m_AttachedEngine != nullptr;
        }

        vehWheel* GetWheel(int num) const {
            if (num < 0 || num >= this->WheelCount)
                return nullptr;
            return this->Wheels[num];
        }

        float GetAngInertia() const                        { return this->AngInertia; }
        void SetAngInertia(float angInertia)               { this->AngInertia = angInertia; }

        float GetBrakeDynamicCoef() const                  { return this->BrakeDynamicCoef; }
        void SetBrakeDynamicCoef(float coef)               { this->BrakeDynamicCoef = coef; }

        float GetBrakeStaticCoef() const                   { return this->BrakeStaticCoef; }
        void SetBrakeStaticCoef(float coef)                { this->BrakeStaticCoef = coef; }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<vehDrivetrain, asNode>("vehDrivetrain")
                //properties
                .addPropertyReadOnly("Attached", &IsAttached)
                .addPropertyReadOnly("WheelCount", &GetWheelCount)

                .addProperty("AngInertia", &GetAngInertia, &SetAngInertia)
                .addProperty("BrakeDynamicCoef", &GetBrakeDynamicCoef, &SetBrakeDynamicCoef)
                .addProperty("BrakeStaticCoef", &GetBrakeStaticCoef, &SetBrakeStaticCoef)

                //functions
                .addFunction("CopyVars", &CopyVars)
                .addFunction("GetWheel", &GetWheel)
                .addFunction("AddWheel", &addWheelLua)
                .addFunction("Attach", &Attach)
                .addFunction("Detach", &Detach)
            .endClass();
        }
    };

    ASSERT_SIZEOF(vehDrivetrain, 0x4C);
}