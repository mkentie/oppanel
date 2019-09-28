#pragma once

class CCpuUsage
{
public:
    CCpuUsage();
    int GetUsage();

private:
    SYSTEM_INFO m_SystemInfo;
    ULARGE_INTEGER m_PrevIdle;
    ULARGE_INTEGER m_PrevSystem;
};
