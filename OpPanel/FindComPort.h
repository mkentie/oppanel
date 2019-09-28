#pragma once

//Finds a COM port's device path using a device id, e.g. "usb\\vid_2341&pid_0043" to find an Arduino.
std::pair<bool, std::wstring> FindComPortPath(const wchar_t* const pszDeviceId);
