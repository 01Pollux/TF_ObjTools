/**
 * vim: set ts=4 :
 * =============================================================================
 * TF2 CTakeDamageInfo Object
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
#include "ammodefs.h"
#include <convar.h>

 /**
  * @file extension.cpp
  * @brief Implement extension code here.
  */

TF2ObjTools g_TF2ObjTools;		/**< Global singleton for extension's main interface */
SMEXT_LINK(&g_TF2ObjTools);

IGameConfig* pConfig = nullptr;
ISDKTools* sdktools = nullptr;
IBinTools* bintools = nullptr;

ICallWrapper* pAmmoDef;
HandleType_t g_TakeDmgInfo;
CTakeDmgInfoHandler g_CTakeDmgInfoHandler;

bool TF2ObjTools::SDK_OnLoad(char* error, size_t maxlength, bool late)
{
	if (!gameconfs->LoadGameConfigFile("tf2.objtools", &pConfig, NULL, NULL))
	{
		ke::SafeStrcpy(error, maxlength, "Failed to load \"tf2.takedmginfo.txt\"");
		return false;
	}
	sharesys->AddDependency(myself, "sdktools.ext", true, true);
	sharesys->AddDependency(myself, "bintools.ext", true, true);

	sharesys->AddNatives(myself, g_InfoNatives);
	sharesys->AddNatives(myself, g_AmmoNatives);

	g_TakeDmgInfo = handlesys->CreateType("CTakeDamageInfo", &g_CTakeDmgInfoHandler, 0, NULL, NULL, myself->GetIdentity(), NULL);
	return true;
}

void TF2ObjTools::SDK_OnUnload()
{
	gameconfs->CloseGameConfigFile(pConfig);
	handlesys->RemoveType(g_TakeDmgInfo, myself->GetIdentity());
	if (pAmmoDef)
	{
		pAmmoDef->Destroy();
		pAmmoDef = nullptr;
	}
}

void TF2ObjTools::SDK_OnAllLoaded()
{
	SM_GET_LATE_IFACE(SDKTOOLS, sdktools);
	SM_GET_LATE_IFACE(BINTOOLS, bintools);
}

bool TF2ObjTools::QueryInterfaceDrop(SMInterface* pInterface)
{
	if (pInterface == sdktools)
	{
		return false;
	}
	if (pInterface == bintools)
	{
		return false;
	}
	return IExtensionInterface::QueryInterfaceDrop(pInterface);
}

bool TF2ObjTools::SDK_OnMetamodLoad(ISmmAPI* ismm, char* error, size_t maxlen, bool late)
{
	GET_V_IFACE_CURRENT(GetEngineFactory, g_pCVar, ICvar, CVAR_INTERFACE_VERSION);
	ConVar_Register(0, this);
	return true;
}

bool TF2ObjTools::SDK_OnMetamodUnload(char* error, size_t maxlen)
{
	ConVar_Unregister();
	return true;
}

bool TF2ObjTools::RegisterConCommandBase(ConCommandBase* pVar)
{
	return META_REGCVAR(pVar);
}

CAmmoDef* GetAmmoDef()
{
	if (!pAmmoDef)
	{
		void* addr = nullptr;
		pConfig->GetMemSig("AmmoDef", &addr);
		AssertFatalMsg(addr, "Failed to find \"AmmoDef\" sig");
		PassInfo retInfo(PassType_Basic, PASSFLAG_BYVAL, sizeof(void*), nullptr, 0);
		pAmmoDef = bintools->CreateCall(addr, CallConv_Cdecl, &retInfo, NULL, 0);
	}
	CAmmoDef* ammodef;
	pAmmoDef->Execute(NULL, &ammodef);
	return ammodef;
}
