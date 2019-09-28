License:
--------
The material contained in this archive is free to use for non-comercial purposes.
For distribution and commercial applications please contact the author of the material:
Arthur_Liberman@hotmail.com
Arthur Liberman
Alcpu

Creating a native Core Temp plugin:
-----------------------------------
For C and C++, simply include the 'CoreTempPlugin.h' file into your project.
For other languages you will have to translate that file to your language, it is a definitions file.
Your code must implement and export, in C format, the functions 'GetPlugin' and 'ReleasePlugin'.

'GetPlugin' function:
---------------------
'GetPlugin' function is called right after the plugin dll file is loaded into memory.
The function must return a pointer to an allocated and initialized 'CoreTempPlugin' structure.
The member 'CoreTempPlugin.pluginInfo' should point to an allocated and initialized 'CoreTempPluginInfo' structure.
Both structures must always remain in memory until the plugin is released.

Core Temp passes the Module handle for the plugin dll to 'GetPlugin' when it's called.

'ReleasePlugin' function:
-------------------------
'ReleasePlugin' function is called right before the plugin dll is unloaded from memory.
The plugin should perform all of its clean-up in this function.
*Note: 'CoreTempPlugin.Stop' should not perform similar clean-up, as the plugin may still be restarted later.

'CoreTempPlugin' structure members:
-----------------------------------
'CoreTempPlugin.interfaceVersion' - Should be 1, this value may be used to indicate newer interface versions to future Core Temp versions.
'CoreTempPlugin.type' - General_Type (value = 1) is currently the only supported type.
'CoreTempPlugin.pluginInfo' - Must point to an allocated and initialized 'CoreTempPluginInfo' structure.
'CoreTempPlugin.Start' - A pointer to the function responsible for starting the plugin.
'CoreTempPlugin.Update' - A pointer to the function which receives updates from Core Temp with new data.
'CoreTempPlugin.Stop' - A pointer to the function responsible for stopping the plugin.
'CoreTempPlugin.Configure' - A pointer to the function which handles any plugin configuration.
'CoreTempPlugin.Remove' - A pointer to the function which receives a notification from Core Temp about the plugin to be immediately uninstalled, it should implement your uninstall clean-up code.

'CoreTempPluginInfo' structure members:
---------------------------------------
'CoreTempPluginInfo.version' - Should contain a Unicode string with the plugin version.
'CoreTempPluginInfo.name' - The plugin name. (unicode)
'CoreTempPluginInfo.description' - The plugin description. (unicode)
'CoreTempPluginInfo.dllInstance' - To be filled by Core Temp*. The Module handle for the plugin dll.
'CoreTempPluginInfo.hwndParent' - To be filled by Core Temp*. The handle to main Core Temp's window.
'CoreTempPluginInfo.remoteStopProc' - To be filled by Core Temp*. A callback function to notify Core Temp it needs to stop this plugin.
* This field will contain valid values only when the "Start" function in the plugin is called.

Installing your plugin:
-----------------------
1. Under the folder of the 'Core Temp.exe' file, create a folder called 'plugins'
2. Create a folder in 'plugins' for your plugin.