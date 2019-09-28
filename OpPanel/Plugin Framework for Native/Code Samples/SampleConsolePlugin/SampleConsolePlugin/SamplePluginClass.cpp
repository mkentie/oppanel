#include <iostream>
#include <Windows.h>
#include "SamplePluginClass.h"

using namespace std;

SamplePluginClass::SamplePluginClass(void)
{
	cout << "Hello World from SamplePluginClass\n";
	this->configurePlugin();
}

SamplePluginClass::~SamplePluginClass(void)
{
	cout << "Bye bye from SamplePluginClass\n";
	delete m_CoreTempPlugin.pluginInfo;
}

int SamplePluginClass::Start()
{
	cout << "The start function has been called\n";

	// Return 0 for success, non-zero for failure.
	return 0;
}

void SamplePluginClass::Update(const LPCoreTempSharedData data)
{
	if (data != NULL)
	{
		int index;

		//Clear screen.
		this->cls();
		char tempType = data->ucFahrenheit ? 'F' : 'C';

		//Now print the output.
		printf("CPU Name: %s\n", data->sCPUName);
		printf("CPU Speed: %.2fMHz (%.2f x %.2f)\n", 
			data->fCPUSpeed, data->fFSBSpeed, data->fMultiplier);
		printf("CPU VID: %.4fv\n", data->fVID);
		printf("Physical CPUs: %d\n", data->uiCPUCnt);
		printf("Cores per CPU: %d\n", data->uiCoreCnt);
		for (UINT i = 0; i < data->uiCPUCnt; i++)
		{
			printf("CPU #%d\n", i);
			printf("Tj.Max: %d%c\n", data->uiTjMax[i], tempType);
			for (UINT g = 0; g < data->uiCoreCnt; g++)
			{
				index = g + (i * data->uiCoreCnt);
				if (data->ucDeltaToTjMax)
				{
					printf("Core #%d: %.2f%c to TjMax, %d%% Load\n", 
						index, data->fTemp[index], tempType, data->uiLoad[index]);
				}
				else
				{
					printf("Core #%d: %.2f%c, %d%% Load\n",
						index, data->fTemp[index], tempType, data->uiLoad[index]);
				}
			}
		}
	}
}

void SamplePluginClass::Stop()
{
	cout << "The plugin has stopped\n";
}

int SamplePluginClass::Configure()
{
	// Return 0 for not-implemented, non-zero for "Handled".
	return 0;
}

void SamplePluginClass::Remove(const wchar_t *path)
{
	cout << "Cleanup should be performed at " << path << "\n";
}

LPCoreTempPlugin SamplePluginClass::GetPluginInstance(HMODULE hModule)
{
	return &this->m_CoreTempPlugin;
}

void SamplePluginClass::configurePlugin()
{
	LPCoreTempPluginInfo pluginInfo = new CoreTempPluginInfo;
	
	m_CoreTempPlugin.pluginInfo = pluginInfo;
	pluginInfo->name = L"Sample C++ Native Plugin";
	pluginInfo->description = L"A sample plugin, showing one way of creating a C++ plugin for Core Temp";
	pluginInfo->version = L"1.0";
	
	// Interface version should be 1 for current plugin API.
	m_CoreTempPlugin.interfaceVersion = 1;
	m_CoreTempPlugin.type = General_Type;
}

void SamplePluginClass::cls()
{
	HANDLE	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD	coordScreen = { 0, 0 };		/* here's where we'll home the cursor */
	BOOL	bSuccess;
	DWORD	cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO	csbi;	/* to get buffer info */
	DWORD	dwConSize;				/* number of character cells in the current buffer */
	/* get the number of character cells in the current buffer */
	bSuccess = GetConsoleScreenBufferInfo(hConsole, &csbi);
	if (!bSuccess) return;
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
	/* fill the entire screen with blanks */
	bSuccess = FillConsoleOutputCharacter(hConsole, (TCHAR) ' ', dwConSize, coordScreen, &cCharsWritten);
	if (!bSuccess) return;
	/* get the current text attribute */ 
	bSuccess = GetConsoleScreenBufferInfo(hConsole, &csbi);
	if (!bSuccess) return;
	/* now set the buffer's attributes accordingly */ 
	bSuccess = FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten); 
	if (!bSuccess) return;
	/* put the cursor at (0, 0) */   
	bSuccess = SetConsoleCursorPosition(hConsole, coordScreen); 
	if (!bSuccess) return;
	return;
}