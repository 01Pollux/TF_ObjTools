
#include "extension.h"
#include "dmginfo.h"
#include <random>
#include <algorithm>

#define DECL_NATIVE_CALLBACK(NAME)		cell_t CTakeDamageInfo_##NAME(IPluginContext* pContext, const cell_t* params)
#define DECL_NATIVE_CALLBACK_GET(NAME)	cell_t CTakeDamageInfo_##NAME##_GET(IPluginContext* pContext, const cell_t* params)
#define DECL_NATIVE_CALLBACK_SET(NAME)	cell_t CTakeDamageInfo_##NAME##_SET(IPluginContext* pContext, const cell_t* params)

SH_DECL_MANUALHOOK1(OnTakeDamage, 0, 0, 0, int, CTakeDmgInfoBuilder&);
SH_DECL_MANUALHOOK1(OnTakeDamageAlive, 0, 0, 0, int, CTakeDmgInfoBuilder&);

static int _OnTakeDamage(CTakeDmgInfoBuilder& infos);
static int _OnTakeDamageAlive(CTakeDmgInfoBuilder& infos);
static int _OnTakeDamagePost(CTakeDmgInfoBuilder& infos);
static int _OnTakeDamageAlivePost(CTakeDmgInfoBuilder& infos);

static void RegNatives();
extern CAmmoDef* GetAmmoDef();

extern ISDKHooks* sdkhooks;
static ConVar* phys_pushscale;
_CTakeDmgInfo take_dmg_info;


bool _CTakeDmgInfo::OnLoad(char* error, size_t maxlength)
{
	phys_pushscale = g_pCVar->FindVar("phys_pushscale");
	{
		int offset = 0;
		if (!gconfig->GetOffset("OnTakeDamage", &offset) || !offset)
		{
			ke::SafeStrcpy(error, maxlength, "Failed to find offset for \"OnTakeDamage\"");
			return false;
		}
		SH_MANUALHOOK_RECONFIGURE(OnTakeDamage, offset, 0, 0);
		if (!gconfig->GetOffset("OnTakeDamageAlive", &offset) || !offset)
		{
			ke::SafeStrcpy(error, maxlength, "Failed to find offset for \"OnTakeDamageAlive\"");
			return false;
		}
		SH_MANUALHOOK_RECONFIGURE(OnTakeDamageAlive, offset, 0, 0);
	}

	RegNatives();
	plsys->AddPluginsListener(this);

	this->hndl_type = handlesys->CreateType("CTakeDamageInfo", this, NULL, NULL, NULL, myself->GetIdentity(), NULL);

	return true;
}

void _CTakeDmgInfo::OnUnload()
{
	for (auto& hook : this->HookedEnt)
		hook.clear();

	handlesys->RemoveType(this->hndl_type, myself->GetIdentity());
	plsys->RemovePluginsListener(this);
	sdkhooks->RemoveEntityListener(this);
}

void _CTakeDmgInfo::OnHandleDestroy(Handle_t type, void* object)
{
	CTakeDmgInfoBuilder* obj = reinterpret_cast<CTakeDmgInfoBuilder*>(object);
	if (obj)
	{
		delete obj;
		obj = nullptr;
	}
}

void _CTakeDmgInfo::OnPluginUnloaded(IPlugin* pPlugin)
{
	IPluginContext* pContext = pPlugin->GetBaseContext();
	for (auto& hooks : HookedEnt)
	{
		for (auto entry = hooks.begin(); entry != hooks.end();)
		{
			auto& callbacks = (*entry)->pCallbacks;
			for (auto iter = callbacks.begin(); iter != callbacks.end(); iter)
			{
				if ((*iter)->GetParentContext() == pContext)
					iter = callbacks.erase(iter);
				else iter++;
			}

			if (callbacks.empty())
				entry = hooks.erase(entry);
			else entry++;
		}
	}
}

void _CTakeDmgInfo::OnEntityDestroyed(CBaseEntity* pEnt)
{
	int ref = gamehelpers->EntityToReference(pEnt);
	for (auto& hooks : HookedEnt)
	{
		for (auto iter = hooks.begin(); iter != hooks.end(); iter++)
		{
			if ((*iter)->ref != ref)
				continue;

			hooks.erase(iter--);
		}
	}
}

