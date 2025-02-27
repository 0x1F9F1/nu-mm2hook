#include "main.h"
#include "mm2_data.h"

#include "handlers\bugfix_handlers.h"
#include "handlers\feature_handlers.h"

#include "handlers\print_handler.h"

#include <discord-presence.h>

#include <imgui\renderer\imgui_age_rendernode.h>

using namespace MM2;

static ConfigValue<int> cfgRandomSeed           ("RandomSeed",          "seed",             0);
static ConfigValue<int> cfgHeapSize             ("HeapSize",            "heapsize",         128);
static ConfigValue<bool> cfgUseOldAutoDetect    ("UseOldAutoDetect",    "oldautodetect",    false);
static ConfigValue<bool> cfgDisableMutex        ("DisableMutex",        "nomutex",          false);

static ConfigValue<bool> cfgShowConsole         ("ShowConsole",                             true);

static ConfigValue<bool> cfgDebugLog            ("DebugLog",            "log",              true);
static ConfigValue<int> cfgDebugLogLevel        ("DebugLogLevel",       "loglevel",         3);
static ConfigValue<bool> cfgAGEDebug            ("AGEDebug",            "ageDebug",         false);

// use BOOL so we can set the corresponding values directly
static ConfigValue<BOOL> cfgGfxDebug            ("GfxDebug",            "gfxDebug",         FALSE);
static ConfigValue<BOOL> cfgAudioDebug          ("AudioDebug",          "audDebug",         FALSE);
static ConfigValue<BOOL> cfgJoystickDebug       ("JoystickDebug",       "joyDebug",         FALSE);
static ConfigValue<BOOL> cfgAssetDebug          ("AssetDebug",          "assetDebug",       FALSE);
static ConfigValue<bool> cfgPhysicsDebug        ("PhysicsDebug",        "physDebug",        false);

// ==========================
// Game-related properties
// ==========================

static int RandomSeed = 0;

/* AGE Debugging */
FILE *ageLogFile = nullptr;
bool bAgeDebug = false;

// ==========================
// Pointer hooks
// ==========================
hook::Type<asNode> ROOT                              = 0x661738;

/*
    ===========================================================================
*/

/*
    Custom coreFileMethods implementation for log files
*/
class coreLogFile {
public:
    static int Open(LPCSTR filename, bool) {
        return (int)LogFileStream::Open(filename);
    }

    static int Create(LPCSTR filename) {
        return (int)LogFileStream::Create(filename);
    }

    static int Read(int, LPVOID, int) {
        return 0;
    };

    static int Write(int handle, const LPVOID buffer, int length) {
        if (length > 0)
        {
            // ensure the buffer is clean
            char buf[4096] = { NULL };
            strncpy(buf, (LPCSTR)buffer, length);

            auto logFile = reinterpret_cast<LogFileStream *>(handle);

            logFile->Write(buf);
            logFile->Flush(false);
        }
        return length;
    }

    static int Close(int handle) {
        reinterpret_cast<LogFileStream *>(handle)->Close();
        return 0;
    }

    static int Flush(int handle) {
        reinterpret_cast<LogFileStream *>(handle)->Flush(true);
        return 0;
    }
};

const coreFileMethods logFileMethods = {
    &coreLogFile::Open,
    &coreLogFile::Create,
    &coreLogFile::Read,
    &coreLogFile::Write,
    NULL,
    &coreLogFile::Close,
    NULL,
    NULL,
    &coreLogFile::Flush,
};

// ==========================
// Callback handlers
// ==========================

class CallbackHandler {
public:
    static void ProgressRect(int x, int y, int width, int height, UINT color) {

        DDPIXELFORMAT ddPixelFormat = { sizeof(ddPixelFormat) };
        lpdsRend->GetPixelFormat(&ddPixelFormat);

        DDBLTFX ddBltFx = { sizeof(ddBltFx) };
        ddBltFx.dwFillColor = GetPixelFormatColor(&ddPixelFormat, color);

        RECT position = {
            x,
            y,
            x + width,
            y + height,
        };

        lpdsRend->Blt(&position, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddBltFx);
    }

    static void CreateGameMutex(LPCSTR lpName) {
        if (cfgDisableMutex) {
            LogFile::WriteLine("Game mutex disabled.");
        } else {
            $CreateGameMutex(lpName);
        }
    }

