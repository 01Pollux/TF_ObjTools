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

#ifndef _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_
#define _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_

 /**
  * @file extension.h
  * @brief Sample extension code header.
  */

#include "smsdk_ext.h"
#include <basehandle.h>
#include <mathlib/vector.h>
#include <ISDKTools.h>

enum CritType {
	kCritType_None,
	kCritType_MiniCrit,
	kCritType_Crit,
};

class CTakeDmgInfoBuilder {

public:
	Vector m_vecDamageForce;
	Vector m_vecDamagePosition;
	Vector m_vecReportedPosition;

	CBaseHandle m_hInflictor;
	CBaseHandle m_hAttacker;
	CBaseHandle m_hWeapon;

	float m_flDamage;
	float m_flMaxDamage;
	float m_flBaseDamage;
	int m_bitsDamageType;
	int m_iDamageCustom;
	int m_iDamageStats;
	int m_iAmmoType;
	int m_iDamagedOtherPlayers;
	int m_iPlayerPenetrationCount;
	float m_flDamageBonus;

	CBaseHandle m_hDamageBonusProvider;

	bool m_bForceFriendlyFire;
	float m_flDamageForForce;
	CritType m_eCritType;
};

enum TakeDmgOffset {
	vecDamageForce = 0,
	vecDamagePosition,
	vecReportedPosition,
	hInflictor,
	hAttacker,
	hWeapon,
	flDamage,
	flMaxDamage,
	flBaseDamage,
	bitsDamageType,
	iDamageCustom,
	iDamageStats,
	iAmmoType,
	iDamagedOtherPlayers,
	iPlayerPenetrationCount,
	flDamageBonus,
	hDamageBonusProvider,
	bForceFriendlyFire,
	flDamageForForce,
	eCritType
};


static cell_t sm_CTakeDamageInfo(IPluginContext* pContext, const cell_t* Params);
static cell_t sm_SetDamageInfo_GetInt(IPluginContext* pContext, const cell_t* Params);
static cell_t sm_SetDamageInfo_SetInt(IPluginContext* pContext, const cell_t* Params);
static cell_t sm_SetDamageInfo_GetFloat(IPluginContext* pContext, const cell_t* Params);
static cell_t sm_SetDamageInfo_SetFloat(IPluginContext* pContext, const cell_t* Params);
static cell_t sm_SetDamageInfo_GetVector(IPluginContext* pContext, const cell_t* Params);
static cell_t sm_SetDamageInfo_SetVector(IPluginContext* pContext, const cell_t* Params);
static cell_t sm_SetDamageInfo_GetEnt(IPluginContext* pContext, const cell_t* Params);
static cell_t sm_SetDamageInfo_SetEnt(IPluginContext* pContext, const cell_t* Params);
static cell_t sm_GetDamageInfo_Data(IPluginContext* pContext, const cell_t* Params);
static cell_t sm_CTakeDamageInfo_Fire(IPluginContext* pContext, const cell_t* Params);

CTakeDmgInfoBuilder* ReadDamageInfoFromHandle(IPluginContext* pContext, cell_t Param);

class CTakeDmgInfoHandler: public IHandleTypeDispatch {
public:
	void OnHandleDestroy(Handle_t type, void* object);
};


class CTakeDmgExt : public SDKExtension
{
public:
	
	virtual bool SDK_OnLoad(char* error, size_t maxlength, bool late);

	virtual void SDK_OnAllLoaded();

	virtual void SDK_OnUnload();
};

extern HandleType_t g_TakeDmgInfo;
extern CTakeDmgInfoHandler g_CTakeDmgInfoHandler;
extern ISDKTools* sdktools;

#endif // _INCLUDE_SOURCEMOD_EXTENSION_PROPER_H_