void _CTakeDmgInfo::HookEnt(int entity, IPluginFunction* pCallback, HookType type)
{
	int ref = gamehelpers->IndexToReference(entity);
	auto& hooks = HookedEnt[type];

	CBaseEntity* pEntity = gamehelpers->ReferenceToEntity(ref);

	size_t i;
	for (i = 0; i < hooks.size(); i++)
		if (ref == hooks[i]->ref)
			break;

	if (i == hooks.size())
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

		auto hook = std::make_unique<IHookInfo>(hookid, ref);
		hooks.emplace_back(std::move(hook));
	}

	auto& callbacks = hooks[i]->pCallbacks;
	for (auto& callback : callbacks)
		if (callback == pCallback)
			return;

	callbacks.push_back(pCallback);
}

void _CTakeDmgInfo::UnHookEnt(int entity, IPluginFunction* pCallback, HookType type)
{
	int ref = gamehelpers->IndexToReference(entity);
	auto& hooks = HookedEnt[type];
	for (auto entry = hooks.begin(); entry != hooks.end(); entry++)
	{
		if (ref != (*entry)->ref)
			continue;

		auto& callbacks = (*entry)->pCallbacks;
		for (auto iter = callbacks.begin(); iter != callbacks.end(); iter++)
		{
			if (pCallback == (*iter))
			{
				callbacks.erase(iter--);
				break;
			}
		}

		if (callbacks.empty())
			hooks.erase(entry--);

		break;
	}
}


inline cell_t _CTakeDmgInfo::CreateHandle(CTakeDmgInfoBuilder* data, HandleError* pError)
{
	return handlesys->CreateHandle(take_dmg_info.hndl_type, data, NULL, myself->GetIdentity(), pError);
}

CTakeDmgInfoBuilder* _CTakeDmgInfo::ReadHandle(IPluginContext* pContext, cell_t param)
{
	Handle_t hndl = static_cast<Handle_t>(param);
	HandleError err;
	HandleSecurity sec{ NULL, myself->GetIdentity() };

	CTakeDmgInfoBuilder* info;

	if ((err = handlesys->ReadHandle(hndl, take_dmg_info.hndl_type, &sec, &reinterpret_cast<void*&>(info))) != HandleError_None)
	{
		pContext->ThrowNativeError("Invalid CTakeDmgInfo Handle %x (error %d)", hndl, err);
		return nullptr;
	}

	return info;
}


static float RandomFloat(float min, float max)
{
	std::default_random_engine r_eng;
	std::uniform_real_distribution<float> random(min, max);

	return random(r_eng);
}

DECL_NATIVE_CALLBACK(CTakeDamageInfo)
{
	CTakeDmgInfoBuilder* data = new CTakeDmgInfoBuilder;
	HandleError err;
	Handle_t hndl = take_dmg_info.CreateHandle(data, &err);
	if (!hndl)
	{
		delete data;
		return pContext->ThrowNativeError("Invalid CTakeDamageInfo Handle %x (error %d)", hndl, err);
	}
	Msg("Handle_t = %x, HandleError: %i\n", err);
	return hndl;
}

DECL_NATIVE_CALLBACK_GET(Infos)
{
	CTakeDmgInfoBuilder* infos = take_dmg_info.ReadHandle(pContext, params[1]);
	if (!infos)
		return NULL;

	return reinterpret_cast<cell_t>(infos);
}

DECL_NATIVE_CALLBACK_SET(Infos)
{
	CTakeDmgInfoBuilder* infos = take_dmg_info.ReadHandle(pContext, params[1]);
	if (!infos)
		return pContext->ThrowNativeError("Invalid CTakeDamageInfo Handle %x", infos);

	void* addr = reinterpret_cast<void*>(params[2]);
	if (!addr || reinterpret_cast<uintptr_t>(addr) < 0x10000)
		return pContext->ThrowNativeError("Invalid Pointer: %p", addr);

	memcpy(infos, addr, sizeof(CTakeDmgInfoBuilder));
	return 1;
}

