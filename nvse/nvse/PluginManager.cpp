#include "PluginManager.h"
#include "CommandTable.h"
#include "common/IDirectoryIterator.h"
#include "ParamInfos.h"
#include "GameAPI.h"
#include "Utilities.h"

#ifdef RUNTIME
//#include "Serialization.h"
//#include "StringVar.h"
#include "Hooks_DirectInput8Create.h"
#include "GameForms.h"
#endif

PluginManager	g_pluginManager;

PluginManager::LoadedPlugin *	PluginManager::s_currentLoadingPlugin = NULL;
PluginHandle					PluginManager::s_currentPluginHandle = 0;

static const NVSEInterface g_NVSEInterface =
{
	PACKED_NVSE_VERSION,

#ifdef RUNTIME
	RUNTIME_VERSION,
	0,
	0,
#else
	0,
	EDITOR_VERSION,
	1,
#endif

	PluginManager::RegisterCommand,
	PluginManager::SetOpcodeBase,
	PluginManager::QueryInterface,
	PluginManager::GetPluginHandle
};

#ifdef RUNTIME
static const NVSEConsoleInterface g_NVSEConsoleInterface =
{
	NVSEConsoleInterface::kVersion,
	Script::RunScriptLine
};
#endif

static NVSEMessagingInterface g_NVSEMessagingInterface =
{
	NVSEMessagingInterface::kVersion,
	PluginManager::RegisterListener,
	PluginManager::Dispatch_Message
};

PluginManager::PluginManager()
{
	//
}

PluginManager::~PluginManager()
{
	DeInit();
}

bool PluginManager::Init(void)
{
	bool	result = false;

	if(FindPluginDirectory())
	{
		_MESSAGE("plugin directory = %s", m_pluginDirectory.c_str());

		__try
		{
			InstallPlugins();

			result = true;
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			// something very bad happened
			_ERROR("exception occurred while loading plugins");
		}
	}

	return result;
}

void PluginManager::DeInit(void)
{
	for(LoadedPluginList::iterator iter = m_plugins.begin(); iter != m_plugins.end(); ++iter)
	{
		LoadedPlugin	* plugin = &(*iter);

		if(plugin->handle)
		{
			FreeLibrary(plugin->handle);
		}
	}

	m_plugins.clear();
}

UInt32 PluginManager::GetNumPlugins(void)
{
	UInt32	numPlugins = m_plugins.size();

	// is one currently loading?
	if(s_currentLoadingPlugin) numPlugins++;

	return numPlugins;
}

UInt32 PluginManager::GetBaseOpcode(UInt32 idx)
{
	return m_plugins[idx].baseOpcode;
}

PluginHandle PluginManager::LookupHandleFromBaseOpcode(UInt32 baseOpcode)
{
	UInt32	idx = 1;

	for(LoadedPluginList::iterator iter = m_plugins.begin(); iter != m_plugins.end(); ++iter)
	{
		LoadedPlugin	* plugin = &(*iter);

		if(plugin->baseOpcode == baseOpcode)
			return idx;

		idx++;
	}

	return kPluginHandle_Invalid;
}

PluginInfo * PluginManager::GetInfoByName(const char * name)
{
	for(LoadedPluginList::iterator iter = m_plugins.begin(); iter != m_plugins.end(); ++iter)
	{
		LoadedPlugin	* plugin = &(*iter);

		if(plugin->info.name && !strcmp(name, plugin->info.name))
			return &plugin->info;
	}

	return NULL;
}

bool PluginManager::RegisterCommand(CommandInfo * _info)
{
	ASSERT(_info);
	ASSERT_STR(s_currentLoadingPlugin, "PluginManager::RegisterCommand: called outside of plugin load");

	CommandInfo	info = *_info;

#ifndef RUNTIME
	// modify callbacks for editor

	info.execute = Cmd_Default_Execute;
	info.eval = NULL;	// not supporting this yet
#endif

	if(!info.parse) info.parse = Cmd_Default_Parse;
	if(!info.shortName) info.shortName = "";
	if(!info.helpText) info.helpText = "";

	_MESSAGE("RegisterCommand %s (%04X)", info.longName, g_scriptCommands.GetCurID());

	g_scriptCommands.Add(&info);

	return true;
}

