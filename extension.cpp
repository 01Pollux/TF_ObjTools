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

 /**
  * @file extension.cpp
  * @brief Implement extension code here.
  */

CTakeDmgExt g_CTakeDmgExt;		/**< Global singleton for extension's main interface */
SMEXT_LINK(&g_CTakeDmgExt);

HandleType_t g_TakeDmgInfo;
CTakeDmgInfoHandler g_CTakeDmgInfoHandler;
ISDKTools* sdktools;

SH_DECL_MANUALHOOK3_void(DeathNotice, 0, 0, 0, CBaseEntity*, CTakeDmgInfoBuilder&, const char*)

const sp_nativeinfo_t g_InfoNatives[] = {
	{"CTakeDamageInfo.CTakeDamageInfo", sm_CTakeDamageInfo},
	{"CTakeDamageInfo.ReadInt", sm_SetDamageInfo_GetInt},
	{"CTakeDamageInfo.StoreInt", sm_SetDamageInfo_SetInt},
	{"CTakeDamageInfo.ReadFloat", sm_SetDamageInfo_GetFloat},
	{"CTakeDamageInfo.StoreFloat", sm_SetDamageInfo_SetFloat},
	{"CTakeDamageInfo.ReadVector", sm_SetDamageInfo_GetVector},
	{"CTakeDamageInfo.StoreVector", sm_SetDamageInfo_SetVector},
	{"CTakeDamageInfo.ReadEnt", sm_SetDamageInfo_GetEnt},
	{"CTakeDamageInfo.StoreEnt", sm_SetDamageInfo_SetEnt},
	{"CTakeDamageInfo.Infos.get", sm_GetDamageInfo_Data},
	{"CTakeDamageInfo.Fire", sm_CTakeDamageInfo_Fire},
	{NULL, NULL},
};

cell_t sm_CTakeDamageInfo(IPluginContext* pContext, const cell_t* Params)
{
	CTakeDmgInfoBuilder* info = new CTakeDmgInfoBuilder;
	memset(info, 0, sizeof(CTakeDmgInfoBuilder));

	HandleError err;
	Handle_t hndl = handlesys->CreateHandle(g_TakeDmgInfo, info, pContext->GetIdentity(), myself->GetIdentity(), &err);
	if (!hndl) {
		return pContext->ThrowNativeError("Invalid CTakeDamageInfo Handle %x (error %d)", hndl, err);
	}
	return hndl;
}

cell_t sm_GetDamageInfo_Data(IPluginContext* pContext, const cell_t* Params)
{
	CTakeDmgInfoBuilder* infos = ReadDamageInfoFromHandle(pContext, Params[1]);
	if (!infos) {
		return NULL;
	}

#ifdef PLATFORM_X86
	return reinterpret_cast<cell_t>(infos);
#else
	return smutils->ToPseudoAddress(infos);
#endif
}

cell_t sm_CTakeDamageInfo_Fire(IPluginContext* pContext, const cell_t* Params)
{
	CTakeDmgInfoBuilder* infos = ReadDamageInfoFromHandle(pContext, Params[1]);
	if (!infos) {
		return pContext->ThrowNativeError("Invalid CTakeDamageInfo Handle %x", infos);
	}

	CBaseEntity* pVictim = gamehelpers->ReferenceToEntity(Params[2]);
	if (!pVictim) {
		return pContext->ThrowNativeError("Invalid Entity index : %i", Params[2]);
	}

	void* gamerules = sdktools->GetGameRules();
	if (!gamerules) {
		return pContext->ThrowNativeError("Failed to find GameRules object");
	}

	SH_MCALL(gamerules, DeathNotice)(pVictim, *infos, "player_death");
	return 1;
}

cell_t sm_SetDamageInfo_GetInt(IPluginContext* pContext, const cell_t* Params)
{
	CTakeDmgInfoBuilder* info = ReadDamageInfoFromHandle(pContext, Params[1]);
	if (!info) {
		return 0;
	}

	TakeDmgOffset offset = static_cast<TakeDmgOffset>(Params[2]);
	switch (offset) {
	case bitsDamageType:
		return info->m_bitsDamageType;
	case iDamageCustom:
		return info->m_iDamageCustom;
	case iDamageStats:
		return info->m_iDamageStats;
	case iAmmoType:
		return info->m_iAmmoType;
	case iDamagedOtherPlayers:
		return info->m_iDamagedOtherPlayers;
	case iPlayerPenetrationCount:
		return info->m_iPlayerPenetrationCount;
	case bForceFriendlyFire:
		return info->m_bForceFriendlyFire ? 1 : 0;
	case eCritType:
		return (cell_t)info->m_eCritType;

	default:
		return pContext->ThrowNativeError("Invalid data type %i for CTakeDamageInfo_Int", Params[2]);
	}
	return 0;
}