SH_DECL_MANUALHOOK3_void(DeathNotice, NULL, NULL, NULL, CBaseEntity*, CTakeDmgInfoBuilder&, const char*);
DECL_NATIVE_CALLBACK(DeathNotice)
{
	CTakeDmgInfoBuilder* infos = take_dmg_info.ReadHandle(pContext, params[1]);
	if (!infos)
		return pContext->ThrowNativeError("Invalid CTakeDamageInfo Handle %x", infos);

	CBaseEntity* pVictim = gamehelpers->ReferenceToEntity(params[2]);
	if (!pVictim)
		return pContext->ThrowNativeError("Invalid Entity index : %i", params[2]);

	void* gamerules = sdktools->GetGameRules();;
	if (!gamerules)
		return pContext->ThrowNativeError("Failed to find GameRules object");

	static int offset = 0;
	if (!offset)
	{
		if (!gconfig->GetOffset("CTFGameRules::DeathNotice", &offset) || !offset)
			return pContext->ThrowNativeError("Failed to find \"CTFGameRules::DeathNotice\" offset");

		SH_MANUALHOOK_RECONFIGURE(DeathNotice, offset, 0, 0);
	}

	SH_MCALL(gamerules, DeathNotice)(pVictim, *infos, "player_death");
	return 1;
}

DECL_NATIVE_CALLBACK(CalcExplosiveDmgForce)
{
	CTakeDmgInfoBuilder* infos = take_dmg_info.ReadHandle(pContext, params[1]);
	if (!infos)
		return pContext->ThrowNativeError("Invalid CTakeDamageInfo Handle %x", infos);

	cell_t* vecpos;
	pContext->LocalToPhysAddr(params[3], &vecpos);
	Vector vecPos(sp_ctof(vecpos[0]), sp_ctof(vecpos[0]), sp_ctof(vecpos[0]));

	cell_t* vecforce;
	pContext->LocalToPhysAddr(params[2], &vecforce);
	Vector vecForce(sp_ctof(vecforce[0]), sp_ctof(vecforce[0]), sp_ctof(vecforce[0]));

	infos->m_vecDamagePosition = vecPos;

	float flclamp = 75 * 400;
	float flscale = infos->m_flBaseDamage * 75 * 4;
	if (flscale > flclamp)
		flscale = flclamp;

	float flForceScale;
	{
		static std::default_random_engine r_eng;
		static std::uniform_real_distribution<float> random(0.85, 1.15);
		flForceScale = random(r_eng);
	}

	VectorNormalize(vecForce);
	vecForce *= flForceScale;
	vecForce *= phys_pushscale->GetFloat();
	vecForce *= flscale;
	infos->m_vecDamageForce = vecForce;

	return (vecForce != vec3_origin) ? 1 : 0;
}

DECL_NATIVE_CALLBACK(CalcBulletDamageForce)
{
	CTakeDmgInfoBuilder* infos = take_dmg_info.ReadHandle(pContext, params[1]);
	if (!infos)
		return pContext->ThrowNativeError("Invalid CTakeDamageInfo Handle %x", infos);

	cell_t* vecdir;
	pContext->LocalToPhysAddr(params[3], &vecdir);
	Vector vecPosition(sp_ctof(vecdir[0]), sp_ctof(vecdir[1]), sp_ctof(vecdir[2]));
	infos->m_vecDamagePosition = vecPosition;

	cell_t* vecforce;
	pContext->LocalToPhysAddr(params[4], &vecforce);
	Vector vecForce(sp_ctof(vecforce[0]), sp_ctof(vecforce[1]), sp_ctof(vecforce[2]));
	VectorNormalize(vecForce);

	vecForce *= GetAmmoDef()->DamageForce(params[2]);
	vecForce *= phys_pushscale->GetFloat();
	vecForce *= sp_ctof(params[5]);
	infos->m_vecDamageForce = vecForce;

	return (vecForce != vec3_origin) ? 1 : 0;
}

DECL_NATIVE_CALLBACK(CalcMeleeDamageForce)
{
	CTakeDmgInfoBuilder* infos = take_dmg_info.ReadHandle(pContext, params[1]);
	if (!infos)
		return pContext->ThrowNativeError("Invalid CTakeDamageInfo Handle %x", infos);

	cell_t* vecpos;
	pContext->LocalToPhysAddr(params[2], &vecpos);
	Vector vecPosition(sp_ctof(vecpos[0]), sp_ctof(vecpos[1]), sp_ctof(vecpos[2]));
	infos->m_vecDamagePosition = vecPosition;

	cell_t* vecforce;
	pContext->LocalToPhysAddr(params[3], &vecforce);
	Vector vecForce(sp_ctof(vecforce[0]), sp_ctof(vecforce[1]), sp_ctof(vecforce[2]));
	VectorNormalize(vecForce);

	float flscale = infos->m_flBaseDamage * 75 * 4;
	vecForce *= flscale;
	vecForce *= phys_pushscale->GetFloat();
	vecForce *= sp_ctof(params[4]);
	infos->m_vecDamageForce = vecForce;

	return (vecForce != vec3_origin) ? 1 : 0;
}

