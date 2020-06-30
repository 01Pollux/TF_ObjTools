
#include "ammodefs.h"
#include "dmginfo.h"
#include <vstdlib/random.h>

void CTakeDmgInfoHandler::OnHandleDestroy(Handle_t type, void* object)
{
	CTakeDmgInfoBuilder* obj = (CTakeDmgInfoBuilder*)object;
	if (obj != NULL) 
	{
		delete obj;
	}
}

CTakeDmgInfoBuilder* ReadDamageInfoFromHandle(IPluginContext* pContext, cell_t Param)
{
	Handle_t hndl = static_cast<Handle_t>(Param);
	HandleError err;
	HandleSecurity sec(NULL, myself->GetIdentity());

	CTakeDmgInfoBuilder* info = NULL;
	if ((err = handlesys->ReadHandle(hndl, g_TakeDmgInfo, &sec, (void**)&info)) != HandleError_None) 
	{
		pContext->ThrowNativeError("Invalid CTakeDmgInfo Handle %x (error %d)", hndl, err);
		return NULL;
	}
	return info;
}


cell_t CTakeDamageInfo_CTakeDamageInfo(IPluginContext* pContext, const cell_t* params)
{
	CTakeDmgInfoBuilder* info = new CTakeDmgInfoBuilder;
	memset(info, 0, sizeof(CTakeDmgInfoBuilder));

	HandleError err;
	Handle_t hndl = handlesys->CreateHandle(g_TakeDmgInfo, info, pContext->GetIdentity(), myself->GetIdentity(), &err);
	if (!hndl) 
	{
		return pContext->ThrowNativeError("Invalid CTakeDamageInfo Handle %x (error %d)", hndl, err);
	}
	return hndl;
}

cell_t CTakeDamageInfo_GetData(IPluginContext* pContext, const cell_t* params)
{
	CTakeDmgInfoBuilder* infos = ReadDamageInfoFromHandle(pContext, params[1]);
	if (!infos) 
	{
		return NULL;
	}

#ifdef PLATFORM_X86
	return reinterpret_cast<cell_t>(infos);
#else
	return smutils->ToPseudoAddress((void*)infos);
#endif
}

cell_t CTakeDamageInfo_SetData(IPluginContext* pContext, const cell_t* params)
{
	CTakeDmgInfoBuilder* infos = ReadDamageInfoFromHandle(pContext, params[1]);
	if (!infos)
	{
		return pContext->ThrowNativeError("Invalid CTakeDamageInfo Handle %x", infos);
	}
	void* addr = reinterpret_cast<void*>(params[2]);
	if (!addr)
	{
		return pContext->ThrowNativeError("Address cannot be NULL!");
	}
	else if (reinterpret_cast<uintptr_t>(addr) < 0x10000)
	{
		return pContext->ThrowNativeError("Address: 0x%x is poiting to a reserved memory.", addr);
	}

	memcpy(infos, addr, sizeof(CTakeDmgInfoBuilder));
	return 1;
}

SH_DECL_MANUALHOOK3_void(DeathNotice, 0, 0, 0, CBaseEntity*, CTakeDmgInfoBuilder&, const char*)
cell_t CTakeDamageInfo_DeathNotice(IPluginContext* pContext, const cell_t* params)
{
	CTakeDmgInfoBuilder* infos = ReadDamageInfoFromHandle(pContext, params[1]);
	if (!infos)
	{
		return pContext->ThrowNativeError("Invalid CTakeDamageInfo Handle %x", infos);
	}

	CBaseEntity* pVictim = gamehelpers->ReferenceToEntity(params[2]);
	if (!pVictim) 
	{
		return pContext->ThrowNativeError("Invalid Entity index : %i", params[2]);
	}

	void* gamerules = sdktools->GetGameRules();
	if (!gamerules) 
	{
		return pContext->ThrowNativeError("Failed to find GameRules object");
	}

	static int offset = 0;
	if (!offset) 
	{
		if (!pConfig->GetOffset("CTFGameRules::DeathNotice", &offset)) {
			return pContext->ThrowNativeError("Failed to find \"CTFGameRules::DeathNotice\" offset");;
		}
		SH_MANUALHOOK_RECONFIGURE(DeathNotice, offset, 0, 0);
	}

	SH_MCALL(gamerules, DeathNotice)(pVictim, *infos, "player_death");
	return 1;
}

cell_t CalcExplosiveDmgForce(IPluginContext* pContext, const cell_t* params)
{
	CTakeDmgInfoBuilder* infos = ReadDamageInfoFromHandle(pContext, params[1]);
	if (!infos)
	{
		return pContext->ThrowNativeError("Invalid CTakeDamageInfo Handle %x", infos);
	}
	
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
	{
		flscale = flclamp;
	}
	
	float flForceScale = RandomFloat(0.85, 1.15);

	VectorNormalize(vecForce);
	vecForce *= flForceScale;
	vecForce *= phys_pushscale->GetFloat();
	vecForce *= flscale;
	infos->m_vecDamageForce = vecForce;

	return vecForce != vec3_origin;
}

