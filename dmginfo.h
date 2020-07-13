#pragma once

#include <ammodef.h>
#include <mathlib/vector.h>
#include <vtable_hook_helper.h>
#include "extension.h"

enum CritType {
	kCritType_None,
	kCritType_MiniCrit,
	kCritType_Crit,
};

class CTakeDmgInfoBuilder
{
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

enum TakeDmgOffset
{
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

enum HookType
{
	GenericPre,
	AlivePre,
	GenericPost,
	AlivePost,

	MaxHooks
};


struct IHookP
{
public:
	int ref;
	int hookid;
	ke::Vector<IPluginFunction*> pCallbacks;

public:
	IHookP(int id, int ref): hookid(hookid), ref(ref) { };
	~IHookP()
	{
		if (hookid)
		{
			SH_REMOVE_HOOK_ID(hookid);
			hookid = 0;
		}
	}
};

cell_t CTakeDamageInfo_CTakeDamageInfo(IPluginContext* pContext, const cell_t* params);
cell_t CTakeDamageInfo_GetInt(IPluginContext* pContext, const cell_t* params);
cell_t CTakeDamageInfo_SetInt(IPluginContext* pContext, const cell_t* params);
cell_t CTakeDamageInfo_GetFloat(IPluginContext* pContext, const cell_t* params);
cell_t CTakeDamageInfo_SetFloat(IPluginContext* pContext, const cell_t* params);
cell_t CTakeDamageInfo_GetVector(IPluginContext* pContext, const cell_t* params);
cell_t CTakeDamageInfo_SetVector(IPluginContext* pContext, const cell_t* params);
cell_t CTakeDamageInfo_GetEnt(IPluginContext* pContext, const cell_t* params);
cell_t CTakeDamageInfo_SetEnt(IPluginContext* pContext, const cell_t* params);
cell_t CTakeDamageInfo_GetData(IPluginContext* pContext, const cell_t* params);
cell_t CTakeDamageInfo_SetData(IPluginContext* pContext, const cell_t* params);
cell_t CTakeDamageInfo_DeathNotice(IPluginContext* pContext, const cell_t* params);
cell_t CalcExplosiveDmgForce(IPluginContext* pContext, const cell_t* params);
cell_t CalcBulletDamageForce(IPluginContext* pContext, const cell_t* params);
cell_t CalcMeleeDamageForce(IPluginContext* pContext, const cell_t* params);

cell_t HookRawOnTakeDamage(IPluginContext* pContext, const cell_t* params);
cell_t UnhookRawOnTakeDamage(IPluginContext* pContext, const cell_t* params);

CTakeDmgInfoBuilder* ReadDamageInfoFromHandle(IPluginContext* pContext, cell_t Param);

const sp_nativeinfo_t g_InfoNatives[] =
{
	{"CTakeDamageInfo.CTakeDamageInfo", CTakeDamageInfo_CTakeDamageInfo},
	{"CTakeDamageInfo.ReadInt", CTakeDamageInfo_GetInt},
	{"CTakeDamageInfo.StoreInt", CTakeDamageInfo_SetInt},
	{"CTakeDamageInfo.ReadFloat", CTakeDamageInfo_GetFloat},
	{"CTakeDamageInfo.StoreFloat", CTakeDamageInfo_SetFloat},
	{"CTakeDamageInfo.ReadVector", CTakeDamageInfo_GetVector},
	{"CTakeDamageInfo.StoreVector", CTakeDamageInfo_SetVector},
	{"CTakeDamageInfo.ReadEnt", CTakeDamageInfo_GetEnt},
	{"CTakeDamageInfo.StoreEnt", CTakeDamageInfo_SetEnt},
	{"CTakeDamageInfo.Infos.get", CTakeDamageInfo_GetData},
	{"CTakeDamageInfo.Infos.set", CTakeDamageInfo_SetData},
	{"CTakeDamageInfo.DeathNotice", CTakeDamageInfo_DeathNotice},
	{"CTakeDamageInfo.CalcExplosiveDmgForce", CalcExplosiveDmgForce},
	{"CTakeDamageInfo.CalcBulletDamageForce", CalcBulletDamageForce},
	{"CTakeDamageInfo.CalcMeleeDamageForce", CalcBulletDamageForce},

	{"HookRawOnTakeDamage", HookRawOnTakeDamage},
	{"UnhookRawOnTakeDamage", UnhookRawOnTakeDamage},
	{NULL, NULL},
};

class CTakeDmgInfoHandler: public IHandleTypeDispatch
{
public:
	void OnHandleDestroy(Handle_t type, void* object);
};

CTakeDmgInfoBuilder* ReadDamageInfoFromHandle(IPluginContext* pContext, cell_t Param);

extern CTakeDmgInfoHandler g_CTakeDmgInfoHandler;
extern HandleType_t g_TakeDmgInfo;
extern ConVar* phys_pushscale;
extern ke::Vector<IHookP*> IHookedEnt[MaxHooks];
