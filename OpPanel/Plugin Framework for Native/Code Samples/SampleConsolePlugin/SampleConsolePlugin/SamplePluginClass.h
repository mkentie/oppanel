#pragma once
#include "CoreTempPlugin.h"

class SamplePluginClass
{
public:
	SamplePluginClass(void);
	virtual ~SamplePluginClass(void);

	int Start();
	void Update(const LPCoreTempSharedData data);
	void Stop();
	int Configure();
	void Remove(const wchar_t *path);

	LPCoreTempPlugin GetPluginInstance(HMODULE hModule);

protected:

	CoreTempPlugin m_CoreTempPlugin;
	void configurePlugin();
	void cls();
};