cell_t CalcBulletDamageForce(IPluginContext* pContext, const cell_t* params)
{
	CTakeDmgInfoBuilder* infos = ReadDamageInfoFromHandle(pContext, params[1]);
	if (!infos) 
	{
		return pContext->ThrowNativeError("Invalid CTakeDamageInfo Handle %x", infos);
	}

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

	return vecForce != vec3_origin;
}

cell_t CalcMeleeDamageForce(IPluginContext* pContext, const cell_t* params)
{
	CTakeDmgInfoBuilder* infos = ReadDamageInfoFromHandle(pContext, params[1]);
	if (!infos) 
	{
		return pContext->ThrowNativeError("Invalid CTakeDamageInfo Handle %x", infos);
	}

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
	
	return vecForce != vec3_origin;
}

cell_t CTakeDamageInfo_GetInt(IPluginContext* pContext, const cell_t* params)
{
	CTakeDmgInfoBuilder* info = ReadDamageInfoFromHandle(pContext, params[1]);
	if (!info)
	{
		return 0;
	}

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
	return 0;
}

cell_t CTakeDamageInfo_SetInt(IPluginContext* pContext, const cell_t* params)
{
	CTakeDmgInfoBuilder* info = ReadDamageInfoFromHandle(pContext, params[1]);
	if (!info)
	{
		return 0;
	}

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
	return 0;
}

cell_t CTakeDamageInfo_GetFloat(IPluginContext* pContext, const cell_t* params)
{
	CTakeDmgInfoBuilder* info = ReadDamageInfoFromHandle(pContext, params[1]);
	if (!info)
	{
		return 0.0;
	}
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
	return 0.0;
}

cell_t CTakeDamageInfo_SetFloat(IPluginContext* pContext, const cell_t* params)
{
	CTakeDmgInfoBuilder* info = ReadDamageInfoFromHandle(pContext, params[1]);
	if (!info)
	{
		return 0;
	}

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
	return 0;
}

cell_t CTakeDamageInfo_GetVector(IPluginContext* pContext, const cell_t* params)
{
	CTakeDmgInfoBuilder* info = ReadDamageInfoFromHandle(pContext, params[1]);
	if (!info)
	{
		return 0;
	}

	cell_t* vec;
	pContext->LocalToPhysAddr(params[3], &vec);

	TakeDmgOffset offset = static_cast<TakeDmgOffset>(params[2]);
	switch (offset)
	{
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
		return pContext->ThrowNativeError("Invalid data type %i for CTakeDamageInfo_Vector", params[2]);
	}
	return 0;
}

cell_t CTakeDamageInfo_SetVector(IPluginContext* pContext, const cell_t* params)
{
	CTakeDmgInfoBuilder* info = ReadDamageInfoFromHandle(pContext, params[1]);
	if (!info)
	{
		return 0;
	}

	cell_t* vec;
	pContext->LocalToPhysAddr(params[3], &vec);

	TakeDmgOffset offset = static_cast<TakeDmgOffset>(params[2]);
	switch (offset) 
	{
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
		return pContext->ThrowNativeError("Invalid data type %i for CTakeDamageInfo_Vector", params[2]);
	}
	return 0;
}

cell_t CTakeDamageInfo_GetEnt(IPluginContext* pContext, const cell_t* params)
{
	CTakeDmgInfoBuilder* info = ReadDamageInfoFromHandle(pContext, params[1]);
	if (!info)
	{
		return -1;
	}

	TakeDmgOffset offset = static_cast<TakeDmgOffset>(params[2]);
	switch (offset)
	{
	case hInflictor:
		return info->m_hInflictor.GetEntryIndex();
	case hAttacker:
		return info->m_hAttacker.GetEntryIndex();
	case hDamageBonusProvider:
		return info->m_hDamageBonusProvider.GetEntryIndex();
	case hWeapon:
		return info->m_hWeapon.GetEntryIndex();

	default:
		return pContext->ThrowNativeError("Invalid data type %i for CTakeDamageInfo_Ent", params[2]);
	}
	return -1;
}

cell_t CTakeDamageInfo_SetEnt(IPluginContext* pContext, const cell_t* params)
{
	CTakeDmgInfoBuilder* info = ReadDamageInfoFromHandle(pContext, params[1]);
	if (!info)
	{
		return -1;
	}

	CBaseEntity* pEnt = gamehelpers->ReferenceToEntity(params[3]);
	if (!pEnt)
	{
		return pContext->ThrowNativeError("Invalid entity index %i", params[3]);
	}

	TakeDmgOffset offset = static_cast<TakeDmgOffset>(params[2]);
	switch (offset)
	{
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
		return pContext->ThrowNativeError("Invalid data type %i for CTakeDamageInfo_Ent", params[2]);
	}
	return -1;
}
