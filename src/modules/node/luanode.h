#pragma once
#include <modules\node\cullmgr.h>

namespace MM2
{
    // Forward declarations
    class luaNode;

    // External declarations
    extern class datParser;
    extern class asCullManager;

    // Class definitions
    class luaNode : public asNode {
    private:
        LPCSTR m_ClassName;
        LuaRef m_UpdateFunction;
        LuaRef m_UpdatePausedFunction;
        LuaRef m_ResetFunction;
        LuaRef m_ResChangeFunction;
        LuaRef m_CullFunction;
        bool m_AutoDeclareCullbale;
    public:
        ANGEL_ALLOCATOR

            luaNode(LPCSTR name) {
            m_ClassName = "luaNode";
            m_UpdateFunction = getNil();
            m_UpdatePausedFunction = getNil();
            m_ResetFunction = getNil();
            m_ResChangeFunction = getNil();
            m_CullFunction = getNil();

            this->SetName(name);
            this->m_AutoDeclareCullbale = false;
        }

        //lua getter
        LuaRef getNil() {
            return LuaRef();
        }

        //lua setters
        void SetUpdateFunction(LuaRef function) {
            if (function.isFunction() && function.isValid())
                m_UpdateFunction = function;
        }

        void SetUpdatePausedFunction(LuaRef function) {
            if (function.isFunction() && function.isValid())
                m_UpdatePausedFunction = function;
        }

        void SetResetFunction(LuaRef function) {
            if (function.isFunction() && function.isValid())
                m_ResetFunction = function;
        }

        void SetResChangeFunction(LuaRef function) {
            if (function.isFunction() && function.isValid())
                m_ResChangeFunction = function;
        }

        void SetCullFunction(LuaRef function) {
            if (function.isFunction() && function.isValid())
                m_CullFunction = function;
        }

        //overrides
        char* GetClassName() override {
            return (char*)m_ClassName;
        }

        void Update() override {
            //Warningf("luaNode::Update");
            if (m_UpdateFunction.isValid() && m_UpdateFunction.isFunction())
                m_UpdateFunction();
            if (m_AutoDeclareCullbale)
                asCullManager::Instance->DeclareCullable(this);
            asNode::Update();
        }

        void UpdatePaused() override {
            //Warningf("luaNode::UpdatePaused");
            if (m_UpdatePausedFunction.isValid() && m_UpdatePausedFunction.isFunction())
                m_UpdatePausedFunction();
            asNode::UpdatePaused();
        }

        void Reset() override {
            //Warningf("luaNode::Reset");
            if (m_ResetFunction.isValid() && m_ResetFunction.isFunction())
                m_ResetFunction();
            asNode::Reset();
        }

        void ResChange(int width, int height) override {
            //Warningf("luaNode::ResChange");
            if (m_ResChangeFunction.isValid() && m_ResChangeFunction.isFunction())
                m_ResChangeFunction(width, height);
            asNode::ResChange(width, height);
        }

        void Cull() override {
            //Warningf("luaNode::Cull");
            if (m_CullFunction.isValid() && m_CullFunction.isFunction())
                m_CullFunction();
            //don't call base here because base does nothing
        }

        //lua
        static void BindLua(LuaState L) {
            LuaBinding(L).beginExtendClass<luaNode, asNode>("luaNode")
                .addConstructor(LUA_ARGS(LPCSTR))
                .addVariableRef("AutoDeclareCullable", &luaNode::m_AutoDeclareCullbale)
                .addProperty("Update", &getNil, &SetUpdateFunction)
                .addProperty("UpdatePaused", &getNil, &SetUpdatePausedFunction)
                .addProperty("Reset", &getNil, &SetResetFunction)
                .addProperty("ResChange", &getNil, &SetResChangeFunction)
                .addProperty("Cull", &getNil, &SetCullFunction)
                .endClass();
        }
    };
}