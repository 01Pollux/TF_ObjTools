/**
 * vim: set ts=4 :
 * =============================================================================
 * Copyright (C) 2004-2008 AlliedModders LLC.  All rights reserved.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 * Version: $Id$
 */

#include "extension.h"
#include "dmginfo.h"
#include <toolframework/itoolentity.h>
#include <convar.h>
#include <vector>

class TF2ObjTools: public SDKExtension
{
public:
	bool SDK_OnLoad(char* error, size_t maxlength, bool late);
	void SDK_OnAllLoaded();
	bool QueryInterfaceDrop(SMInterface* pInterface);
	void SDK_OnUnload();
	bool SDK_OnMetamodLoad(ISmmAPI* ismm, char* error, size_t maxlen, bool late);

public:
} static _obj_tools;
SMEXT_LINK(&_obj_tools);

IGameConfig* gconfig;
ISDKTools* sdktools;
ISDKHooks* sdkhooks;
IServerTools* servertools;


bool TF2ObjTools::SDK_OnLoad(char* error, size_t maxlength, bool late)
{
	if (!gameconfs->LoadGameConfigFile("tf2.objtools", &gconfig, NULL, NULL))
	{
		::strncpy(error, "Failed to load \"tf2.objtools.txt\"", maxlength);
		return false;
	}
	sharesys->AddDependency(myself, "sdktools.ext", true, true);

	for (auto hook : IGlobalHooks::List())
		if (!hook->OnLoad(error, maxlength))
			return false;

	return true;
}

void TF2ObjTools::SDK_OnUnload()
{
	for (auto hook : IGlobalHooks::List())
		hook->OnUnload();

	gameconfs->CloseGameConfigFile(gconfig);
}

void TF2ObjTools::SDK_OnAllLoaded()
{
	SM_GET_LATE_IFACE(SDKTOOLS, sdktools);
	SM_GET_LATE_IFACE(SDKHOOKS, sdkhooks);
	extern _CTakeDmgInfo take_dmg_info;
	sdkhooks->AddEntityListener(&take_dmg_info);
}

bool TF2ObjTools::QueryInterfaceDrop(SMInterface* pInterface)
{
	if (pInterface == sdktools)
		return false;
	else if (pInterface == sdkhooks)
		return false;

	return IExtensionInterface::QueryInterfaceDrop(pInterface);
}

bool TF2ObjTools::SDK_OnMetamodLoad(ISmmAPI* ismm, char* error, size_t maxlen, bool late)
{
	GET_V_IFACE_CURRENT(GetServerFactory, servertools, IServerTools, VSERVERTOOLS_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetEngineFactory, g_pCVar, ICvar, CVAR_INTERFACE_VERSION);
	return true;
}