    static void ageDebug(int debug, const char* format, ...) {
        va_list va;

        if (bAgeDebug)
        {
            // print to AGE.log if user specified -ageDebug

            va_start(va, format);
            vfprintf(ageLogFile, format, va);
            va_end(va);

            fputc('\n', ageLogFile);
            // fflush(ageLogFile);
        } else if (debug) {
            va_start(va, format);

            // treat as Messagef
            Printer(1, format, va);

            va_end(va);
        }
    }

    static LocString * AngelReadString(UINT stringId) {
        static const LPCSTR STRING_UNKNOWN = "?? lang:%d ??";

        static HMODULE h_MMLANG = NULL;

        static LocString string_buffer[8];
        static int string_index = 0;

        LPCSTR str = NULL;

        if (MM2Lua::IsInitialized())
        {
            auto L = MM2Lua::GetState();
            
            LuaRef func(*L, "GetLocaleString");
            if (func.isFunction())
                str = func.call<LPCSTR>();
        }

        auto locStr = &string_buffer[(string_index++ & 0x7)];

        // not found in Lua, let's look in MMLANG.DLL
        if (str == NULL)
        {
            // revert to MMLANG.DLL
            if (h_MMLANG == NULL)
            {
                if ((h_MMLANG = LoadLibrary("MMLANG.DLL")) == NULL)
                {
                    MessageBox(NULL, "MMLANG.DLL not found.", "Midtown Madness 2", MB_ICONHAND);
                    ExitProcess(0);
                }
            }

            if ($MyLoadStringA(h_MMLANG, stringId, locStr, sizeof(LocString)) == 0)
            {
                // string wasn't in Lua or DLL, return an unknown string
                // e.g. "?? lang:123 ??"
                sprintf((char *)locStr, STRING_UNKNOWN, stringId);
            }
        } else {
            strcpy(locStr->buffer, str);
        }

        return locStr;
    }

    static void ComputeCpuSpeed() {
        *mmCpuSpeed = 9000;
    }

    static BOOL __stdcall AutoDetectCallback(GUID *lpGUID,
                                             LPSTR lpDriverDescription, LPSTR lpDriverName, LPVOID lpContext)
    {
        stopwatch timer;

        Displayf("AutoDetect: GUID=%x, Description=%s, Name=%s", lpGUID, lpDriverDescription, lpDriverName);

        timer.start();

        if (lpDirectDrawCreateEx(lpGUID, (LPVOID*)&lpDD, IID_IDirectDraw7, nullptr) == DD_OK)
        {
            Displayf("  Created device in %.4f ms", timer.elapsedMilliseconds());

            gfxInterface *gfxInterface = gfxInterfaces[gfxInterfaceCount];

            strcpy (gfxInterface->Name, lpDriverDescription);

            gfxInterface->DeviceCaps = 1;
            gfxInterface->AcceptableDepths = gfxDepthFlags::Depth32;

            DDDEVICEIDENTIFIER2 ddDeviceIdentifier = { NULL };

            if (lpDD->GetDeviceIdentifier(&ddDeviceIdentifier, 0) == DD_OK)
            {
                gfxInterface->VendorID  = ddDeviceIdentifier.dwVendorId;
                gfxInterface->DeviceID  = ddDeviceIdentifier.dwDeviceId;
                gfxInterface->GUID      = ddDeviceIdentifier.guidDeviceIdentifier;
            }

            if (lpDD->QueryInterface(IID_IDirect3D7, (LPVOID*)&lpD3D) == DD_OK)
            {
                lpD3D->EnumDevices($DeviceCallback, gfxInterface);
                lpD3D->Release();

                lpD3D = nullptr;
            }

            gfxInterface->DeviceType        = gfxDeviceType::HardwareWithTnL;

            gfxInterface->ResolutionCount   = 0;
            gfxInterface->ResolutionChoice  = 0;

            DWORD availableMemory = 0x40000000; // 1GB = 1024 * 1024 * 1024

            DDSCAPS2 ddsCaps = { NULL };

            ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;

            if (lpDD->GetAvailableVidMem(&ddsCaps, &availableMemory, NULL) != DD_OK)
                Warningf("  Couldn't get video memory, using default");

            Displayf("  Total video memory: %dMB", (availableMemory >> 20));

            gfxInterface->AvailableMemory = availableMemory;

            gfxMaxScreenWidth = 0;
            gfxMaxScreenHeight = 0;

            stopwatch enumTimer;

            enumTimer.start();
            lpDD->EnumDisplayModes(0, 0, gfxInterface, $ResCallback);
            lpDD->Release();
            enumTimer.stop();

            Displayf("  Enumerated display modes in %.4f ms", enumTimer.elapsedMilliseconds());

            lpDD = nullptr;

            ++*gfxInterfaceCount;
        }

        timer.stop();

        Displayf("  Finished in %.4f ms", timer.elapsedMilliseconds());

        return TRUE;
    }

