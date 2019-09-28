#include <windows.h>
#include <cfgmgr32.h>
#include <memory>
#include <initguid.h>
#include <Devpkey.h>
#include <string>
#include "FindComPort.h"

std::pair<bool, std::wstring> FindComPortPath(const wchar_t* const pszDeviceId)
{
    ULONG ulBufSize;
    if (CM_Get_Device_Interface_List_SizeA(&ulBufSize, const_cast<GUID*>(&GUID_DEVINTERFACE_COMPORT), nullptr, CM_GET_DEVICE_INTERFACE_LIST_PRESENT) != CR_SUCCESS)
    {
        return std::make_pair(false, std::wstring()); 
    }

    std::unique_ptr<wchar_t[]> BufInterfaces(new wchar_t[ulBufSize]);

    if (CM_Get_Device_Interface_List(const_cast<GUID*>(&GUID_DEVINTERFACE_COMPORT), nullptr, BufInterfaces.get(), ulBufSize, CM_GET_DEVICE_INTERFACE_LIST_PRESENT) != CR_SUCCESS)
    {
        return std::make_pair(false, std::wstring()); 
    }

    //Walk through array-of-null-terminated-strings of devices
    for (wchar_t* pszCurrentInterface = BufInterfaces.get(); *pszCurrentInterface; pszCurrentInterface += wcslen(pszCurrentInterface) + 1)
    {
        ULONG ulPropSize = 0;
        DEVPROPTYPE PropType = DEVPROP_TYPE_STRING;

        //Find device instance id of device
        if(CM_Get_Device_Interface_Property(pszCurrentInterface, &DEVPKEY_Device_InstanceId, &PropType, nullptr, &ulPropSize, 0) != CR_BUFFER_SMALL)
        {
            return std::make_pair(false, std::wstring()); 
        }

        std::unique_ptr<BYTE[]> BufInstanceId(new BYTE[ulPropSize]);
        if(CM_Get_Device_Interface_Property(pszCurrentInterface, &DEVPKEY_Device_InstanceId, &PropType, BufInstanceId.get(), &ulPropSize, 0) != CR_SUCCESS)
        {
            return std::make_pair(false, std::wstring()); 
        }

        //Find device node
        wchar_t* const pszDevInstId = reinterpret_cast<wchar_t*>(BufInstanceId.get());
        DEVINST DevInst;
        if(CM_Locate_DevNode(&DevInst, pszDevInstId, CM_LOCATE_DEVNODE_NORMAL) != CR_SUCCESS)
        {
            return std::make_pair(false, std::wstring()); 
        }

        //Get 'matching device id' e.g. "usb\vid_2341&pid_0043"
        ulPropSize = 0;
        if (CM_Get_DevNode_Property(DevInst, &DEVPKEY_Device_MatchingDeviceId, &PropType, nullptr, &ulPropSize, 0) != CR_BUFFER_SMALL)
        {
            return std::make_pair(false, std::wstring()); 
        }

        std::unique_ptr<BYTE[]> BufMatchingDeviceId(new BYTE[ulPropSize]);
        if (CM_Get_DevNode_Property(DevInst, &DEVPKEY_Device_MatchingDeviceId, &PropType, BufMatchingDeviceId.get(), &ulPropSize, 0) != CR_SUCCESS)
        {
            return std::make_pair(false, std::wstring()); 
        }

        if (wcscmp(reinterpret_cast<wchar_t*>(BufMatchingDeviceId.get()), pszDeviceId) == 0)
        {
            return std::make_pair(true, pszCurrentInterface);
        }
    }

    return std::make_pair(false, std::wstring());
}