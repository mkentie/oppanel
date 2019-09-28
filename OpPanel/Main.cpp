#include <windows.h>
#include <string>
#include "FindComPort.h"

class HandleWrapper
{
public:
    HandleWrapper() : m_h(INVALID_HANDLE_VALUE) {}
    HandleWrapper(const HANDLE h) : m_h(h) {}
    HandleWrapper(const HandleWrapper&) = delete;
    HandleWrapper(HandleWrapper&& Other) noexcept : m_h(Other.m_h) { Other.m_h = INVALID_HANDLE_VALUE; }
    HandleWrapper& operator=(const HandleWrapper&) = delete;
    ~HandleWrapper()
    {
        CloseHandle(m_h);
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

int main(int argc, char* argv[])
{
    auto [bSuccess, strDevicePath] = FindComPortPath(L"usb\\vid_2341&pid_0043");
    if (!bSuccess)
    {
        puts("Arduino not found.");
        return EXIT_FAILURE;
    }

    HandleWrapper h = OpenComPort(strDevicePath.c_str());
    if (!h.IsValid())
    {
        puts("Failed to open COM port.");
        return EXIT_FAILURE;
    }

    DWORD dwWritten = 0;
    WriteFile(h.Get(), "\rblabla12", 9, &dwWritten, nullptr);

    return EXIT_SUCCESS;

}