    static bool isVehInfoFile(const char *filename) {
        auto ext = strrchr(filename, '.');
        return (ext != NULL) ? (_strcmpi(ext, ".info") == 0) : false;
    }

    static bool isCityInfoFile(const char *filename) {
        auto ext = strrchr(filename, '.');
        return (ext != NULL) ? (_strcmpi(ext, ".cinfo") == 0) : false;
    }

    bool ParseStateArgs(void) {
        if (datArgParser::Get("allrewards"))
            MMSTATE->UnlockRewards = true;

        return true;
    }

    void GenerateRandomSeed() {
        RandomSeed = stopwatch::ticks();
    }

    void ResetRandomSeed() {
        // make sure the seed was generated at least one time
        if (RandomSeed == 0)
            GenerateRandomSeed();

        gRandSeed = RandomSeed;
    }

    static void Install() {
        InstallCallback("CreateGameMutex", "Adds '-nomutex' argument to allow multiple game processes.",
            &CreateGameMutex, {
                cb::call(0x40128D),
            }
        );

        InstallCallback("CheckGlobalMemory", "Disables check for available memory.",
            &NullSub, {
                cb::call(0x401295),
            }
        );

        InstallCallback("ageDebug", "Verbose debug logger.",
            &ageDebug, {
                cb::jmp(0x402630),
            }
        );

        InstallCallback("ProgressRect", "Fixes white loading bar in 32-bit display mode.",
            &ProgressRect, {
                cb::call(0x401163),
                cb::call(0x4011CC),
            }
        );

        if (!cfgUseOldAutoDetect)
        {
            InstallCallback("ComputeCpuSpeed", "Removes the CPU speed calculation for the old auto detect method and improves startup times.",
                &ComputeCpuSpeed, {
                    cb::call(0x401208),
                }
            );

            // cb::hook into the original AutoDetect and replace it with our own version
            InstallCallback("AutoDetectCallback", "Replaces the default AutoDetect method with a much faster one.",
                &AutoDetectCallback, {
                    cb::jmp(0x4AC030),
                }
            );
        }

        InstallCallback("isVehInfoFile", "Fixes random crashes.",
            &isVehInfoFile, {
                cb::call(0x5248E1),
            }
        );

        InstallCallback("isCityInfoFile", "Fixes random crashes.",
            &isCityInfoFile, {
                cb::call(0x5244CF),
            }
        );

        // NOTE: Completely overrides the original AngelReadString (will check Lua first then DLL)
        InstallCallback("AngelReadString", "Adds support for Lua-based locale. Uses MMLANG.DLL on Lua failure.",
            &AngelReadString, {
                cb::jmp(0x534790),
            }
        );

        InstallCallback("zipFile::Init", "Fixes 'extraLen' spam in the console/log.",
            &NullSub, {
                cb::call(0x5738EA),
            }
        );

        // don't print certain errors unless specified
        if (!cfgPhysicsDebug) {
            InstallCallback(&NullSub, {
                    cb::call(0x469A20), // ; 'CollideInstances: Attempting to collide instances without bounds'
                    cb::call(0x4692C5), // ; 'dgPhysManager::CollideProbe : instance has no bound'
                    cb::call(0x469B24), // ; 'dgPhysManager::CollideTerrain - entry in room 0'
                }, "Disables physics collision error debugging (use '-physDebug' to enable)."
            );
        }

        InstallCallback(&ParseStateArgs, {
            cb::call(0x4013A4)
        }, "State pack argument parsing.");

        if (cfgRandomSeed.Get(RandomSeed)
            && (RandomSeed > 0))
        {
            InstallCallback("ResetRandomSeed", "Resets the random seed to a user-specified one.",
                &ResetRandomSeed, {
                    cb::call(0x4068F0), // mmReplayManager::ctor
                    cb::call(0x406993), // mmReplayManager::Reset
                    cb::call(0x444B79), // cityLevel::Load
                    cb::call(0x536A68), // aiMap::Reset
                }
            );
        }
        else if ((RandomSeed == -1)
            || datArgParser::Get("randy"))
        {
            InstallCallback("GenerateRandomSeed", "Generates a new random seed instead of resetting it to a fixed value.",
                &GenerateRandomSeed, {
                    cb::call(0x4068F0), // mmReplayManager::ctor
                    cb::call(0x444B79), // cityLevel::Load
                }
            );

            InstallCallback("ResetRandomSeed", "Resets the random seed to one we previously generated.",
                &ResetRandomSeed, {
                    cb::call(0x406993), // mmReplayManager::Reset
                    cb::call(0x536A68), // aiMap::Reset
                }
            );
        }
    }
};

