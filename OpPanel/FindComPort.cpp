#include <windows.h>
#include <setupapi.h>
#include <memory>
#include <comdef.h>
#include <initguid.h>
#include <Devpkey.h>
#include <string>
#include "FindComPort.h"



std::pair<bool, std::wstring> FindComPortPath(const wchar_t* const pszDeviceId)
{
    static const GUID s_GUIDPorts = { 0x4d36e978,0xe325,0x11ce,{0xbf,0xc1,0x08,0x00,0x2b,0xe1,0x03,0x18} };

    class HDEVINFODeleter //RAII wrapper
    {
    public:
        typedef HDEVINFO pointer;
        void operator()(HDEVINFO h) { ::SetupDiDestroyDeviceInfoList(h); }
    };

    //Get device
    std::unique_ptr<HDEVINFO, HDEVINFODeleter> hDevInfo(SetupDiGetClassDevs(&GUID_DEVINTERFACE_COMPORT, NULL, NULL, DIGCF_PRESENT | DIGCF_PROFILE | DIGCF_DEVICEINTERFACE));
    if (hDevInfo.get() == INVALID_HANDLE_VALUE)
    {
        wprintf_s(L"SetupDiGetClassDevs: %s\n", _com_error(GetLastError()).ErrorMessage());
        return std::make_pair(false, std::wstring());
    }

    //Get device info for the devices
    SP_DEVINFO_DATA DeviceInfoData;
    DeviceInfoData.cbSize = sizeof(DeviceInfoData);
    if (!SetupDiEnumDeviceInfo(hDevInfo.get(), 0, &DeviceInfoData))
    {
        if (GetLastError() == ERROR_NO_MORE_ITEMS)
        {
            puts("No devices found.");
        }
        else
        {
            wprintf_s(L"SetupDiEnumDeviceInfo: %s\n", _com_error(GetLastError()).ErrorMessage());
        }

        return std::make_pair(false, std::wstring());
    }


    //Get the first matching device interface of that device
    SP_DEVICE_INTERFACE_DATA DeviceInterfaceData;
    DeviceInterfaceData.cbSize = sizeof(DeviceInterfaceData);
    for (DWORD iDevice = 0; SetupDiEnumDeviceInterfaces(hDevInfo.get(), &DeviceInfoData, &GUID_DEVINTERFACE_COMPORT, iDevice, &DeviceInterfaceData); iDevice++)
    {
        //Get size of detailed device interface data
        DWORD dwRequiredSize;
        if (!SetupDiGetDeviceInterfaceDetail(hDevInfo.get(), &DeviceInterfaceData, nullptr, 0, &dwRequiredSize, nullptr) && GetLastError() != ERROR_INSUFFICIENT_BUFFER)
        {
            wprintf_s(L"SetupDiGetDeviceInterfaceDetail: %s\n", _com_error(GetLastError()).ErrorMessage());
            return std::make_pair(false, std::wstring());
        }

        //SP_DEVICE_INTERFACE_DETAIL_DATA's actual size isn't declared
        std::unique_ptr<SP_DEVICE_INTERFACE_DETAIL_DATA> pInterfaceDetailData(reinterpret_cast<SP_DEVICE_INTERFACE_DETAIL_DATA*>(new BYTE[dwRequiredSize]));
        pInterfaceDetailData->cbSize = sizeof(*pInterfaceDetailData);
        if (!SetupDiGetDeviceInterfaceDetail(hDevInfo.get(), &DeviceInterfaceData, pInterfaceDetailData.get(), dwRequiredSize, nullptr, nullptr))
        {
            wprintf_s(L"SetupDiGetDeviceInterfaceDetail: %s\n", _com_error(GetLastError()).ErrorMessage());
            return std::make_pair(false, std::wstring());
        }

        //Get id size
        ULONG ulPropType = DEVPROP_TYPE_STRING;
        if (!SetupDiGetDeviceProperty(hDevInfo.get(), &DeviceInfoData, &DEVPKEY_Device_MatchingDeviceId, &ulPropType, nullptr, 0, &dwRequiredSize, 0) && GetLastError() != ERROR_INSUFFICIENT_BUFFER)
        {
            wprintf_s(L"SetupDiGetDeviceProperty: %s\n", _com_error(GetLastError()).ErrorMessage());
            return std::make_pair(false, std::wstring());
        }

        //Get device id
        std::unique_ptr<wchar_t> Buf(new wchar_t[dwRequiredSize]);
        if (!SetupDiGetDeviceProperty(hDevInfo.get(), &DeviceInfoData, &DEVPKEY_Device_MatchingDeviceId, &ulPropType, reinterpret_cast<PBYTE>(Buf.get()), dwRequiredSize, 0, 0))
        {
            wprintf_s(L"SetupDiGetDeviceProperty: %s\n", _com_error(GetLastError()).ErrorMessage());
            return std::make_pair(false, std::wstring());
        }

        if (wcscmp(Buf.get(), pszDeviceId) == 0)
        {
            //We found the device, get its name
            return std::make_pair(true, pInterfaceDetailData->DevicePath);
        }

    }

    if (GetLastError() != ERROR_NO_MORE_ITEMS)
    {
        wprintf_s(L"SetupDiEnumDeviceInterfaces: %s\n", _com_error(GetLastError()).ErrorMessage());
        return std::make_pair(false, std::wstring());
    }

    return std::make_pair(false, std::wstring());
}