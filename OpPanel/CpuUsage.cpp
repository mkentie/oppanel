#include "StdAfx.h"
#include "CpuUsage.h"

CCpuUsage::CCpuUsage()
:m_PrevIdle()
,m_PrevSystem()
{
    GetSystemInfo(&m_SystemInfo);
}

int CCpuUsage::GetUsage()
{
    //Determine average cpu usage since last call by comparing old system and idle to new system and idle time.
    FILETIME idleTime, kernelTime, userTime, systemTime;
    SYSTEMTIME SsystemTime;
    GetSystemTimes(&idleTime,&kernelTime,&userTime );
    GetSystemTime(&SsystemTime);
    SystemTimeToFileTime(&SsystemTime,&systemTime);
                        
    //Store the FILETIMES in values we that can manipulate
    ULARGE_INTEGER iIdleTime,iSystemTime,iSystemTimeDiff,iIdleTimeDiff;
    iIdleTime.HighPart=idleTime.dwHighDateTime;
    iIdleTime.LowPart=idleTime.dwLowDateTime;
    iSystemTime.HighPart=systemTime.dwHighDateTime;
    iSystemTime.LowPart=systemTime.dwLowDateTime;                        
                        
    //Calculate CPU usage % by subtracting previous and current idle/system times and calculation percentage
    iIdleTimeDiff.QuadPart=(iIdleTime.QuadPart - m_PrevIdle.QuadPart);
    iSystemTimeDiff.QuadPart=(iSystemTime.QuadPart-m_PrevSystem.QuadPart);
    int cpuUsage = m_PrevSystem.QuadPart && iSystemTimeDiff.QuadPart ? (int)(100-iIdleTimeDiff.QuadPart*100/m_SystemInfo.dwNumberOfProcessors/iSystemTimeDiff.QuadPart) : 0;

    m_PrevIdle =  iIdleTime;
    m_PrevSystem = iSystemTime;

    return cpuUsage;


   
}
