#pragma once
#include "mm2_common.h"

#include <modules\gfx.h>
#include <modules\lights.h>

namespace MM2
{
    //Forward declarations

    //External declarations

    //Lua initialization
    template<>
    void luaAddModule<module_gfx>(LuaState L) {
        luaBind<ltLight>(L);
        luaBind<ltLensFlare>(L);
        luaBind<gfxTexture>(L);
        luaBind<gfxTextureCacheEntry>(L);
        luaBind<gfxTextureCachePool>(L);

        LuaBinding(L).addFunction("gfxGetTexture", &gfxGetTexture);
    }
}
