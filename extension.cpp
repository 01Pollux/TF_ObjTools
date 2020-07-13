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
#include "serverutils.h"
#include <convar.h>

SH_DECL_MANUALHOOK1(OnTakeDamage, 0, 0, 0, int, CTakeDmgInfoBuilder&)
SH_DECL_MANUALHOOK1(OnTakeDamageAlive, 0, 0, 0, int, CTakeDmgInfoBuilder&)
static int _OnTakeDamage(CTakeDmgInfoBuilder& infos);
static int _OnTakeDamageAlive(CTakeDmgInfoBuilder& infos);
static int _OnTakeDamagePost(CTakeDmgInfoBuilder& infos);
static int _OnTakeDamageAlivePost(CTakeDmgInfoBuilder& infos);

/**
  * @file extension.cpp
  * @brief Implement extension code here.
  */

TF2ObjTools g_TF2ObjTools;		/**< Global singleton for extension's main interface */
SMEXT_LINK(&g_TF2ObjTools);

IGameConfig* pConfig = nullptr;
ISDKTools* sdktools = nullptr;
ISDKHooks* sdkhooks = nullptr;
IServerTools* servertools = nullptr;
ConVar* phys_pushscale;
ke::Vector<IHookP*> IHookedEnt[MaxHooks];

HandleType_t g_TakeDmgInfo;
CTakeDmgInfoHandler g_CTakeDmgInfoHandler;

bool TF2ObjTools::SDK_OnLoad(char* error, size_t maxlength, bool late)
{
	if (!gameconfs->LoadGameConfigFile("tf2.objtools", &pConfig, NULL, NULL))
	{
		ke::SafeStrcpy(error, maxlength, "Failed to load \"tf2.objtools.txt\"");
		return false;
	}

	{
		int offset = 0;
		if (!pConfig->GetOffset("OnTakeDamage", &offset) || !offset)
		{
			ke::SafeStrcpy(error, maxlength, "Failed to find offset for \"OnTakeDamage\"");
			return false;
		}
		SH_MANUALHOOK_RECONFIGURE(OnTakeDamage, offset, 0, 0);
		if (!pConfig->GetOffset("OnTakeDamageAlive", &offset) || !offset)
		{
			ke::SafeStrcpy(error, maxlength, "Failed to find offset for \"OnTakeDamageAlive\"");
			return false;
		}
		SH_MANUALHOOK_RECONFIGURE(OnTakeDamageAlive, offset, 0, 0);
	}

	sharesys->AddDependency(myself, "sdktools.ext", true, true);

	sharesys->AddNatives(myself, g_InfoNatives);
	sharesys->AddNatives(myself, g_AmmoNatives);
	sharesys->AddNatives(myself, g_ServerNatives);

	plsys->AddPluginsListener(this);

	g_TakeDmgInfo = handlesys->CreateType("CTakeDamageInfo", &g_CTakeDmgInfoHandler, 0, NULL, NULL, myself->GetIdentity(), NULL);
	return true;
}

void TF2ObjTools::SDK_OnUnload()
{
	for (uint8 type = 0; type < MaxHooks; type++)
	{
		auto& hooks = IHookedEnt[type];
		for (size_t i = 0; i < hooks.length(); i++)
		{
			delete hooks[i];
			hooks.remove(i--);
		}
	}

	gameconfs->CloseGameConfigFile(pConfig);
	handlesys->RemoveType(g_TakeDmgInfo, myself->GetIdentity());
	plsys->RemovePluginsListener(this);
	sdkhooks->RemoveEntityListener(this);
}

void TF2ObjTools::SDK_OnAllLoaded()
{
	SM_GET_LATE_IFACE(SDKTOOLS, sdktools);
	SM_GET_LATE_IFACE(SDKHOOKS, sdkhooks);
	sdkhooks->AddEntityListener(this);
	phys_pushscale = g_pCVar->FindVar("phys_pushscale");
}

bool TF2ObjTools::QueryInterfaceDrop(SMInterface* pInterface)
{
	if (pInterface == sdktools)
	{
		return false;
	}
	else if (pInterface == sdkhooks)
	{
		return false;
	}
	return IExtensionInterface::QueryInterfaceDrop(pInterface);
}

