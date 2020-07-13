
#include "ammodefs.h"
#include "extension.h"

static inline bool IndexInRange(int index)
{
	if (index < 0 || index >= GetAmmoDef()->m_nAmmoIndex) {
		return false;
	}
	return true;
}

Ammo_t* CAmmoDef::GetAmmoOfIndex(int nAmmoIndex)
{
	if (nAmmoIndex >= m_nAmmoIndex) {
		return NULL;
	}
	return &m_AmmoType[nAmmoIndex];
}

int CAmmoDef::Index(const char* psz)
{
	int i;
	if (!psz) {
		return -1;
	}
	for (i = 1; i < m_nAmmoIndex; i++) {
		if (stricmp(psz, m_AmmoType[i].pName) == 0) {
			return i;
		}
	}
	return -1;
}

float CAmmoDef::DamageForce(int nAmmoIndex)
{
	if (nAmmoIndex < 1 || nAmmoIndex >= m_nAmmoIndex) {
		return 0;
	}
	return m_AmmoType[nAmmoIndex].physicsForceImpulse;
}


cell_t CAmmoDefs_FindIndex(IPluginContext* pContext, const cell_t* params)
{
	char* name;
	pContext->LocalToString(params[1], &name);
	return GetAmmoDef()->Index(name);
}

cell_t CAmmoDefs_GetName(IPluginContext* pContext, const cell_t* params)
{
	int index = static_cast<int>(params[1]);
	if (!IndexInRange(index)) {
		return pContext->ThrowNativeError("Index %i for AmmoIndex out of range [0 ... %i]", index, GetAmmoDef()->m_nAmmoIndex - 1);
	}
	Ammo_t* table = GetAmmoDef()->GetAmmoOfIndex(index);
	const char* name = table->pName;
	pContext->StringToLocal(params[2], params[3], name);
	return 1;
}

cell_t CAmmoDefs_GetDamageType(IPluginContext* pContext, const cell_t* params)
{
	int index = static_cast<int>(params[1]);
	if (!IndexInRange(index)) {
		return  pContext->ThrowNativeError("Index %i for AmmoIndex out of range", index);
	}
	Ammo_t* table = GetAmmoDef()->GetAmmoOfIndex(index);
	return table->nDamageType;
}

cell_t CAmmoDefs_GeteTracerType(IPluginContext* pContext, const cell_t* params)
{
	int index = static_cast<int>(params[1]);
	if (!IndexInRange(index)) {
		return  pContext->ThrowNativeError("Index %i for AmmoIndex out of range", index);
	}
	Ammo_t* table = GetAmmoDef()->GetAmmoOfIndex(index);
	return table->eTracerType;
}

cell_t CAmmoDefs_GetDamageForce(IPluginContext* pContext, const cell_t* params)
{
	int index = static_cast<int>(params[1]);
	if (!IndexInRange(index)) {
		return  pContext->ThrowNativeError("Index %i for AmmoIndex out of range", index);
	}
	Ammo_t* table = GetAmmoDef()->GetAmmoOfIndex(index);
	return sp_ftoc(table->physicsForceImpulse);
}

cell_t CAmmoDefs_GetFlags(IPluginContext* pContext, const cell_t* params)
{
	int index = static_cast<int>(params[1]);
	if (!IndexInRange(index)) {
		return  pContext->ThrowNativeError("Index %i for AmmoIndex out of range", index);
	}
	Ammo_t* table = GetAmmoDef()->GetAmmoOfIndex(index);
	return static_cast<AmmoFlags_t>(table->nFlags);
}

cell_t CAmmoDefs_GetMaxCarry(IPluginContext* pContext, const cell_t* params)
{
	int index = static_cast<int>(params[1]);
	if (!IndexInRange(index)) {
		return  pContext->ThrowNativeError("Index %i for AmmoIndex out of range", index);
	}
	Ammo_t* table = GetAmmoDef()->GetAmmoOfIndex(index);
	if (table->pMaxCarry == USE_CVAR)
	{
		if (table->pMaxCarryCVar)
		{
			return (int)table->pMaxCarryCVar->GetFloat();
		}
	}
	return table->pMaxCarry;
}

cell_t CAmmoDefs_GetRaw(IPluginContext* pContext, const cell_t* params)
{
	CAmmoDef* table = GetAmmoDef();
	if (!table) {
		return pContext->ThrowNativeError("Invalid AmmoDef table!");
	}
	return reinterpret_cast<cell_t>(table);
}

CON_COMMAND(dump_ammodef, "Dump all ammodef infos")
{
	CAmmoDef* table = GetAmmoDef();
	if (!table) {
		META_CONPRINT("Invalid CAmmoDef address");
		return;
	}
	int count = table->m_nAmmoIndex;
	Ammo_t* ammo = nullptr;

	META_CONPRINTF("Dumping CAmmoDef Table: \n");
	META_CONPRINTF("--------------------------------------------------------------------\n");
	for(int i = 0; i < count; i++) {
		ammo = table->GetAmmoOfIndex(i);
		
		META_CONPRINTF("Name: %s - Flags: %i - DmgType: %i - Tracer: %i - MaxCarry: %i\n",
			ammo->pName, ammo->nFlags, ammo->nDamageType, ammo->nDamageType, ammo->eTracerType, 
			[ammo]() -> int {
				if (ammo->pMaxCarry == USE_CVAR) {
					if (ammo->pMaxCarryCVar) {
						return ammo->pMaxCarryCVar->GetFloat();
					}
				}
				return ammo->pMaxCarry;
			});
	}
	META_CONPRINTF("--------------------------------------------------------------------\n");
}