DECL_NATIVE_CALLBACK(ReadInt)
{
	CTakeDmgInfoBuilder* info = take_dmg_info.ReadHandle(pContext, params[1]);
	if (!info)
		return 0;

	TakeDmgOffset offset = static_cast<TakeDmgOffset>(params[2]);
	switch (offset)
	{
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
		return pContext->ThrowNativeError("Invalid data type %i for CTakeDamageInfo_Int", params[2]);
	}
}

DECL_NATIVE_CALLBACK(StoreInt)
{
	CTakeDmgInfoBuilder* info = take_dmg_info.ReadHandle(pContext, params[1]);
	if (!info)
		return 0;

	TakeDmgOffset offset = static_cast<TakeDmgOffset>(params[2]);
	switch (offset)
	{
	case bitsDamageType:
		info->m_bitsDamageType = params[3];
		return 1;
	case iDamageCustom:
		info->m_iDamageCustom = params[3];
		return 1;
	case iDamageStats:
		info->m_iDamageStats = params[3];
		return 1;
	case iAmmoType:
		info->m_iAmmoType = params[3];
		return 1;
	case iDamagedOtherPlayers:
		info->m_iDamagedOtherPlayers = params[3];
		return 1;
	case iPlayerPenetrationCount:
		info->m_iPlayerPenetrationCount = params[3];
		return 1;
	case bForceFriendlyFire:
		info->m_bForceFriendlyFire = params[3] != 0 ? true : false;
		return 1;
	case eCritType:
		info->m_eCritType = (CritType)params[3];
		return 1;

	default:
		return pContext->ThrowNativeError("Invalid data type %i for CTakeDamageInfo_Int", params[2]);
	}
}

DECL_NATIVE_CALLBACK(ReadFloat)
{
	CTakeDmgInfoBuilder* info = take_dmg_info.ReadHandle(pContext, params[1]);
	if (!info)
		return sp_ftoc(0.0);

	TakeDmgOffset offset = static_cast<TakeDmgOffset>(params[2]);
	switch (offset)
	{
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
		return pContext->ThrowNativeError("Invalid data type %i for CTakeDamageInfo_Float", params[2]);
	}
}

DECL_NATIVE_CALLBACK(StoreFloat)
{
	CTakeDmgInfoBuilder* info = take_dmg_info.ReadHandle(pContext, params[1]);
	if (!info)
		return 0;

	TakeDmgOffset offset = static_cast<TakeDmgOffset>(params[2]);
	switch (offset)
	{
	case flDamage:
		info->m_flDamage = sp_ctof(params[3]);
		return 1;
	case flMaxDamage:
		info->m_flMaxDamage = sp_ctof(params[3]);
		return 1;
	case flBaseDamage:
		info->m_flBaseDamage = sp_ctof(params[3]);
		return 1;
	case flDamageBonus:
		info->m_flDamageBonus = sp_ctof(params[3]);
		return 1;
	case flDamageForForce:
		info->m_flDamageForForce = sp_ctof(params[3]);
		return 1;

	default:
		return pContext->ThrowNativeError("Invalid data type %i for CTakeDamageInfo_Float", params[2]);
	}
}