bool TF2ObjTools::SDK_OnMetamodLoad(ISmmAPI* ismm, char* error, size_t maxlen, bool late)
{
	GET_V_IFACE_CURRENT(GetServerFactory, servertools, IServerTools, VSERVERTOOLS_INTERFACE_VERSION);
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

void TF2ObjTools::OnPluginUnloaded(IPlugin* pPlugin)
{
	IPluginContext* pContext = pPlugin->GetBaseContext();
	for (uint8 type = 0; type < MaxHooks; type++)
	{
		auto& hooks = IHookedEnt[type];
		for (size_t i = 0; i < hooks.length(); i++)
		{
			auto& callbacks = hooks[i]->pCallbacks;
			for (size_t cb = 0; cb < callbacks.length(); cb++)
			{
				if (callbacks[cb]->GetParentContext() == pContext)
				{
					callbacks.remove(cb--);
				}
			}
			if (!callbacks.length())
			{
				delete hooks[i];
				hooks.remove(i--);
			}
		}
	}
}

void TF2ObjTools::OnEntityDestroyed(CBaseEntity* pEntity)
{
	int ref = gamehelpers->EntityToReference(pEntity);
	for (uint8 type = 0; type < MaxHooks; type++)
	{
		auto& hooks = IHookedEnt[type];
		for (size_t i = 0; i < hooks.length(); i++)
		{
			if (hooks[i]->ref != ref)
			{
				continue;
			}

			delete hooks[i];
			hooks.remove(i--);
		}
	}
}


void TF2ObjTools::HookEnt(int entity, IPluginFunction* pCallback, HookType type)
{
	int ref = gamehelpers->IndexToReference(entity);
	auto& hooks = IHookedEnt[type];

	CBaseEntity* pEntity = gamehelpers->ReferenceToEntity(ref);

	size_t i;
	for (i = 0; i < hooks.length(); i++)
	{
		if (ref == hooks[i]->ref)
		{
			break;
		}
	}

	if (i == hooks.length())
	{
		int hookid;
		switch (type)
		{
		case GenericPre:
			hookid = SH_ADD_MANUALVPHOOK(OnTakeDamage, pEntity, SH_STATIC(&_OnTakeDamage), false);
			break;
		case AlivePre:
			hookid = SH_ADD_MANUALVPHOOK(OnTakeDamageAlive, pEntity, SH_STATIC(&_OnTakeDamageAlive), false);
			break;
		case GenericPost:
			hookid = SH_ADD_MANUALVPHOOK(OnTakeDamage, pEntity, SH_STATIC(&_OnTakeDamagePost), true);
			break;
		case AlivePost:
			hookid = SH_ADD_MANUALVPHOOK(OnTakeDamageAlive, pEntity, SH_STATIC(&_OnTakeDamageAlivePost), true);
			break;
		}
		
		auto hook = new IHookP(hookid, ref);
		hooks.append(hook);
	}

	auto& callbacks = hooks[i]->pCallbacks;
	for (size_t cb = 0; cb < callbacks.length(); cb++)
	{
		if (callbacks[cb] == pCallback)
		{
			return;
		}
	}
	callbacks.append(pCallback);
}

void TF2ObjTools::UnHookEnt(int entity, IPluginFunction* pCallback, HookType type)
{
	int ref = gamehelpers->IndexToReference(entity);
	auto& hooks = IHookedEnt[type];
	for (size_t i = 0; i < hooks.length(); i++)
	{
		if (ref != hooks[i]->ref)
		{
			continue;
		}
		auto& callbacks = hooks[i]->pCallbacks;
		for (size_t cb = 0; cb < callbacks.length(); cb++)
		{
			if (pCallback == callbacks[cb])
			{
				callbacks.remove(cb);
				break;
			}
		}
		if (!callbacks.length())
		{
			delete hooks[i];
			hooks.remove(i);
		}
		break;
	}
}


static int HandleTakeDmgInfo(CTakeDmgInfoBuilder* infos, HookType type)
{
	CBaseEntity* pVictim = META_IFACEPTR(CBaseEntity);
	int ref = gamehelpers->EntityToReference(pVictim);

	auto& hooks = IHookedEnt[type];

	for (size_t i = 0; i < hooks.length(); i++)
	{
		if (ref != hooks[i]->ref)
		{
			continue;
		}

		auto& callbacks = hooks[i]->pCallbacks;
		if (!callbacks.length())
		{
			delete hooks[i];
			hooks.remove(i);
			RETURN_META_VALUE(MRES_IGNORED, NULL);
		}

		static IdentityToken_t* token;
		token = myself->GetIdentity();
		Handle_t hndl = handlesys->CreateHandle(g_TakeDmgInfo, infos, token, token, NULL);;

		int client = gamehelpers->ReferenceToBCompatRef(ref);

		cell_t action = Pl_Continue;
		cell_t final = Pl_Continue;
		for (size_t cb = 0; cb < callbacks.length(); cb++)
		{
			callbacks[cb]->PushCell(client);
			callbacks[cb]->PushCellByRef((cell_t*)&hndl);
			callbacks[cb]->Execute(&action);

			if (action > final)
			{
				final = action;
				if (final == Pl_Changed)
				{
					auto newinfo = ReadDamageInfoFromHandle(callbacks[cb]->GetParentContext(), hndl);
					if (!newinfo)
					{
						handlesys->FreeHandle(hndl, NULL);
						RETURN_META_VALUE(MRES_IGNORED, NULL);
					}
					
					CBaseHandle& basehndl = newinfo->m_hAttacker;
					if (!basehndl.IsValid())
					{
						handlesys->FreeHandle(hndl, NULL);
						callbacks[cb]->GetParentContext()->BlamePluginError(callbacks[cb], "Invalid Attacker index: %d", basehndl.GetEntryIndex());
						RETURN_META_VALUE(MRES_IGNORED, NULL);
					}

					basehndl = newinfo->m_hAttacker;
					if (!basehndl.IsValid())
					{
						handlesys->FreeHandle(hndl, NULL);
						callbacks[cb]->GetParentContext()->BlamePluginError(callbacks[cb], "Invalid Inflictor index: %d", basehndl.GetEntryIndex());
						RETURN_META_VALUE(MRES_IGNORED, NULL);
					}

					memcpy(infos, newinfo, sizeof(CTakeDmgInfoBuilder));
				}
			}
		}

		handlesys->FreeHandle(hndl, NULL);
		if (final >= Pl_Handled)
		{
			RETURN_META_VALUE(MRES_SUPERCEDE, 1);
		}
		else if (final == Pl_Changed)
		{
			RETURN_META_VALUE(MRES_HANDLED, 1);
		}

		break;
	}
	RETURN_META_VALUE(MRES_IGNORED, NULL);
}

static int HandleTakeDmgInfoPost(CTakeDmgInfoBuilder* infos, HookType type)
{
	CBaseEntity* pVictim = META_IFACEPTR(CBaseEntity);
	int ref = gamehelpers->EntityToReference(pVictim);

	auto& hook = IHookedEnt[type];
	for (size_t i = 0; i < hook.length(); i++)
	{
		if (ref != hook[i]->ref)
		{
			continue;
		}

		auto& callbacks = hook[i]->pCallbacks;
		if (!callbacks.length())
		{
			delete hook[i];
			hook.remove(i--);
			RETURN_META_VALUE(MRES_IGNORED, NULL);
		}

		static auto token = myself->GetIdentity();
		Handle_t hndl = handlesys->CreateHandle(g_TakeDmgInfo, (void*)infos, token, token, NULL);
		int client = gamehelpers->ReferenceToBCompatRef(ref);

		for (size_t cb = 0; cb < callbacks.length(); cb++)
		{
			callbacks[cb]->PushCell(client);
			callbacks[cb]->PushCell(hndl);
			callbacks[cb]->Execute(NULL);
		}

		handlesys->FreeHandle(hndl, NULL);
		break;
	}
	RETURN_META_VALUE(MRES_IGNORED, NULL);
}

int _OnTakeDamage(CTakeDmgInfoBuilder& infos)
{
	return HandleTakeDmgInfo(&infos, GenericPre);
}

int _OnTakeDamageAlive(CTakeDmgInfoBuilder& infos)
{
	return HandleTakeDmgInfo(&infos, AlivePre);
}

int _OnTakeDamagePost(CTakeDmgInfoBuilder& infos)
{
	return HandleTakeDmgInfoPost(&infos, GenericPost);
}

int _OnTakeDamageAlivePost(CTakeDmgInfoBuilder& infos)
{
	return HandleTakeDmgInfoPost(&infos, AlivePost);
}


CAmmoDef* GetAmmoDef()
{
	static CAmmoDef* (*pAmmoDef)() = nullptr;
	if (!pAmmoDef)
	{
		void* addr = nullptr;
		if (!pConfig->GetMemSig("AmmoDef", &addr) || !addr)
		{
			AssertFatalMsg(0, "Failed to find \"AmmoDef\" sig");
		}
		pAmmoDef = reinterpret_cast<CAmmoDef* (*)()>(addr);
	}
	return (*pAmmoDef)();
}