cell_t sm_SetDamageInfo_SetInt(IPluginContext* pContext, const cell_t* Params)
{
	CTakeDmgInfoBuilder* info = ReadDamageInfoFromHandle(pContext, Params[1]);
	if (!info) {
		return 0;
	}

	TakeDmgOffset offset = static_cast<TakeDmgOffset>(Params[2]);
	switch (offset) {
	case bitsDamageType:
		info->m_bitsDamageType = Params[3];
		return 1;
	case iDamageCustom:
		info->m_iDamageCustom = Params[3];
		return 1;
	case iDamageStats:
		info->m_iDamageStats = Params[3];
		return 1;
	case iAmmoType:
		info->m_iAmmoType = Params[3];
		return 1;
	case iDamagedOtherPlayers:
		info->m_iDamagedOtherPlayers = Params[3];
		return 1;
	case iPlayerPenetrationCount:
		info->m_iPlayerPenetrationCount = Params[3];
		return 1;
	case bForceFriendlyFire:
		info->m_bForceFriendlyFire = Params[3] != 0 ? true : false;
		return 1;
	case eCritType:
		info->m_eCritType = (CritType)Params[3];
		return 1;

	default:
		return pContext->ThrowNativeError("Invalid data type %i for CTakeDamageInfo_Int", Params[2]);
	}
	return 0;
}

cell_t sm_SetDamageInfo_GetFloat(IPluginContext* pContext, const cell_t* Params)
{
	CTakeDmgInfoBuilder* info = ReadDamageInfoFromHandle(pContext, Params[1]);
	if (!info) {
		return 0.0;
	}
	TakeDmgOffset offset = static_cast<TakeDmgOffset>(Params[2]);
	switch (offset) {
	case flDamage:
		return sp_ftoc(info->m_flDamage);
	case flMaxDamage:
		return sp_ftoc(info->m_flMaxDamage);
	case flBaseDamage:
		return sp_ftoc(info->m_flBaseDamage);
	case flDamageBonus:
		return sp_ftoc(info->m_flDamageBonus);
	case flDamageForForce:
		return sp_ftoc(info->m_flDamageForForce);

	default:
		return pContext->ThrowNativeError("Invalid data type %i for CTakeDamageInfo_Float", Params[2]);
	}
	return 0.0;
}

cell_t sm_SetDamageInfo_SetFloat(IPluginContext* pContext, const cell_t* Params)
{
	CTakeDmgInfoBuilder* info = ReadDamageInfoFromHandle(pContext, Params[1]);
	if (!info) {
		return 0;
	}

	TakeDmgOffset offset = static_cast<TakeDmgOffset>(Params[2]);
	switch (offset) {
	case flDamage:
		info->m_flDamage = sp_ctof(Params[3]);
		return 1;
	case flMaxDamage:
		info->m_flMaxDamage = sp_ctof(Params[3]);
		return 1;
	case flBaseDamage:
		info->m_flBaseDamage = sp_ctof(Params[3]);
		return 1;
	case flDamageBonus:
		info->m_flDamageBonus = sp_ctof(Params[3]);
		return 1;
	case flDamageForForce:
		info->m_flDamageForForce = sp_ctof(Params[3]);
		return 1;

	default:
		return pContext->ThrowNativeError("Invalid data type %i for CTakeDamageInfo_Float", Params[2]);
	}
	return 0;
}

cell_t sm_SetDamageInfo_GetVector(IPluginContext* pContext, const cell_t* Params)
{
	CTakeDmgInfoBuilder* info = ReadDamageInfoFromHandle(pContext, Params[1]);
	if (!info) {
		return 0;
	}

	cell_t* vec;
	pContext->LocalToPhysAddr(Params[3], &vec);

	TakeDmgOffset offset = static_cast<TakeDmgOffset>(Params[2]);
	switch (offset) {
	case vecDamageForce: {
		Vector force = info->m_vecDamageForce;
		vec[0] = sp_ftoc(force.x);
		vec[1] = sp_ftoc(force.y);
		vec[2] = sp_ftoc(force.z);
		return 1;
	}

	case vecDamagePosition: {
		Vector dmg = info->m_vecDamagePosition;
		vec[0] = sp_ftoc(dmg.x);
		vec[1] = sp_ftoc(dmg.y);
		vec[2] = sp_ftoc(dmg.z);
		return 1;
	}

	case vecReportedPosition: {
		Vector reported = info->m_vecReportedPosition;
		vec[0] = sp_ftoc(reported.x);
		vec[1] = sp_ftoc(reported.y);
		vec[2] = sp_ftoc(reported.z);
		return 1;
	}

	default:
		return pContext->ThrowNativeError("Invalid data type %i for CTakeDamageInfo_Vector", Params[2]);
	}
	return 0;
}

cell_t sm_SetDamageInfo_SetVector(IPluginContext* pContext, const cell_t* Params)
{
	CTakeDmgInfoBuilder* info = ReadDamageInfoFromHandle(pContext, Params[1]);
	if (!info) {
		return 0;
	}

	cell_t* vec;
	pContext->LocalToPhysAddr(Params[3], &vec);

	TakeDmgOffset offset = static_cast<TakeDmgOffset>(Params[2]);
	switch (offset) {
	case vecDamageForce: {
		Vector force(sp_ctof(vec[0]), sp_ctof(vec[1]), sp_ctof(vec[2]));
		info->m_vecDamageForce = force;
		return 1;
	}

	case vecDamagePosition: {
		Vector dmg(sp_ctof(vec[0]), sp_ctof(vec[1]), sp_ctof(vec[2]));
		info->m_vecDamagePosition = dmg;
		return 1;
	}

	case vecReportedPosition: {
		Vector reported(sp_ctof(vec[0]), sp_ctof(vec[1]), sp_ctof(vec[2]));
		info->m_vecReportedPosition = reported;
		return 1;
	}

	default:
		return pContext->ThrowNativeError("Invalid data type %i for CTakeDamageInfo_Vector", Params[2]);
	}
	return 0;
}

