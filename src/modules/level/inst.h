#pragma once
#include <modules\phys\bound.h>
#include <modules\model\package.h>

namespace MM2
{
    // Forward declarations
    class lvlInstance;
    class lvlInstanceLuaIterator;

    // External declarations
    extern class cltLight;
    extern class dgPhysEntity;
    extern class gfxViewport;
    extern class modStatic;
    extern class modShader;
    extern class modPackage;

    // Class definitions
    class lvlInstance
    {
    private:
        byte byte4;
        byte byte5;

        ushort room;
        ushort Flags;

        short GeomSet;

        lvlInstance* Previous;
        lvlInstance *Next;
        int getBoundLua(lua_State* L, int a1)
        {
            auto bound = this->GetBound(a1);
            if (bound == nullptr) {
                LuaState(L).push(nullptr);
                return 1;
            }

            switch (bound->getType()) 
            {
            case phBound::BoundType::Box:
                LuaState(L).push(reinterpret_cast<phBoundBox*>(bound));
                break;
            case phBound::BoundType::Geometry:
                LuaState(L).push(reinterpret_cast<phBoundGeometry*>(bound));
                break;
            case phBound::BoundType::Hotdog:
                LuaState(L).push(reinterpret_cast<phBoundHotdog*>(bound));
                break;
            case phBound::BoundType::Level:
                LuaState(L).push(nullptr);
                break;
            case phBound::BoundType::Sphere:
                LuaState(L).push(reinterpret_cast<phBoundSphere*>(bound));
                break;
            case phBound::BoundType::Terrain:
                LuaState(L).push(reinterpret_cast<phBoundTerrain*>(bound));
                break;
            case phBound::BoundType::TerrainLocal:
                LuaState(L).push(reinterpret_cast<phBoundTerrainLocal*>(bound));
                break;
            default:
                LuaState(L).push(bound);
                break;
            }
            return 1;
        }
    protected:
        static AGE_API int GetGeomSet(char const * a1, char const * a2, int a3)
                                                            { return hook::StaticThunk<0x4632C0>::Call<int>(a1, a2, a3); }
        static AGE_API void CreateTempBounds()              { hook::StaticThunk<0x464680>::Call<void>(); }
        static AGE_API void DeleteTempBounds()              { hook::StaticThunk<0x4647E0>::Call<void>(); }
    public:
        struct GeomTableEntry
        {
            modStatic *LOD[4];
            modShader **pShaders;
            phBound *Bound;
            float Radius;
            char numShaders;
            char numShadersPerPaintjob;

            inline modStatic * getLOD(int lod) const {
                if (lod < 0 || lod > 3)
                    return nullptr;

                return this->LOD[lod];
            }

            inline modStatic * getHighestLOD() const {
                for (int i = 3; i >= 0; i--) {
                    if (this->LOD[i] != nullptr)
                        return this->LOD[i];
                }
                return nullptr;
            }

            inline modStatic * getLowestLOD() const {
                for (int i = 0; i < 4; i++) {
                    if (this->LOD[i] != nullptr)
                        return this->LOD[i];
                }
                return nullptr;
            }

            inline modStatic * getVeryLowLOD() const {
                return this->LOD[0];
            }

            inline modStatic * getLowLOD() const {
                return this->LOD[1];
            }

            inline modStatic * getMedLOD() const {
                return this->LOD[2];
            }

            inline modStatic * getHighLOD() const {
                return this->LOD[3];
            }

            inline float getRadius() const {
                return Radius;
            }

            inline phBound* getBound() const {
                return this->Bound;
            }

            static void BindLua(LuaState L) {
                LuaBinding(L).beginClass<GeomTableEntry>("GeomTableEntry")
                    //fields
                    .addPropertyReadOnly("VL", &getVeryLowLOD)
                    .addPropertyReadOnly("L", &getLowLOD)
                    .addPropertyReadOnly("M", &getMedLOD)
                    .addPropertyReadOnly("H", &getHighLOD)
                    .addPropertyReadOnly("Radius", &getRadius)
                    .addPropertyReadOnly("Bound", &getBound)
                    .addFunction("GetLOD", &getLOD)
                    .addFunction("GetHighestLOD", &getHighestLOD)
                    .addFunction("GetLowestLOD", &getLowestLOD)
                .endClass();
            }

        };
        ASSERT_SIZEOF(GeomTableEntry, 32);

        static char* GetGeomName(int id) {
            if (id >= GetGeomSetCount())
                return nullptr;
            return GetGeomNameTablePtr()[id];
        }