DECL_NATIVE_CALLBACK(ReadVector)
{
	CTakeDmgInfoBuilder* info = take_dmg_info.ReadHandle(pContext, params[1]);
	if (!info)
		return 0;

	cell_t* vec;
	pContext->LocalToPhysAddr(params[3], &vec);

	TakeDmgOffset offset = static_cast<TakeDmgOffset>(params[2]);
	switch (offset)
	{
	case vecDamageForce: 
	{
		Vector force = info->m_vecDamageForce;
		vec[0] = sp_ftoc(force.x);
		vec[1] = sp_ftoc(force.y);
		vec[2] = sp_ftoc(force.z);
		return 1;
	}

	case vecDamagePosition: 
	{
		Vector dmg = info->m_vecDamagePosition;
		vec[0] = sp_ftoc(dmg.x);
		vec[1] = sp_ftoc(dmg.y);
		vec[2] = sp_ftoc(dmg.z);
		return 1;
	}

	case vecReportedPosition:
	{
		Vector reported = info->m_vecReportedPosition;
		vec[0] = sp_ftoc(reported.x);
		vec[1] = sp_ftoc(reported.y);
		vec[2] = sp_ftoc(reported.z);
		return 1;
	}

	default:
		return pContext->ThrowNativeError("Invalid data type %i for CTakeDamageInfo_Vector", params[2]);
	}
}

DECL_NATIVE_CALLBACK(StoreVector)
{
	CTakeDmgInfoBuilder* info = take_dmg_info.ReadHandle(pContext, params[1]);
	if (!info)
		return 0;

	cell_t* vec;
	pContext->LocalToPhysAddr(params[3], &vec);

	TakeDmgOffset offset = static_cast<TakeDmgOffset>(params[2]);
	switch (offset)
	{
	case vecDamageForce:
	{
		Vector force(sp_ctof(vec[0]), sp_ctof(vec[1]), sp_ctof(vec[2]));
		info->m_vecDamageForce = force;
		return 1;
	}

	case vecDamagePosition:
	{
		Vector dmg(sp_ctof(vec[0]), sp_ctof(vec[1]), sp_ctof(vec[2]));
		info->m_vecDamagePosition = dmg;
		return 1;
	}

	case vecReportedPosition:
	{
		Vector reported(sp_ctof(vec[0]), sp_ctof(vec[1]), sp_ctof(vec[2]));
		info->m_vecReportedPosition = reported;
		return 1;
	}

	default:
		return pContext->ThrowNativeError("Invalid data type %i for CTakeDamageInfo_Vector", params[2]);
	}
}

DECL_NATIVE_CALLBACK(ReadEnt)
{
	CTakeDmgInfoBuilder* info = take_dmg_info.ReadHandle(pContext, params[1]);
	if (!info)
		return -1;

	TakeDmgOffset offset = static_cast<TakeDmgOffset>(params[2]);
	switch (offset)
	{
	case hInflictor:
		return info->m_hInflictor.IsValid() ? info->m_hInflictor.GetEntryIndex() : -1;
	case hAttacker:
		return info->m_hAttacker.IsValid() ? info->m_hAttacker.GetEntryIndex() : -1;
	case hDamageBonusProvider:
		return info->m_hDamageBonusProvider.IsValid() ? info->m_hDamageBonusProvider.GetEntryIndex() : -1;
	case hWeapon:
		return info->m_hWeapon.IsValid() ? info->m_hWeapon.GetEntryIndex() : -1;

	default:
		return pContext->ThrowNativeError("Invalid data type %i for CTakeDamageInfo_Ent", params[2]);
	}
}

DECL_NATIVE_CALLBACK(StoreEnt)
{
	CTakeDmgInfoBuilder* info = take_dmg_info.ReadHandle(pContext, params[1]);
	if (!info)
		return 0;

	CBaseEntity* pEnt = NULL;
	if (params[3] != -1)
	{
		pEnt = gamehelpers->ReferenceToEntity(params[3]);
		if (!pEnt)
			return pContext->ThrowNativeError("Invalid entity index %i", params[3]);
	}

	TakeDmgOffset offset = static_cast<TakeDmgOffset>(params[2]);
	switch (offset)
	{
	case hInflictor:
		info->m_hInflictor = reinterpret_cast<IHandleEntity*>(pEnt)->GetRefEHandle();
		return 1;
	case hAttacker:
		info->m_hAttacker = reinterpret_cast<IHandleEntity*>(pEnt)->GetRefEHandle();
		return 1;
	case hDamageBonusProvider:
		info->m_hDamageBonusProvider = reinterpret_cast<IHandleEntity*>(pEnt)->GetRefEHandle();
		return 1;
	case hWeapon:
		info->m_hWeapon = reinterpret_cast<IHandleEntity*>(pEnt)->GetRefEHandle();
		return 1;

	default:
		return pContext->ThrowNativeError("Invalid data type %i for CTakeDamageInfo_Ent", params[2]);
	}
}


