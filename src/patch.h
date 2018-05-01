#pragma once
#include "common.h"

#include <type_traits>

/*
    Patching utilities/structs
*/

namespace mem
{
    inline bool copy(void *lpAddress, const void *lpReadAddress, size_t dwReadSize)
    {
        DWORD dwOldProtect;

        if (VirtualProtect(lpAddress, dwReadSize, PAGE_EXECUTE_READWRITE, &dwOldProtect))
        {
            memcpy(lpAddress, lpReadAddress, dwReadSize);
            VirtualProtect(lpAddress, dwReadSize, dwOldProtect, &dwOldProtect);

            return true;
        }

        return false;
    }

    template <typename T>
    inline T read(auto_ptr address) {
        DWORD dwOldProtect;

        if (VirtualProtect(address, sizeof(T), PAGE_EXECUTE_READ, &dwOldProtect))
        {
            T value = *(T *)address;
            VirtualProtect(address, sizeof(T), dwOldProtect, &dwOldProtect);

            return value;
        }

        return NULL;
    }

    template <typename ...TArgs>
    inline bool write(auto_ptr address, TArgs ...args)
    {
        static_assert(sizeof...(args) > 0,
                      "No arguments provided");

        static_assert(variadic::true_for_all<std::is_trivially_copyable<TArgs>::value...>,
                      "Not all arguments are trivially copyable");

        constexpr size_t totalSize = variadic::sum<sizeof(TArgs)...>;

        DWORD dwOldProtect;

        if (VirtualProtect(address, totalSize, PAGE_EXECUTE_READWRITE, &dwOldProtect))
        {
            void *lpDst = address;

            using variadic_unpacker_t = int[];

            (void)variadic_unpacker_t
            {
                (
                    memcpy(lpDst, &args, sizeof(args)),
                    lpDst = static_cast<char*>(lpDst) + sizeof(args),
                0)...
            };

            VirtualProtect(address, totalSize, dwOldProtect, &dwOldProtect);

            return true;
        }

        return false;
    }
}