        static GeomTableEntry* GetGeomTableEntry(int id) {
            if (id >= GetGeomSetCount())
                return nullptr;
            return &GetGeomTablePtr()[id];
        }

        static GeomTableEntry* GetGeomTablePtr() {
            return reinterpret_cast<GeomTableEntry*>(0x6316D8);
        }

        static char** GetGeomNameTablePtr() {
            return reinterpret_cast<char**>(0x651760);
        }

        static int GetGeomSetCount() {
            return *reinterpret_cast<int*>(0x655764);
        }
    private:
        Matrix34 getMatrixLua() 
        {
            Matrix34 discard;
            auto matrix = this->GetMatrix(&discard);
            discard.Set(&matrix);
            return discard;
        }

        Vector3 getPositionLua() 
        {
            auto position = this->GetPosition();
            return Vector3(position);
        }

        void setPositionLua(Vector3 position)
        {
            auto matrix = getMatrixLua();
            matrix.SetRow(3, position);
            this->SetMatrix(matrix);
        }
    public:
        LEVEL_ALLOCATOR

        AGE_API lvlInstance(void) 
        {
            scoped_vtable x(this);
            hook::Thunk<0x4631F0>::Call<void>(this);
        }

        AGE_API ~lvlInstance(void) 
        {
            scoped_vtable x(this);
            hook::Thunk<0x463220>::Call<void>(this);
        }

        inline lvlInstance* getNext() const
        {
            return this->Next;
        }

        inline lvlInstance* getPrevious() const
        {
            return this->Previous;
        }

        inline short getRoomId() const
        {
            return room;
        }

        inline short getGeomSetId() const
        {
            return GeomSet;
        }

        inline short getFlags() const 
        {
            return this->Flags;
        }

        inline void setFlags(ushort flags) 
        {
            this->Flags = flags;
        }

        inline void setFlag(ushort flag) 
        {
            this->Flags |= flag;
        }

        static AGE_API bool ComputeShadowMatrix(Matrix34 *outMatrix, int room, Matrix34 const *inMatrix)
                                                            { return hook::StaticThunk<0x464430>::Call<bool>(outMatrix, room, inMatrix); }
        static AGE_API void ResetInstanceHeap()             { hook::StaticThunk<0x4631A0>::Call<void>(); }
        static AGE_API void ResetAll()                      { hook::StaticThunk<0x4631E0>::Call<void>(); }
        static AGE_API void SetShadowBillboardMtx(Matrix44 &a1)
                                                            { hook::StaticThunk<0x463290>::Call<void>(&a1); }

        static AGE_API int AddSphere(float a1)              { return hook::StaticThunk<0x463D50>::Call<int>(a1); }

        AGE_API bool LoadBoundOnLastEntry(const char *a1)   { return hook::Thunk<0x463940>::Call<bool>(this, a1); }
        AGE_API Vector4 * GetBoundSphere(Vector4 *a1)       { return hook::Thunk<0x463A40>::Call<Vector4 *>(this, a1); }

        AGE_API bool BeginGeom(const char *a1, const char *a2, int a3)
                                                            { return hook::Thunk<0x463A80>::Call<bool>(this, a1, a2, a3); }
        AGE_API int AddGeom(const char *a1, const char *a2, int a3)
                                                            { return hook::Thunk<0x463BA0>::Call<int>(this, a1, a2, a3); }
        AGE_API void EndGeom()                              { hook::Thunk<0x463BC0>::Call<void>(this); }

        AGE_API bool InitBoundTerrain(const char *a1)       { return hook::Thunk<0x463DA0>::Call<bool>(this, a1); }
        AGE_API bool InitBoundTerrainLocal(const char *a1)  { return hook::Thunk<0x463F50>::Call<bool>(this, a1); }
        AGE_API bool NeedGhostBound(const Vector3 *a1, int a2)
                                                            { return hook::Thunk<0x4641A0>::Call<bool>(this, a1, a2); }
        AGE_API bool InitGhostBound(phBound *a1, const Vector3 *a2, int a3)
                                                            { return hook::Thunk<0x464200>::Call<bool>(this, a1, a2, a3); }
        AGE_API int InitGhost(const char *a1, const Matrix34 &a2)
                                                            { return hook::Thunk<0x464330>::Call<int>(this, a1, &a2); }

        AGE_API void PreLoadShader(int a1)                  { hook::Thunk<0x464B00>::Call<void>(this, a1); }
        AGE_API void Optimize(int a1)                       { hook::Thunk<0x464B70>::Call<void>(this, a1); }