static inline bool ValidHook(_CTakeDmgInfo::HookType h)
{
	return (h < _CTakeDmgInfo::MaxHooks || h >= 0);
}

cell_t HookRawOnTakeDamage(IPluginContext* pContext, const cell_t* params)
{
	cell_t entity = params[1];
	if (!gamehelpers->ReferenceToEntity(entity))
		return pContext->ThrowNativeError("Invalid entity index: %i", entity);

	IPluginFunction* pFunction = pContext->GetFunctionById(params[2]);
	if (!pFunction)
		return pContext->ThrowNativeError("Invalid Function id: %x", pFunction);

	auto type = static_cast<_CTakeDmgInfo::HookType>(params[3]);
	if (!ValidHook(type))
		return pContext->ThrowNativeError("Invalid HookType: %i", type);

	take_dmg_info.HookEnt(entity, pFunction, type);
	return 1;
}

cell_t UnhookRawOnTakeDamage(IPluginContext* pContext, const cell_t* params)
{
	cell_t entity = params[1];
	if (!gamehelpers->ReferenceToEntity(entity))
		return pContext->ThrowNativeError("Invalid entity index: %i", entity);

	IPluginFunction* pFunction = pContext->GetFunctionById(params[2]);
	if (!pFunction)
		return pContext->ThrowNativeError("Invalid Function id: %x", pFunction);

	auto type = static_cast<_CTakeDmgInfo::HookType>(params[3]);
	if (!ValidHook(type))
		return pContext->ThrowNativeError("Invalid HookType: %i", type);

	take_dmg_info.UnHookEnt(entity, pFunction, type);
	return 1;
}


#define DECL_NATIVE(NAME)		{ "CTakeDamageInfo."###NAME,		 CTakeDamageInfo_##NAME }
#define DECL_NATIVE_GET(NAME)	{ "CTakeDamageInfo."###NAME##".get", CTakeDamageInfo_##NAME##_GET }
#define DECL_NATIVE_SET(NAME)	{ "CTakeDamageInfo."###NAME##".set", CTakeDamageInfo_##NAME##_SET }

const sp_nativeinfo_t dmg_natives[] =
{
	DECL_NATIVE(CTakeDamageInfo),
	DECL_NATIVE(ReadInt),
	DECL_NATIVE(StoreInt),
	DECL_NATIVE(ReadFloat),
	DECL_NATIVE(ReadFloat),
	DECL_NATIVE(ReadVector),
	DECL_NATIVE(StoreVector),
	DECL_NATIVE(ReadEnt),
	DECL_NATIVE(StoreEnt),
	DECL_NATIVE_GET(Infos),
	DECL_NATIVE_SET(Infos),
	DECL_NATIVE(DeathNotice),
	DECL_NATIVE(CalcExplosiveDmgForce),
	DECL_NATIVE(CalcBulletDamageForce),
	DECL_NATIVE(CalcMeleeDamageForce),

	{"HookRawOnTakeDamage", HookRawOnTakeDamage},
	{"UnhookRawOnTakeDamage", UnhookRawOnTakeDamage},
	{NULL, NULL},
};

#undef DECL_NATIVE
#undef DECL_NATIVE_SET
#undef DECL_NATIVE_GET
#undef DECL_NATIVE_CALLBACK
#undef DECL_NATIVE_CALLBACK_GET
#undef DECL_NATIVE_CALLBACK_SET


/// OnTakeDamage* Hook

