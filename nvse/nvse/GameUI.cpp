#include "nvse/GameUI.h"

UInt8	* g_MenuVisibilityArray = (UInt8 *)0x011F308F;
NiTArray <TileMenu *> * g_TileMenuArray = (NiTArray <TileMenu *> *)0x011F3508;

enum
{
	// ###

	kMenuType_Min = 0x3E9,
	kMenuType_Max = 0x43C,
};

InterfaceManager * InterfaceManager::GetSingleton(void)
{
	return *(InterfaceManager **)0x011D8A80;
}

bool InterfaceManager::IsMenuVisible(UInt32 menuType)
{
	if((menuType >= kMenuType_Min) && (menuType <= kMenuType_Max))
		return g_MenuVisibilityArray[menuType] != 0;

	return false;
}

Menu * InterfaceManager::GetMenuByType(UInt32 menuType)
{
	if((menuType >= kMenuType_Min) && (menuType <= kMenuType_Max))
	{
		TileMenu * tileMenu = g_TileMenuArray->Get(menuType - kMenuType_Min);
		if (tileMenu)
			return tileMenu->menu;
	}

	return NULL;
}

// Split component path into "top-level menu name" and "everything else".
// Path is of format "MenuType/tile/tile/..." following hierarchy defined in menu's xml.
// Returns pointer to top-level menu or NULL.
// pSlashPos is set to the slash character after the top-level menu name.
TileMenu* InterfaceManager::GetMenuByPath(const char * componentPath, const char ** pSlashPos)
{
	// get menu name - stored by game as "&MENUNAME;" so need to fix it up
	const char* slashPos = strpbrk(componentPath, "\\/");
	if(slashPos)
	{
		std::string menuName("&");
		menuName.append(componentPath, (slashPos - componentPath));
		menuName.append(";");

		UInt32 menuType = Tile::TraitNameToID(menuName.c_str());
		if((menuType >= kMenuType_Min) && (menuType <= kMenuType_Max))
		{
			TileMenu * tileMenu = g_TileMenuArray->Get(menuType - kMenuType_Min);
			if(tileMenu)
			{
				*pSlashPos = slashPos;
				return tileMenu;
			}
		}
	}

	return NULL;
}

Tile::Value* InterfaceManager::GetMenuComponentValue(const char * componentPath)
{
	// path is of format "MenuType/tile/tile/.../traitName" following hierarchy defined in menu's xml
	const char* slashPos = NULL;
	TileMenu * tileMenu = GetMenuByPath(componentPath, &slashPos);
	if(tileMenu && slashPos)
	{
		return tileMenu->GetComponentValue(slashPos + 1);
	}
	return NULL;
}

Tile* InterfaceManager::GetMenuComponentTile(const char * componentPath)
{
	// path is of format "MenuType/tile/tile/.../tile" following hierarchy defined in menu's xml
	const char* slashPos = NULL;
	TileMenu * tileMenu = GetMenuByPath(componentPath, &slashPos);
	if(tileMenu && slashPos)
	{
		return tileMenu->GetComponentTile(slashPos + 1);
	}
	return NULL;
}

void Debug_DumpMenus(void)
{
	for(UInt32 i = 0; i < g_TileMenuArray->length; i++)
	{
		TileMenu	* tileMenu = g_TileMenuArray->Get(i);

		if(tileMenu)
		{
			_MESSAGE("menu %d:");
			gLog.Indent();

			tileMenu->Dump();

			gLog.Outdent();
		}
	}
}