cell_t sm_SetDamageInfo_GetEnt(IPluginContext* pContext, const cell_t* Params)
{
	CTakeDmgInfoBuilder* info = ReadDamageInfoFromHandle(pContext, Params[1]);
	if (!info) {
		return -1;
	}

	TakeDmgOffset offset = static_cast<TakeDmgOffset>(Params[2]);
	switch (offset) {
	case hInflictor:
		return info->m_hInflictor.GetEntryIndex();
	case hAttacker:
		return info->m_hAttacker.GetEntryIndex();
	case hDamageBonusProvider:
		return info->m_hDamageBonusProvider.GetEntryIndex();
	case hWeapon:
		return info->m_hWeapon.GetEntryIndex();

	default:
		return pContext->ThrowNativeError("Invalid data type %i for CTakeDamageInfo_Ent", Params[2]);
	}
	return -1;
}

cell_t sm_SetDamageInfo_SetEnt(IPluginContext* pContext, const cell_t* Params)
{
	CTakeDmgInfoBuilder* info = ReadDamageInfoFromHandle(pContext, Params[1]);
	if (!info) {
		return -1;
	}

	CBaseEntity* pEnt = gamehelpers->ReferenceToEntity(Params[3]);
	if (!pEnt) {
		return pContext->ThrowNativeError("Invalid entity index %i", Params[3]);
	}

	TakeDmgOffset offset = static_cast<TakeDmgOffset>(Params[2]);
	switch (offset) {
	case hInflictor:
		info->m_hInflictor = ((IHandleEntity*)(intp)pEnt)->GetRefEHandle();
		return 1;
	case hAttacker:
		info->m_hAttacker = ((IHandleEntity*)(intp)pEnt)->GetRefEHandle();
		return 1;
	case hDamageBonusProvider:
		info->m_hDamageBonusProvider = ((IHandleEntity*)(intp)pEnt)->GetRefEHandle();
		return 1;
	case hWeapon:
		info->m_hWeapon = ((IHandleEntity*)(intp)pEnt)->GetRefEHandle();
		return 1;

	default:
		return pContext->ThrowNativeError("Invalid data type %i for CTakeDamageInfo_Ent", Params[2]);
	}
	return -1;
}


bool CTakeDmgExt::SDK_OnLoad(char* error, size_t maxlength, bool late)
{
	IGameConfig* pConfig = NULL;
	if (!gameconfs->LoadGameConfigFile("tf2.takedmginfo", &pConfig, NULL, NULL)) {
		ke::SafeStrcpy(error, maxlength, "Failed to load tf2.takedmginfo.txt");
		return false;
	}
	int offset;
	if (!pConfig->GetOffset("CTFGameRules::DeathNotice", &offset)) {
		ke::SafeStrcpy(error, maxlength, "Failed to load \"CTFGameRules::DeathNotice\" offset");
		return false;
	}
	SH_MANUALHOOK_RECONFIGURE(DeathNotice, offset, 0, 0);
	gameconfs->CloseGameConfigFile(pConfig);
	
	sharesys->AddDependency(myself, "sdktools.ext", true, true);
	sharesys->AddNatives(myself, g_InfoNatives);
	g_TakeDmgInfo = handlesys->CreateType("CTakeDamageInfo", &g_CTakeDmgInfoHandler, 0, NULL, NULL, myself->GetIdentity(), NULL);

	return true;
}

void CTakeDmgExt::SDK_OnUnload()
{
	handlesys->RemoveType(g_TakeDmgInfo, myself->GetIdentity());
}

void CTakeDmgExt::SDK_OnAllLoaded()
{
	SM_GET_LATE_IFACE(SDKTOOLS, sdktools);
}


void CTakeDmgInfoHandler::OnHandleDestroy(Handle_t type, void* object)
{
	CTakeDmgInfoBuilder* obj = (CTakeDmgInfoBuilder*)object;
	if (obj != NULL) {
		delete obj;
	}
}

CTakeDmgInfoBuilder* ReadDamageInfoFromHandle(IPluginContext* pContext, cell_t Param)
{
	Handle_t hndl = static_cast<Handle_t>(Param);
	HandleError err;
	HandleSecurity sec(NULL, myself->GetIdentity());

	CTakeDmgInfoBuilder* info = NULL;
	if ((err = handlesys->ReadHandle(hndl, g_TakeDmgInfo, &sec, (void**)&info)) != HandleError_None) {
		pContext->ThrowNativeError("Invalid CTakeDmgInfo Handle %x (error %d)", hndl, err);
		return NULL;
	}
	return info;
}