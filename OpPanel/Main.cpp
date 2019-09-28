#include "StdAfx.h"
#include "FindComPort.h"
#include "CpuUsage.h"
#include "CoreTempPlugin.h"



namespace
{
    class HandleWrapper //RAII wrapper for HANDLE
    {
    public:
        HandleWrapper() : m_h(INVALID_HANDLE_VALUE) {}
        HandleWrapper(const HANDLE h) : m_h(h) {}
        HandleWrapper(const HandleWrapper&) = delete;
        HandleWrapper(HandleWrapper&& Other) noexcept : m_h(Other.m_h) { Other.m_h = INVALID_HANDLE_VALUE; }
        HandleWrapper& operator=(const HandleWrapper&) = delete;
        HandleWrapper& operator=(HandleWrapper&& Other) noexcept { std::swap(m_h, Other.m_h); return *this; }
        ~HandleWrapper()
        {
            CloseHandle(m_h);
        }

        void Reset()
        {
            CloseHandle(m_h);
            m_h = INVALID_HANDLE_VALUE;
        }

        bool IsValid() const { return m_h != INVALID_HANDLE_VALUE; }
        HANDLE Get() { return m_h; }

    private:
        HANDLE m_h;
    };

    HandleWrapper OpenComPort(const wchar_t* const pszDevPath)
    {
        HandleWrapper h(CreateFile(pszDevPath, GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr));
        if (!h.IsValid())
        {
            return h;
        }

        DCB SerialParams = {};
        SerialParams.DCBlength = sizeof(SerialParams);
        SerialParams.BaudRate = CBR_9600;
        SerialParams.ByteSize = DATABITS_8;
        SerialParams.StopBits = ONESTOPBIT;
        //serialParams.fDtrControl = DTR_CONTROL_ENABLE; //This resets the arduino
        SerialParams.Parity = NOPARITY;

        if (!SetCommState(h.Get(), &SerialParams))
        {
            return HandleWrapper();
        }

        return h;
    }

    class CPluginClass
    {
    public:
        CPluginClass(HandleWrapper&& OpenedComPort)
        :m_ComPortHandle(std::move(OpenedComPort))
        ,m_iInfoType(EInfoType::SPEED)
        {

        }

        void Update(const CoreTempSharedData& Data)
        {
            m_iInfoType = static_cast<EInfoType>((static_cast<int>(m_iInfoType) + 1) % static_cast<int>(EInfoType::TYPES_COUNT));

            constexpr size_t iDisplayChars = 8;
            char buf[iDisplayChars+1]; //Need to store null terminator

            switch (m_iInfoType)
            {
            case EInfoType::SPEED:
                {
                    snprintf(buf, _countof(buf), "%4.fMHz", Data.fCPUSpeed);
                }
                break;
            case EInfoType::LOAD:
                {
                    const int iLoad = std::accumulate(Data.uiLoad, Data.uiLoad + Data.uiCoreCnt, 0) / Data.uiCoreCnt;
                    const float fTemp = *std::max_element(Data.fTemp, Data.fTemp + Data.uiCoreCnt);
                    snprintf(buf, _countof(buf), "%3d%%%3.f\x01b", iLoad, fTemp);
                }
                break;
            }

            DWORD dwWritten;
            WriteFile(m_ComPortHandle.Get(), buf, iDisplayChars, &dwWritten, nullptr);
        }

    private:
        HandleWrapper m_ComPortHandle;
        enum class EInfoType { SPEED, LOAD, TYPES_COUNT };
        EInfoType m_iInfoType; //What info to display
    };

    std::unique_ptr<CPluginClass> s_ThePluginPtr;
}



namespace CoreTempPluginFuncs
{

    int Start()
    {
        auto [bFound, strPath] = FindComPortPath(L"usb\\vid_2341&pid_0043");

        if (!bFound)
        {
            return 1;
        }

        HandleWrapper h = OpenComPort(strPath.c_str());
        if (!h.IsValid())
        {
            return 1;
        }

        s_ThePluginPtr.reset(new CPluginClass(std::move(h)));

        return 0; //0 = success
    }

    void Update(const LPCoreTempSharedData data)
    {
        if (!s_ThePluginPtr || !data)
        {
            return;
        }

        s_ThePluginPtr->Update(*data);
    }

    void Stop()
    {
        s_ThePluginPtr.reset();
    }

    int Configure()
    {
        return 0; //0 = not implemented
    }

    void Remove(const wchar_t* const /*path*/)
    {

    }
}

extern "C" __declspec(dllexport) LPCoreTempPlugin WINAPI GetPlugin(HMODULE /*hModule*/)
{
    //The docs suggest we should heap-allocate a plugin instance, but as there's no handle or anything that allows CoreTemp to create multiple plugin instances, we don't bother
    static CoreTempPluginInfo PluginInfo;
    PluginInfo.name = L"OpPanel";
    PluginInfo.version = L"1.0";
    PluginInfo.description = L"IBM PS/2 Model 95 Operator Panel Plugin";

    static CoreTempPlugin Plugin;
    Plugin.interfaceVersion = 1;
    Plugin.type = General_Type;
    Plugin.pluginInfo = &PluginInfo;
    Plugin.Start = CoreTempPluginFuncs::Start;
    Plugin.Update = CoreTempPluginFuncs::Update;
    Plugin.Stop = CoreTempPluginFuncs::Stop;
    Plugin.Configure = CoreTempPluginFuncs::Configure;
    Plugin.Remove = CoreTempPluginFuncs::Remove;

    return &Plugin;
}

extern "C" __declspec(dllexport) void WINAPI ReleasePlugin()
{

}