static int HandleTakeDmgInfo(CTakeDmgInfoBuilder* infos, _CTakeDmgInfo::HookType type)
{
	CBaseEntity* pVictim = META_IFACEPTR(CBaseEntity);
	int ref = gamehelpers->EntityToReference(pVictim);

	auto& hooks = take_dmg_info.HookedEnt[type];

	for (auto entry = hooks.begin(); entry != hooks.end(); entry++)
	{
		IHookInfo* pHook = (*entry).get();
		if (ref != pHook->ref)
			continue;

		auto& callbacks = pHook->pCallbacks;
		if (callbacks.empty())
		{
			hooks.erase(entry--);
			RETURN_META_VALUE(MRES_IGNORED, NULL);
		}

		static IdentityToken_t* token = myself->GetIdentity();
		Handle_t hndl = take_dmg_info.CreateHandle(infos);

		int client = gamehelpers->ReferenceToBCompatRef(ref);

		cell_t action = Pl_Continue;
		cell_t final = Pl_Continue;
		for (auto& callback : callbacks)
		{
			callback->PushCell(client);
			callback->PushCellByRef(&reinterpret_cast<cell_t&>(hndl));
			callback->Execute(&action);

			if (action > final)
			{
				final = action;
				if (final == Pl_Changed)
				{
					auto newinfo = take_dmg_info.ReadHandle(callback->GetParentContext(), hndl);
					if (!newinfo)
					{
						handlesys->FreeHandle(hndl, NULL);
						RETURN_META_VALUE(MRES_IGNORED, NULL);
					}

					CBaseHandle& basehndl = newinfo->m_hAttacker;
					if (!basehndl.IsValid())
					{
						handlesys->FreeHandle(hndl, NULL);
						callback->GetParentContext()->BlamePluginError(callback, "Invalid Attacker index: %d", basehndl.GetEntryIndex());
						RETURN_META_VALUE(MRES_IGNORED, NULL);
					}

					basehndl = newinfo->m_hAttacker;
					if (!basehndl.IsValid())
					{
						handlesys->FreeHandle(hndl, NULL);
						callback->GetParentContext()->BlamePluginError(callback, "Invalid Inflictor index: %d", basehndl.GetEntryIndex());
						RETURN_META_VALUE(MRES_IGNORED, NULL);
					}

					memcpy(infos, newinfo, sizeof(CTakeDmgInfoBuilder));
				}
			}
		}

		handlesys->FreeHandle(hndl, NULL);
		if (final >= Pl_Handled)
			RETURN_META_VALUE(MRES_SUPERCEDE, 1);
		else if (final == Pl_Changed)
			RETURN_META_VALUE(MRES_HANDLED, 1);

		break;
	}
	RETURN_META_VALUE(MRES_IGNORED, NULL);
}

static int HandleTakeDmgInfoPost(CTakeDmgInfoBuilder* infos, _CTakeDmgInfo::HookType type)
{
	CBaseEntity* pVictim = META_IFACEPTR(CBaseEntity);
	int ref = gamehelpers->EntityToReference(pVictim);

	auto& hooks = take_dmg_info.HookedEnt[type];

	for (auto entry = hooks.begin(); entry != hooks.end(); entry++)
	{
		IHookInfo* pHook = (*entry).get();
		if (ref != pHook->ref)
			continue;

		auto& callbacks = pHook->pCallbacks;
		if (callbacks.empty())
		{
			hooks.erase(entry--);
			RETURN_META_VALUE(MRES_IGNORED, NULL);
		}

		static auto token = myself->GetIdentity();
		Handle_t hndl = handlesys->CreateHandle(take_dmg_info.hndl_type, reinterpret_cast<void*>(infos), token, token, NULL);
		int client = gamehelpers->ReferenceToBCompatRef(ref);

		for (auto& callback : callbacks)
		{
			callback->PushCell(client);
			callback->PushCell(hndl);
			callback->Execute(NULL);
		}

		handlesys->FreeHandle(hndl, NULL);
		break;
	}

	RETURN_META_VALUE(MRES_IGNORED, NULL);
}

int _OnTakeDamage(CTakeDmgInfoBuilder& infos)
{
	return HandleTakeDmgInfo(&infos, _CTakeDmgInfo::HookType::GenericPre);
}

int _OnTakeDamageAlive(CTakeDmgInfoBuilder& infos)
{
	return HandleTakeDmgInfo(&infos, _CTakeDmgInfo::HookType::AlivePre);
}

int _OnTakeDamagePost(CTakeDmgInfoBuilder& infos)
{
	return HandleTakeDmgInfoPost(&infos, _CTakeDmgInfo::HookType::GenericPost);
}

int _OnTakeDamageAlivePost(CTakeDmgInfoBuilder& infos)
{
	return HandleTakeDmgInfoPost(&infos, _CTakeDmgInfo::HookType::AlivePost);
}


inline void RegNatives()
{
	sharesys->AddNatives(myself, dmg_natives);
}