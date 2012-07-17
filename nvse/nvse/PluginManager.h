#pragma once

#include <string>
#include <vector>

#include "nvse/PluginAPI.h"

class PluginManager
{
public:
	PluginManager();
	~PluginManager();

	bool	Init(void);
	void	DeInit(void);

	PluginInfo *	GetInfoByName(const char * name);

	UInt32			GetNumPlugins(void);
	UInt32			GetBaseOpcode(UInt32 idx);
	PluginHandle	LookupHandleFromBaseOpcode(UInt32 baseOpcode);

	static bool			RegisterCommand(CommandInfo * _info);
	static void			SetOpcodeBase(UInt32 opcode);
	static void *		QueryInterface(UInt32 id);
	static PluginHandle	GetPluginHandle(void);

private:
	struct LoadedPlugin
	{
		HMODULE		handle;
		PluginInfo	info;
		UInt32		baseOpcode;

		_NVSEPlugin_Query	query;
		_NVSEPlugin_Load	load;
	};

	bool	FindPluginDirectory(void);
	void	InstallPlugins(void);

	const char *	SafeCallQueryPlugin(LoadedPlugin * plugin, const NVSEInterface * nvse);
	const char *	SafeCallLoadPlugin(LoadedPlugin * plugin, const NVSEInterface * nvse);

	const char *	CheckPluginCompatibility(LoadedPlugin * plugin);

	typedef std::vector <LoadedPlugin>	LoadedPluginList;

	std::string			m_pluginDirectory;
	LoadedPluginList	m_plugins;

	static LoadedPlugin		* s_currentLoadingPlugin;
	static PluginHandle		s_currentPluginHandle;
};

extern PluginManager	g_pluginManager;

extern CommandInfo kCommandInfo_IsPluginInstalled;
extern CommandInfo kCommandInfo_GetPluginVersion;