static volatile struct {
    HINSTANCE instance;

    intptr_t begin;
    intptr_t end;

    size_t size;
} meminfo = { nullptr };

static bool InitModuleInfo(HINSTANCE instance) {
    intptr_t base = reinterpret_cast<intptr_t>(instance);

    PIMAGE_DOS_HEADER dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(base);

    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
        return false;

    PIMAGE_NT_HEADERS peHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(base + dosHeader->e_lfanew);

    if (peHeader->Signature != IMAGE_NT_SIGNATURE)
        return false;

    meminfo.instance = instance;

    meminfo.begin = base;
    meminfo.end = base + (peHeader->OptionalHeader.BaseOfCode + peHeader->OptionalHeader.SizeOfCode);
    meminfo.size = peHeader->OptionalHeader.SizeOfImage;

    LogFile::Printf(1, "module info:");
    LogFile::Printf(1, " begin: %08X", meminfo.begin);
    LogFile::Printf(1, " end: %08X", meminfo.end);
    LogFile::Printf(1, " size: %08X", meminfo.size);

    return true;
}

class StackHandler {
public:
    static void GetAddressName(char *buffer, LPCSTR, int address) {
        bool unknown = true;

        if (meminfo.instance != nullptr) {
            /*
                TODO: Retrieve actual symbols from MM2Hook
            */
            if ((address >= meminfo.begin) && (address <= meminfo.end)) {
                intptr_t relAddr = (address - meminfo.begin);

                sprintf(buffer, "%08x (MM2Hook+%x)", address, relAddr);

                unknown = false;
            }
        }

        if (unknown)
            sprintf(buffer, "%08x (Unknown)", address);
    }

    static void GetAddressName(char *buffer, LPCSTR, int address, char *fnSymbol, int offset) {
        char fnName[1024] = { NULL };

        // no error checking (for now?)
        UnDecorateSymbolName(fnSymbol, fnName, sizeof(fnName),
            UNDNAME_COMPLETE
            | UNDNAME_NO_FUNCTION_RETURNS
            | UNDNAME_NO_ALLOCATION_MODEL
            | UNDNAME_NO_ALLOCATION_LANGUAGE
            | UNDNAME_NO_ACCESS_SPECIFIERS
            | UNDNAME_NO_THROW_SIGNATURES
            | UNDNAME_NO_MEMBER_TYPE
            | UNDNAME_NO_RETURN_UDT_MODEL
        );

        sprintf(buffer, "%08x (\"%s\"+%x)", address, fnName, offset);
    }

    static void Install() {
        InstallCallback("datStack::ExceptionFilter", "Custom exception filter",
            &MM2::datStack::ExceptionFilter, {
                cb::jmp(0x4C7720), //redirect function
            });

        InstallCallback("datStack::ExceptionFilter", "Custom exception filter",
            &MM2::datStack::ExceptionFilterCombined, {
                cb::call(0x4022FF), //redirect function
            });

        InstallCallback("datStack::LookupAddress", "Allows for more detailed address information.",
            static_cast<void (*)(char*, LPCSTR, int, char*, int)>(&GetAddressName), {
                cb::call(0x4C74DD), // sprintf
            }
        );

        InstallCallback("datStack::LookupAddress", "Allows for more detailed information of unknown symbols.",
            static_cast<void(*)(char*, LPCSTR, int)>(&GetAddressName), {
                cb::call(0x4C74B9), // sprintf
            }
        );
    }
};


class HookSystemFramework
{
private:
    /*
        Installs all of the callbacks for MM2Hook.
    */
    static void InstallHandlers() {
        /*
            Initialize the important handlers first
        */

        InstallHandler<CallbackHandler>("Generic callbacks");
        InstallHandler<PrintHandler>("Print system");
        InstallHandler<StackHandler>("Stack information");

        InstallHandler<GameEventDispatcher>("Event dispatcher");
        InstallHandler<discordHandler>("Discord Rich Presence");

        /*
            Now install everything else
        */

        init_base::RunAll();
    }

