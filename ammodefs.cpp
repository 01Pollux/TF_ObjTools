
#include <tier1/utlvector.h>
#include <mathlib/vector.h>
#include <ammodef.h>
#include "extension.h"

#include "extension.h"

#define DECL_NATIVE_CALLBACK(NAME)		cell_t CAmmoDefs_##NAME(IPluginContext* pContext, const cell_t* params)

void RegNatives();

class _AmmoDefNatives : public IGlobalHooks
{
public:
public:	//	IGlobalHooks
	bool OnLoad(char*, size_t) override;
} static ammo_def;

bool _AmmoDefNatives::OnLoad(char*, size_t)
{
	RegNatives(); 
	return true;
}

CAmmoDef* GetAmmoDef();


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
	if (!psz)
		return -1;

	for (i = 1; i < m_nAmmoIndex; i++) 
	if (!stricmp(psz, m_AmmoType[i].pName))
		return i;

	return -1;
}

float CAmmoDef::DamageForce(int nAmmoIndex)
{
	if (nAmmoIndex < 1 || nAmmoIndex >= m_nAmmoIndex)
		return 0.0;

	return m_AmmoType[nAmmoIndex].physicsForceImpulse;
}


DECL_NATIVE_CALLBACK(FindIndex)
{
	char* name;
	pContext->LocalToString(params[1], &name);
	return GetAmmoDef()->Index(name);
}

DECL_NATIVE_CALLBACK(GetName)
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

DECL_NATIVE_CALLBACK(DamageType)
{
	int index = static_cast<int>(params[1]);
	if (!IndexInRange(index)) {
		return  pContext->ThrowNativeError("Index %i for AmmoIndex out of range", index);
	}
	Ammo_t* table = GetAmmoDef()->GetAmmoOfIndex(index);
	return table->nDamageType;
}

DECL_NATIVE_CALLBACK(eTracerType)
{
	int index = static_cast<int>(params[1]);
	if (!IndexInRange(index)) {
		return  pContext->ThrowNativeError("Index %i for AmmoIndex out of range", index);
	}
	Ammo_t* table = GetAmmoDef()->GetAmmoOfIndex(index);
	return table->eTracerType;
}

DECL_NATIVE_CALLBACK(DamageForce)
{
	int index = static_cast<int>(params[1]);
	if (!IndexInRange(index)) {
		return  pContext->ThrowNativeError("Index %i for AmmoIndex out of range", index);
	}
	Ammo_t* table = GetAmmoDef()->GetAmmoOfIndex(index);
	return sp_ftoc(table->physicsForceImpulse);
}

DECL_NATIVE_CALLBACK(GetFlags)
{
	int index = static_cast<int>(params[1]);
	if (!IndexInRange(index)) {
		return  pContext->ThrowNativeError("Index %i for AmmoIndex out of range", index);
	}
	Ammo_t* table = GetAmmoDef()->GetAmmoOfIndex(index);
	return static_cast<cell_t>(table->nFlags);
}

DECL_NATIVE_CALLBACK(GetMaxCarry)
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

DECL_NATIVE_CALLBACK(Raw)
{
	CAmmoDef* table = GetAmmoDef();
	if (!table) {
		return pContext->ThrowNativeError("Invalid AmmoDef table!");
	}
	return reinterpret_cast<cell_t>(table);
}


#define DECL_NATIVE(NAME)		{ "CAmmoDefs."###NAME,		 CAmmoDefs_##NAME }

const sp_nativeinfo_t ad_natives[] = {
	DECL_NATIVE(FindIndex),
	DECL_NATIVE(GetName),
	DECL_NATIVE(DamageType),
	DECL_NATIVE(eTracerType),
	DECL_NATIVE(DamageForce),
	DECL_NATIVE(GetFlags),
	DECL_NATIVE(GetMaxCarry),
	DECL_NATIVE(Raw),

	{NULL, NULL},
};

#undef DECL_NATIVE
#undef DECL_NATIVE_CALLBACK


inline void RegNatives()
{
	sharesys->AddNatives(myself, ad_natives);
}


CAmmoDef* GetAmmoDef()
{
	static CAmmoDef* (*pAmmoDef)() = nullptr;
	if (!pAmmoDef)
	{
		if (!gconfig->GetMemSig("AmmoDef", &reinterpret_cast<void*&>(pAmmoDef)) || !pAmmoDef)
		{
			smutils->LogError(myself, "Failed to find \"AmmoDef\" sig");
			return nullptr;
		}
	}
	return (*pAmmoDef)();
}