        /*
            Custom additions
        */
        bool BeginGeomWithGroup(const char* a1, const char* a2, const char* group, int a3)
        {
            string_buf<256> groupedName("%s_%s", a1, group);

            *(bool*)0x651720 = (a3 & 8) != 0; //set dontPreload
            *(const char**)0x6516DC = groupedName; //set lastName
            auto PackageHash = (HashTable*)0x651728;

            

            int existingGeomSet;
            if (PackageHash->Access(groupedName, &existingGeomSet))
            {
                this->GeomSet = existingGeomSet;
            }
            else
            {
                auto package = new modPackage();
                *(modPackage**)0x0651740 = package; //set PKG

                if (package->Open("geometry", a1))
                {
                    this->GeomSet = GetGeomSet(a1, a2, a3);
                    PackageHash->Insert(groupedName, (void*)this->GeomSet);
                    return true;
                }

                if (package != nullptr)
                {
                    delete package;
                }

                *(modPackage**)0x0651740 = nullptr; //set PKG
                PackageHash->Insert(groupedName, (void*)0);
            }
            return false;
        }

        /*
            lvlInstance virtuals
        */

        virtual AGE_API void Reset()                        { hook::Thunk<0x463280>::Call<void>(this); }
        virtual AGE_API const Vector3 & GetPosition()       PURE;
        virtual AGE_API int IsVisible(const gfxViewport *a1)
                                                            { return hook::Thunk<0x4649F0>::Call<int>(this, a1); }

        virtual AGE_API const Matrix34 & GetMatrix(Matrix34 *a1)
                                                            PURE;
        virtual AGE_API void SetMatrix(const Matrix34 & a1) PURE;
        
        virtual AGE_API void SetVariant(int a1)             { hook::Thunk<0x4643D0>::Call<void>(this, a1); }
        virtual AGE_API const float GetRadius()             { return hook::Thunk<0x4643E0>::Call<float>(this); }
        virtual AGE_API dgPhysEntity * GetEntity()          { return hook::Thunk<0x4643B0>::Call<dgPhysEntity *>(this); }
        virtual AGE_API dgPhysEntity * AttachEntity()       { return hook::Thunk<0x4643C0>::Call<dgPhysEntity *>(this); }
        virtual AGE_API const Vector3 * GetVelocity()       { return hook::Thunk<0x4643A0>::Call<const Vector3 *>(this); }
        virtual AGE_API void Detach()                       { hook::Thunk<0x43FC30>::Call<void>(this); }
        
        virtual AGE_API void Draw(int)                      PURE;
        virtual AGE_API void DrawShadow()                   { hook::Thunk<0x4643F0>::Call<void>(this); }
        virtual AGE_API void DrawShadowMap()                { hook::Thunk<0x464400>::Call<void>(this); }
        virtual AGE_API void DrawGlow()                     { hook::Thunk<0x464410>::Call<void>(this); }
        virtual AGE_API void DrawReflected(float a1)        { hook::Thunk<0x464420>::Call<void>(this, a1); }
        virtual AGE_API void DrawReflectedParts(int a1)     { hook::Thunk<0x4648B0>::Call<void>(this, a1); }
        virtual AGE_API int Init(const char *a1, const Matrix34 &a2, int a3)
                                                            {return hook::Thunk<0x463D90>::Call<int>(this, a1, &a2, a3); }

        virtual AGE_API unsigned int SizeOf(void)           PURE;

        virtual AGE_API bool IsLandmark()                   { return hook::Thunk<0x463180>::Call<bool>(this); }
        virtual AGE_API bool IsCollidable()                 { return hook::Thunk<0x43FC40>::Call<bool>(this); }
        virtual AGE_API bool IsTerrainCollidable()          { return hook::Thunk<0x43FC50>::Call<bool>(this); }
        virtual AGE_API int GetNumLightSources()            { return hook::Thunk<0x4632B0>::Call<int>(this); }
        virtual AGE_API void GetLightInfo(int a1, cltLight *a2)
                                                            { hook::Thunk<0x4630B0>::Call<void>(this); }
        virtual AGE_API int SetupGfxLights(const Matrix34 &a1)
                                                            { return hook::Thunk<0x464670>::Call<int>(this, &a1); }
        virtual AGE_API phBound * GetBound(int a1)          { return hook::Thunk<0x4648C0>::Call<phBound *>(this, a1); };

        static void BindLuaLate(LuaState L) {
            LuaBinding(L).beginClass<lvlInstance>("lvlInstance")
                .addFunction("GetBound", &getBoundLua)
            .endClass();
        }