    static void InstallPatches() {
        InstallPatch("Increase cop limit", { 64 }, {
            0x55100B,
        });

        InstallPatch("Fix crash for missing images", { 0xEB /* jnz -> jmp */ }, {
            0x4B329B, // gfxGetBitmap
        });

        InstallPatch("Disable lock check", { 0x65 /* jnz 40130D */ }, {
            0x4012A7, // Main
        });

        InstallPatch("Fixes being kicked in multiplayer when losing focus", { 0xB8, 0x00, 0x00, 0x00, 0x00, 0xC3 /* mov eax, 0 -> ret */ }, {
            0x4390F0,   //mmGameMulti::LostCallback
        });

        InstallPatch({ 0x4F, 0x77, 0x4F }, {
            0x5DBE71,
            0x5DBE81,
        });

        InstallPatch("Fixes AI shadows not being drawn sometimes.", { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }, {
            0x552CD5, // aiVehicleInstance::DrawShadow
        });
    }
public:
    static void Initialize(int argc, char **argv) {
        LogFile::WriteLine("Installing patches...");
        InstallPatches();

        LogFile::WriteLine("Installing handlers...");
        InstallHandlers();

        // limit the amount of logging if specified
        // otherwise, everything will be captured
        //   1 = Printf, Messagef, Displayf
        //   2 = Warningf
        //   3 = Errorf
        int outputMask = 0;

        if (cfgDebugLog && !datArgParser::Get("nolog")) {
            if (!datOutput::OpenLog("mm2.log", &logFileMethods))
                LogFile::Print(2, "Failed to initialize MM2 log!");

            int logLevel = cfgDebugLogLevel;

            if (logLevel >= 1)
                outputMask |= 2;
            if (logLevel >= 2)
                outputMask |= 4;
            if (logLevel >= 3)
                outputMask |= 8;
        }

        datOutput::SetOutputMask(outputMask);

        if (cfgAGEDebug || datArgParser::Get("age_debug"))
        {
            // AGE.log is a catch-all debug log
            // it will output _all_ debug to a file

            ageLogFile = fopen("AGE.log", "w+");
            bAgeDebug = true;
        } else {
            // these will output to the console and mm2.log if specified

            gfxDebug = cfgGfxDebug;
            audDebug = cfgAudioDebug;
            joyDebug = cfgJoystickDebug;
            assetDebug = cfgAssetDebug;
        }
    }

    static void Update(bool parsedStateArgs) {
        GameEventDispatcher::onStateBegin();
        hook::StaticThunk<0x401A00>::Call<void>(parsedStateArgs); // GameLoop
        GameEventDispatcher::onStateEnd();
    }

    static void BeginPhase(bool a1) {
        //call original
        hook::StaticThunk<0x401AA0>::Call<void>(a1);

        //initialize imgui
        auto imguiNode = new mmImGuiManager();
        MM2::ROOT->AddChild(imguiNode);

        //initialize lua
        MM2Lua::Initialize();
        GameEventDispatcher::BeginPhase(a1);
    }

    static void EndPhase() {
        GameEventDispatcher::EndPhase();
        
        // shutdown imgui
        if (mmImGuiManager::Instance != nullptr) {
            delete mmImGuiManager::Instance;
        }

        //call original
        hook::StaticThunk<0x401FC0>::Call<void>();
    }

    static void Shutdown() {
        LogFile::WriteLine("Hook shutdown request received.");

        // gfxPipeline::EndGfx2D
        hook::StaticThunk<0x4AAA10>::Call<void>();

        // close this stuff as late as possible
        atexit([](){
            // close datOutput log
            datOutput::CloseLog();

            if (ageLogFile)
                fclose(ageLogFile);

            ConsoleLog::Close();
            LogFile::Close();
        });
    }

