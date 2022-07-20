#pragma once
#include <mm2_common.h>

namespace MM2
{
    // Forward declarations
    class phInertialCS;

    // External declarations


    // Class definitions

    class phInertialCS {
    private:
        byte _buffer[0x1AD];
    protected:
        hook::Field<0xC, float> _mass;
        hook::Field<0x2C, float> _maxVelocity;
        hook::Field<0x54, Matrix34> _matrix;
        hook::Field<0x9C, Vector3> _force;
        hook::Field<0x84, Vector3> _velocity;
        hook::Field<0x3C, Vector3> _scaledVelocity;
        hook::Field<0xA8, Vector3> _torque;
    public:
        AGE_API phInertialCS();

        //props
        float GetMass() const;
        void SetMass(float mass);
        float GetMaxVelocity() const;
        void SetMaxVelocity(float velocity);
        Vector3 GetPosition() const;
        Vector3 GetVelocity() const;
        void SetVelocity(Vector3 velocity);
        Matrix34 GetMatrix() const;
        void SetPosition(Vector3 const& position);
        void SetMatrix(Matrix34 const& matrix);
        Vector3 GetForce() const;
        void AddForce(Vector3 force);
        void SetForce(Vector3 force);
        Vector3 GetTorque() const;
        void AddTorque(Vector3 torque);
        void SetTorque(Vector3 torque);

        //members (not all here yet!)
        AGE_API void Zero();
        AGE_API void Reset();
        AGE_API void Freeze();
        AGE_API void ZeroForces();
        AGE_API void Init(float mass, float inertiaTensorX, float inertiaTensorY, float inertiaTensorZ);
        AGE_API void InitBoxMass(float mass, float inertiaBoxX, float inertiaBoxY, float inertiaBoxZ);
        AGE_API void Rotate(Vector3* angles);
        AGE_API void ClearInertialValues();
        
        AGE_API void MoveICS();

        //lua
        static void BindLua(LuaState L);
    };

    ASSERT_SIZEOF(phInertialCS, 0x1B4);

    // Lua initialization

}