        static void BindLua(LuaState L) {
            LuaBinding(L).beginClass<lvlInstance>("lvlInstance")
                //fields
                .addProperty("Flags", &getFlags, &setFlags)
                .addPropertyReadOnly("Previous", &getPrevious)
                .addPropertyReadOnly("Next", &getNext)
                .addPropertyReadOnly("CurrentRoom", &getRoomId)
                .addPropertyReadOnly("GeometrySetIndex", &getGeomSetId)

                //statics
                .addStaticFunction("GetGeomName", &GetGeomName)
                .addStaticFunction("GetGeomTableEntry", &GetGeomTableEntry)
                .addStaticProperty("GeomTableSize", &GetGeomSetCount)

                .addStaticFunction("ResetInstanceHeap", &ResetInstanceHeap)
                .addStaticFunction("ResetAll", &ResetAll)
                .addStaticFunction("SetShadowBillboardMtx", &SetShadowBillboardMtx)
                .addStaticFunction("GetGeomSet", &GetGeomSet)

                //functions
                .addFunction("LoadBoundOnLastEntry", &LoadBoundOnLastEntry)
                .addFunction("GetBoundSphere", &GetBoundSphere)
                .addFunction("BeginGeom", &BeginGeom)
                .addFunction("AddGeom", &AddGeom)
                .addFunction("EndGeom", &EndGeom)
                .addStaticFunction("AddSphere", &AddSphere)
                .addFunction("InitBoundTerrain", &InitBoundTerrain)
                .addFunction("InitBoundTerrrainLocal", &InitBoundTerrainLocal)
                .addFunction("NeedGhostBound", &NeedGhostBound)
                .addFunction("InitGhostBound", &InitGhostBound)
                .addFunction("InitGhost", &InitGhost)
                .addStaticFunction("CreateTempBounds", &CreateTempBounds)
                .addStaticFunction("DeleteTempBounds", &DeleteTempBounds)
                .addFunction("PreLoadShader", &PreLoadShader)
                .addFunction("Optimize", &Optimize)

                //virtuals
                .addFunction("Reset", &Reset)
                .addFunction("IsVisible", &IsVisible)
                .addFunction("GetMatrix", &getMatrixLua)
                .addFunction("GetPosition", &getPositionLua)
                .addFunction("SetMatrix", &SetMatrix)
                .addFunction("SetPosition", &setPositionLua)
                .addFunction("SetVariant", &SetVariant)
                .addFunction("GetRadius", &GetRadius)
                .addFunction("GetEntity", &GetEntity)
                .addFunction("AttachEntity", &AttachEntity)
                .addFunction("GetVelocity", &GetVelocity)
                .addFunction("Detach", &Detach)
                .addFunction("Draw", &Draw)
                .addFunction("DrawShadow", &DrawShadow)
                .addFunction("DrawShadowMap", &DrawShadowMap)
                .addFunction("DrawGlow", &DrawGlow)
                .addFunction("DrawReflected", &DrawReflected)
                .addFunction("DrawReflectedParts", &DrawReflectedParts)
                .addFunction("Init", &Init)
                .addFunction("SizeOf", &SizeOf)
                .addFunction("IsLandmark", &IsLandmark)
                .addFunction("IsCollidable", &IsCollidable)
                .addFunction("IsTerrainCollidable", &IsTerrainCollidable)
                .addFunction("GetNumLightSources", &GetNumLightSources)
                //addFunction("GetLightInfo", [](int light) { cltLight light; lvlInstance::GetLightInfo(light, &light); return light; })
                .addFunction("SetupGfxLights", &SetupGfxLights)
                //.addFunction("GetBound", &GetBound) -> moved to BindLuaLate
            .endClass();
        }
    };

    class LvlInstanceLuaIterator : public CppFunctor
    {
    private:
        lvlInstance* last;
        lvlInstance* end;
    public:
        LvlInstanceLuaIterator(lvlInstance* instance)
        {
            this->last = instance;
        }

        LvlInstanceLuaIterator(lvlInstance* instance, lvlInstance* end)
        {
            this->last = instance;
            this->end = end;
        }

        virtual ~LvlInstanceLuaIterator()
        {
        }

        virtual int run(lua_State* L) override
        {
            if (this->last)
            {
                LuaState(L).push(this->last);
                this->last = this->last->getNext();
                if (this->last == this->end)
                    this->last = nullptr;
                return 1;
            }
            else
            {
                return 0;
            }
        }
    };
    // Lua initialization

}