void PluginManager::SetOpcodeBase(UInt32 opcode)
{
	_MESSAGE("SetOpcodeBase %08X", opcode);

	ASSERT(opcode < 0x8000);	// arbitrary maximum for samity check
	ASSERT(opcode >= 0x2000);	// beginning of plugin opcode space
	ASSERT_STR(s_currentLoadingPlugin, "PluginManager::SetOpcodeBase: called outside of plugin load");

	if(opcode == 0x2000)
	{
		const char	* pluginName = "<unknown name>";

		if(s_currentLoadingPlugin && s_currentLoadingPlugin->info.name)
			pluginName = s_currentLoadingPlugin->info.name;

		_ERROR("You have a plugin installed that is using the default opcode base. (%s)", pluginName);
		_ERROR("This is acceptable for temporary development, but not for plugins released to the public.");
		_ERROR("As multiple plugins using the same opcode base create compatibility issues, plugins triggering this message may not load in future versions of NVSE.");
		_ERROR("Please contact the authors of the plugin and have them request and begin using an opcode range assigned by the NVSE team.");

#ifdef _DEBUG
		_ERROR("WARNING: serialization is being allowed for this plugin as this is a debug build of NVSE. It will not work in release builds.");
#endif
	}
#ifndef _DEBUG
	else	// disallow plugins using default opcode base from using it as a unique id
#endif
	{
		// record the first opcode registered for this plugin
		if(!s_currentLoadingPlugin->baseOpcode)
			s_currentLoadingPlugin->baseOpcode = opcode;
	}

	g_scriptCommands.PadTo(opcode);
	g_scriptCommands.SetCurID(opcode);
}

void * PluginManager::QueryInterface(UInt32 id)
{
	void	* result = NULL;
	switch(id)
	{
#ifdef RUNTIME
	case kInterface_Serialization:
//		result = (void *)&g_NVSESerializationInterface;
		break;

	case kInterface_Console:
		result = (void *)&g_NVSEConsoleInterface;
		break;

#endif
	case kInterface_Messaging:
		result = (void *)&g_NVSEMessagingInterface;
		break;
	default:
		_WARNING("unknown QueryInterface %08X", id);
		break;
	}

	return result;
}

PluginHandle PluginManager::GetPluginHandle(void)
{
	ASSERT_STR(s_currentPluginHandle, "A plugin has called NVSEInterface::GetPluginHandle outside of its Query/Load handlers");

	return s_currentPluginHandle;
}

bool PluginManager::FindPluginDirectory(void)
{
	bool	result = false;

	// find the path <fallout directory>/data/nvse/
	std::string	falloutDirectory = GetFalloutDirectory();

	if(!falloutDirectory.empty())
	{
		m_pluginDirectory = falloutDirectory + "Data\\NVSE\\Plugins\\";
		result = true;
	}

	return result;
}

void PluginManager::InstallPlugins(void)
{
	// avoid realloc
	m_plugins.reserve(5);

	for(IDirectoryIterator iter(m_pluginDirectory.c_str(), "*.dll"); !iter.Done(); iter.Next())
	{
		std::string	pluginPath = iter.GetFullPath();

		_MESSAGE("checking plugin %s", pluginPath.c_str());

		LoadedPlugin	plugin;
		memset(&plugin, 0, sizeof(plugin));

		s_currentLoadingPlugin = &plugin;
		s_currentPluginHandle = m_plugins.size() + 1;	// +1 because 0 is reserved for internal use

		plugin.handle = (HMODULE)LoadLibrary(pluginPath.c_str());
		if(plugin.handle)
		{
			bool		success = false;

			plugin.query = (_NVSEPlugin_Query)GetProcAddress(plugin.handle, "NVSEPlugin_Query");
			plugin.load = (_NVSEPlugin_Load)GetProcAddress(plugin.handle, "NVSEPlugin_Load");

			if(plugin.query && plugin.load)
			{
				const char	* loadStatus = NULL;

				loadStatus = SafeCallQueryPlugin(&plugin, &g_NVSEInterface);

				if(!loadStatus)
				{
					loadStatus = CheckPluginCompatibility(&plugin);

					if(!loadStatus)
					{
						loadStatus = SafeCallLoadPlugin(&plugin, &g_NVSEInterface);

						if(!loadStatus)
						{
							loadStatus = "loaded correctly";
							success = true;
						}
					}
				}
				else
				{
					loadStatus = "reported as incompatible during query";
				}

				ASSERT(loadStatus);

				_MESSAGE("plugin %s (%08X %s %08X) %s",
					pluginPath.c_str(),
					plugin.info.infoVersion,
					plugin.info.name ? plugin.info.name : "<NULL>",
					plugin.info.version,
					loadStatus);
			}
			else
			{
				_MESSAGE("plugin %s does not appear to be an NVSE plugin", pluginPath.c_str());
			}

			if(success)
			{
				// succeeded, add it to the list
				m_plugins.push_back(plugin);
			}
			else
			{
				// failed, unload the library
				FreeLibrary(plugin.handle);
			}
		}
		else
		{
			_ERROR("couldn't load plugin %s", pluginPath.c_str());
		}
	}

	s_currentLoadingPlugin = NULL;
	s_currentPluginHandle = 0;
}

