#pragma once
#include <modules\node\node.h>

namespace MM2
{
    // Forward declarations
    class MenuManager;

    // External declarations
    extern class UIMenu;

    // Class definitions
    class MenuManager : public asNode {
    private:
        char _buf[0x138];
    private:
        static hook::Type<MenuManager*> Instance;

        static hook::Field<0x138, int> _currentMenuId;
        static hook::Field<0x114, int> _lastMenuId;
    public:
        static MenuManager* GetInstance()
        {
            return Instance.get();
        }

        int GetCurrentMenuID() const {
            return _currentMenuId.get(this);
        }

       void Switch(int menuID)                                   { hook::Thunk<0x4E5A30>::Call<void>(this, menuID); }
       void OpenDialog(int menuID)                               { hook::Thunk<0x4E5110>::Call<void>(this, menuID); }
       void EnableNavBar()                                       { hook::Thunk<0x4E5270>::Call<void>(this); }
       void DisableNavBar()                                      { hook::Thunk<0x4E5290>::Call<void>(this); }
       void PlaySound(int id)                                    { hook::Thunk<0x4E5320>::Call<void>(this, id); }
       UIMenu* GetCurrentMenu()                                  { return hook::Thunk<0x4E58D0>::Call<UIMenu*>(this); }

       static void BindLua(LuaState L) {
           LuaBinding(L).beginExtendClass<MenuManager, asNode>("MenuManager")
               .addStaticProperty("Instance", &GetInstance)
               .addPropertyReadOnly("CurrentMenuID", &GetCurrentMenuID)
               .addFunction("Switch", &Switch)
               .addFunction("OpenDialog", &OpenDialog)
               .addFunction("EnableNavBar", &EnableNavBar)
               .addFunction("DisableNavBar", &DisableNavBar)
               .addFunction("PlaySound", &PlaySound)
               .endClass();
       }
    };
    ASSERT_SIZEOF(MenuManager, 0x150);
}