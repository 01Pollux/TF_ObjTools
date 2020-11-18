#ifndef _INCLUDE_DMG_INFO
#define _INCLUDE_DMG_INFO

#include <ammodef.h>
#include <vector>
#include <memory>
#include <mathlib/vector.h>
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

class _CTakeDmgInfo: public IGlobalHooks,
					 public IHandleTypeDispatch,
					 public ISMEntityListener,
					 public IPluginsListener
{
public:	//	IGlobalHooks
	bool OnLoad(char*, size_t) override;
	void OnUnload() override;


public:	//	IHandleTypeDispatch
	void OnHandleDestroy(Handle_t, void*) override;
	CTakeDmgInfoBuilder* ReadHandle(IPluginContext*, cell_t);
	cell_t CreateHandle(CTakeDmgInfoBuilder*, HandleError* = NULL);

public:
	HandleType_t hndl_type;


public:	//	IPluginsListener
	void OnPluginUnloaded(IPlugin*) override;


public:	//	ISMEntityListener
	void OnEntityDestroyed(CBaseEntity*);


public:
	enum HookType
	{
		GenericPre,
		AlivePre,
		GenericPost,
		AlivePost,

		MaxHooks
	};
	std::vector<std::unique_ptr<IHookInfo>> HookedEnt[MaxHooks];

	void HookEnt(int entity, IPluginFunction* pCallback, HookType type);
	void UnHookEnt(int entity, IPluginFunction* pCallback, HookType type);
};

#endif