// SEH-wrapped calls to plugin API functions to avoid bugs from bringing down the core
const char * PluginManager::SafeCallQueryPlugin(LoadedPlugin * plugin, const NVSEInterface * nvse)
{
	__try
	{
		if(!plugin->query(nvse, &plugin->info))
		{
			return "reported as incompatible during query";
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		// something very bad happened
		return "disabled, fatal error occurred while querying plugin";
	}

	return NULL;
}

const char * PluginManager::SafeCallLoadPlugin(LoadedPlugin * plugin, const NVSEInterface * nvse)
{
	__try
	{
		if(!plugin->load(nvse))
		{
			return "reported as incompatible during load";
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		// something very bad happened
		return "disabled, fatal error occurred while loading plugin";
	}

	return NULL;
}

enum
{
	kCompat_BlockFromRuntime =	1 << 0,
	kCompat_BlockFromEditor =	1 << 1,
};

struct MinVersionEntry
{
	const char	* name;
	UInt32		minVersion;
	const char	* reason;
	UInt32		compatFlags;
};

static const MinVersionEntry	kMinVersionList[] =
{
	{	NULL, 0, NULL }
};

// see if we have a plugin that we know causes problems
const char * PluginManager::CheckPluginCompatibility(LoadedPlugin * plugin)
{
	__try
	{
		// stupid plugin check
		if(!plugin->info.name)
		{
			return "disabled, no name specified";
		}

		// check for 'known bad' versions of plugins
		for(const MinVersionEntry * iter = kMinVersionList; iter->name; ++iter)
		{
			if(!strcmp(iter->name, plugin->info.name))
			{
				if(plugin->info.version < iter->minVersion)
				{
#ifdef RUNTIME
					if(iter->compatFlags & kCompat_BlockFromRuntime)
					{
						return iter->reason;
					}
#endif

#ifdef EDITOR
					if(iter->compatFlags & kCompat_BlockFromEditor)
					{
						return iter->reason;
					}
#endif
				}

				break;
			}
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		// paranoia
		return "disabled, fatal error occurred while checking plugin compatibility";
	}

	return NULL;
}

// Plugin communication interface
struct PluginListener {
	PluginHandle	listener;
	NVSEMessagingInterface::EventCallback	handleMessage;
};

typedef std::vector<std::vector<PluginListener> > PluginListeners;
static PluginListeners s_pluginListeners;

bool PluginManager::RegisterListener(PluginHandle listener, const char* sender, NVSEMessagingInterface::EventCallback handler)
{
	_MESSAGE("register plugin listener for %s", sender);
	// because this can be called while plugins are loading, gotta make sure number of plugins hasn't increased
	UInt32 numPlugins = g_pluginManager.GetNumPlugins() + 1;
	if (s_pluginListeners.size() < numPlugins)
	{
		s_pluginListeners.resize(numPlugins + 5);	// add some extra room to avoid unnecessary re-alloc
	}

	// handle > num plugins = invalid
	if (listener > g_pluginManager.GetNumPlugins() || !handler) 
	{
		return false;
	}

	if (sender)
	{
		// is target loaded?
		PluginHandle target = g_pluginManager.LookupHandleFromName(sender);
		if (target == kPluginHandle_Invalid)
		{
			return false;
		}
		// is listener already registered?
		for (std::vector<PluginListener>::iterator iter = s_pluginListeners[target].begin(); iter != s_pluginListeners[target].end(); ++iter)
		{
			if (iter->listener == listener)
			{
				return true;
			}
		}

		// register new listener
		PluginListener newListener;
		newListener.handleMessage = handler;
		newListener.listener = listener;

		s_pluginListeners[target].push_back(newListener);
	} else{
		// register listener to every loaded plugin
		UInt32 idx = 0;
		for(PluginListeners::iterator iter = s_pluginListeners.begin(); iter != s_pluginListeners.end(); ++iter)
		{
			// don't add the listener to its own list
			if (idx && idx != listener)
			{
				bool skipCurrentList = false;
				for (std::vector<PluginListener>::iterator iterEx = iter->begin(); iterEx != iter->end(); ++iterEx)
				{
					// already registered with this plugin, skip it
					if (iterEx->listener == listener)
					{
						skipCurrentList = true;
						break;
					}
				}
				if (skipCurrentList)
				{
					continue;
				}
				PluginListener newListener;
				newListener.handleMessage = handler;
				newListener.listener = listener;

				iter->push_back(newListener);
			}
			idx++;
		}
	}

	return true;
}

bool PluginManager::Dispatch_Message(PluginHandle sender, UInt32 messageType, void * data, UInt32 dataLen, const char* receiver)
{
	_MESSAGE("dispatch message to plugin listeners");
	UInt32 numRespondents = 0;
	PluginHandle target = kPluginHandle_Invalid;

	if (!s_pluginListeners.size())	// no listeners yet registered
	{
	    _MESSAGE("no listeners registered");
		return false;
	}
	else if (sender >= s_pluginListeners.size())
	{
	    _MESSAGE("sender is not in the list");
		return false;
	}

	if (receiver)
	{
		target = g_pluginManager.LookupHandleFromName(receiver);
		if (target == kPluginHandle_Invalid)
			return false;
	}

	const char* senderName = g_pluginManager.GetPluginNameFromHandle(sender);
	if (!senderName)
		return false;
	for (std::vector<PluginListener>::iterator iter = s_pluginListeners[sender].begin(); iter != s_pluginListeners[sender].end(); ++iter)
	{
		NVSEMessagingInterface::Message msg;
		msg.data = data;
		msg.type = messageType;
		msg.sender = senderName;
		msg.dataLen = dataLen;

		if (target != kPluginHandle_Invalid)	// sending message to specific plugin
		{
			if (iter->listener == target)
			{
				iter->handleMessage(&msg);
				return true;
			}
		} else {
			iter->handleMessage(&msg);
			numRespondents++;
		}
	}
	return numRespondents ? true : false;
}

PluginHandle PluginManager::LookupHandleFromName(const char* pluginName)
{
	if (!_stricmp("NVSE", pluginName))
		return 0;

	UInt32	idx = 1;
	for(LoadedPluginList::iterator iter = m_plugins.begin(); iter != m_plugins.end(); ++iter)
	{
		LoadedPlugin	* plugin = &(*iter);
		if(!_stricmp(plugin->info.name, pluginName))
		{
			return idx;
		}
		idx++;
	}
}

const char * PluginManager::GetPluginNameFromHandle(PluginHandle handle)
{
	if (handle > 0 && handle <= m_plugins.size())
		return (m_plugins[handle - 1].info.name);
	else if (handle == 0)
		return "NVSE";

	return NULL;
}


#ifdef RUNTIME

bool Cmd_IsPluginInstalled_Execute(COMMAND_ARGS)
{
	char	pluginName[256];

	*result = 0;

	if(!ExtractArgs(EXTRACT_ARGS, &pluginName)) return true;

	*result = (g_pluginManager.GetInfoByName(pluginName) != NULL) ? 1 : 0;

	return true;
}

bool Cmd_GetPluginVersion_Execute(COMMAND_ARGS)
{
	char	pluginName[256];

	*result = -1;

	if(!ExtractArgs(EXTRACT_ARGS, &pluginName)) return true;

	PluginInfo	* info = g_pluginManager.GetInfoByName(pluginName);

	if(info) *result = info->version;

	return true;
}

#endif

CommandInfo kCommandInfo_IsPluginInstalled =
{
	"IsPluginInstalled",
	"",
	0,
	"returns 1 if the specified plugin is installed, else 0",
	0,
	1,
	kParams_OneString,

	HANDLER(Cmd_IsPluginInstalled_Execute),
	Cmd_Default_Parse,
	NULL,
	NULL
};

CommandInfo kCommandInfo_GetPluginVersion =
{
	"GetPluginVersion",
	"",
	0,
	"returns the version of the specified plugin, or -1 if the plugin is not installed",
	0,
	1,
	kParams_OneString,

	HANDLER(Cmd_GetPluginVersion_Execute),
	Cmd_Default_Parse,
	NULL,
	NULL
};
