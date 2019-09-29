#pragma once

//Finds a COM port's device path using a device id, e.g. "usb\\vid_2341&pid_0043" to find an Arduino.
//An example device path would be "\\\\?\\USB#VID_2341&PID_0043#757353034313519152F0#{86e0d1e0-8089-11d0-9ce4-08003e301f73}"
std::pair<bool, std::wstring> FindComPortPath(const wchar_t* const pszDeviceId);