    static void Install() {
        LogFile::WriteLine("Installing framework...");

        InstallCallback(
            &Update, {
                cb::call(0x401989), // MainPhase
            }, "GameLoop hook" );

        InstallCallback(
            &Shutdown, {
                cb::call(0x40161B) // Main
            }, "Shutdown hook");

        InstallCallback(
            &BeginPhase, {
                cb::call(0x401704),
            }, "BeginPhase hook" );

        InstallCallback(
            &EndPhase, {
                cb::call(0x4019E2), 
            }, "EndPhase hook" );
        /*
            We'll hook into ArchInit (an empty function),
            and use it to install our callbacks/patches.

            However, this time around, we can now use datArgParser
            to determine if a patch/callback should be installed or not,
            whereas before we needed to check after it was already hooked in.

            Basically, this method is a lot safer, and guarantees
            we'll have access to any arguments passed.
        */

        InstallCallback("ArchInit", "Allows the hook to initialize before the game starts.",
            &Initialize, {
                cb::call(0x4023DB),
            }
        );

        /*
            IMPORTANT:
            Add any patches/callbacks here that must be initialized prior to the game's entry point.
            This should be used for very very advanced callbacks/patches only!
        */
    }
};

/*
    ===========================================================================
*/
//
// Initialize all the important stuff prior to MM2 starting up.
// NOTE: We do not have access to datArgParser yet.
//
void Initialize(ageInfoLookup &gameInfo) {
    // initialize game manager
    pMM2 = new CMidtownMadness2(gameInfo.info);
    pMM2->Initialize();

    // install the framework
    HookSystemFramework::Install();
}

bool IsGameSupported(ageInfoLookup &gameInfo) {
    LogFile::WriteLine("Checking for known MM2 versions...");

    if (CMidtownMadness2::GetGameInfo(gameInfo))
    {
        LogFile::Format(" - Detected game version %d\n", gameInfo.info.engineVersion);
        return gameInfo.isSupported;
    } else {
        LogFile::WriteLine("Unknown module detected! Terminating...");
        MessageBox(NULL, "Unknown module! MM2Hook will now terminate the process.", "MM2Hook", MB_OK | MB_ICONERROR);

        ExitProcess(EXIT_FAILURE);
    }

    return false;
}

bool getPathSpec(char *path, char *dest, int destLen) {
    char ch;
    int idx = 0, len = 0;

    if ((path != NULL) && (dest != NULL))
    {
        while ((ch = path[idx++]) != NULL) {
            if (ch == '\\')
                len = idx;
        }

        if (len < destLen) {
            strncpy(dest, path, len);
            return true;
        }
    }

    return false;
}

static char mm2_path[MAX_PATH]{ NULL };

void initPath(void) {
    char dir[MAX_PATH]{ NULL };
    auto len = GetModuleFileName(NULL, dir, MAX_PATH);

    if (getPathSpec(dir, mm2_path, len)) {
        SetCurrentDirectory(mm2_path);
    } else {
        GetCurrentDirectory(MAX_PATH, mm2_path);
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call)
	{
        case DLL_PROCESS_ATTACH:
        {
            debug("Initializing MM2Hook...");

            // setup the current directory
            initPath();

            // Initialize the log file
            LogFile::Initialize("mm2hook.log", "--<< MM2Hook log file >>--\n");

            bool configLoaded = HookConfig::Initialize("mm2hook.ini");

            if (cfgShowConsole) {
                ConsoleLog::Initialize();
                ConsoleLog::SetTitle("MM2Hook Console");
            }

            if (!InitModuleInfo(hModule))
                LogFile::Printf(3, "Couldn't initialize module info!");

            LogFile::Format("Working directory is '%s'\n", mm2_path);

            if (configLoaded) {
                LogFile::WriteLine("Configuration file loaded successfully.");
            } else {
                LogFile::WriteLine("No configuration file was found.");
            }

            HMODULE hDIModule = NULL;
            ageInfoLookup gameInfo;

            if (IsGameSupported(gameInfo))
            {
                if (LoadSystemLibrary("dinput.dll", &hDIModule) &&
                    GetHookProcAddress(hDIModule, "DirectInputCreateA", (FARPROC*)&lpDICreate))
                {
                    LogFile::WriteLine("Injected into the game process successfully.");

                    // initialize the hook
                    Initialize(gameInfo);
                } else {
                    LogFile::WriteLine("Failed to inject into the game process.");
                    MessageBox(NULL, "Could not inject into the game process. Unknown errors may occur.", "MM2Hook", MB_OK | MB_ICONWARNING);
                }
            }
            else
            {
                LogFile::WriteLine("Unsupported game version -- terminating...");
                MessageBox(NULL, "Sorry, this version of MM2 is unsupported. Please remove MM2Hook to launch the game.", "MM2Hook", MB_OK | MB_ICONERROR);

                ExitProcess(EXIT_FAILURE);
            }
        } break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
        break;
	}
	return TRUE;
}
