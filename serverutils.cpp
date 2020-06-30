#include "serverutils.h"
#include "dmginfo.h"

class CTakeDamageInfo: public CTakeDmgInfoBuilder {};

#define CELLT_TO_ENTITY(pentity, param) \
		if(param == -1) \
		{ \
			pentity = NULL; \
		} \
		else \
		{ \
			pentity = gamehelpers->ReferenceToEntity(param); \
			if (!pentity) \
			{ \
				return pContext->ThrowNativeError("Invalid entity index: %i", param); \
			} \
		}

cell_t ST_FindEntityProcedural(IPluginContext* pContext, const cell_t* params)
{
	CBaseEntity* pSearch;
	CELLT_TO_ENTITY(pSearch, params[2])
	
	CBaseEntity* pActivator;
	CELLT_TO_ENTITY(pActivator, params[3])

	CBaseEntity* pCaller;
	CELLT_TO_ENTITY(pCaller, params[4])

	char* name;
	pContext->LocalToString(params[1], &name);

	CBaseEntity* pEntity = servertools->FindEntityProcedural(name, pSearch, pActivator, pCaller);
	if (!pEntity)
	{
		return -1;
	}
	return gamehelpers->EntityToBCompatRef(pEntity);
}

cell_t ST_FindEntityClassNearestFacing(IPluginContext* pContext, const cell_t* params)
{
	cell_t* org;
	pContext->LocalToPhysAddr(params[1], &org);
	Vector vecOrigin(sp_ctof(org[0]), sp_ctof(org[1]), sp_ctof(org[2]));

	cell_t* fac;
	pContext->LocalToPhysAddr(params[2], &fac);
	Vector vecFacing(sp_ctof(fac[0]), sp_ctof(fac[1]), sp_ctof(fac[2]));

	char* name;
	pContext->LocalToString(params[4], &name);

	CBaseEntity* pEntity = servertools->FindEntityClassNearestFacing(vecOrigin, vecFacing, sp_ctof(params[3]), name);
	if (!pEntity)
	{
		return -1;
	}
	return gamehelpers->EntityToBCompatRef(pEntity);
}

cell_t ST_FindEntityNearestFacing(IPluginContext* pContext, const cell_t* params)
{
	cell_t* org;
	pContext->LocalToPhysAddr(params[1], &org);
	Vector vecOrigin(sp_ctof(org[0]), sp_ctof(org[1]), sp_ctof(org[2]));

	cell_t* fac;
	pContext->LocalToPhysAddr(params[2], &fac);
	Vector vecFacing(sp_ctof(fac[0]), sp_ctof(fac[1]), sp_ctof(fac[2]));

	CBaseEntity* pEntity = servertools->FindEntityNearestFacing(vecOrigin, vecFacing, sp_ctof(params[3]));
	if (!pEntity)
	{
		return -1;
	}
	return gamehelpers->EntityToBCompatRef(pEntity);
}

cell_t ST_FindEntityByClassnameWithin(IPluginContext* pContext, const cell_t* params)
{
	CBaseEntity* pStart;
	CELLT_TO_ENTITY(pStart, params[1]);

	char* name;
	pContext->LocalToString(params[2], &name);

	cell_t* fac;
	pContext->LocalToPhysAddr(params[3], &fac);
	Vector vecOrigin(sp_ctof(fac[0]), sp_ctof(fac[1]), sp_ctof(fac[2]));

	CBaseEntity* pEntity = servertools->FindEntityByClassnameWithin(pStart, name, vecOrigin, sp_ctof(params[4]));
	if (!pEntity)
	{
		return -1;
	}
	return gamehelpers->EntityToBCompatRef(pEntity);
}

cell_t ST_FindEntityByClassnameNearest(IPluginContext* pContext, const cell_t* params)
{
	char* name;
	pContext->LocalToString(params[1], &name);

	cell_t* fac;
	pContext->LocalToPhysAddr(params[2], &fac);
	Vector vecOrigin(sp_ctof(fac[0]), sp_ctof(fac[1]), sp_ctof(fac[2]));

	CBaseEntity* pEntity = servertools->FindEntityByClassnameNearest(name, vecOrigin, sp_ctof(params[3]));
	if (!pEntity)
	{
		return -1;
	}
	return gamehelpers->EntityToBCompatRef(pEntity);
}

cell_t ST_FindEntityByModel(IPluginContext* pContext, const cell_t* params)
{
	CBaseEntity* pStart;
	CELLT_TO_ENTITY(pStart, params[1]);

	char* model;
	pContext->LocalToString(params[2], &model);

	CBaseEntity* pEntity = servertools->FindEntityByModel(pStart, model);
	if (!pEntity)
	{
		return -1;
	}
	return gamehelpers->EntityToBCompatRef(pEntity);
}

cell_t ST_FindEntityInSphere(IPluginContext* pContext, const cell_t* params)
{
	CBaseEntity* pStart;
	CELLT_TO_ENTITY(pStart, params[1]);

	cell_t* fac;
	pContext->LocalToPhysAddr(params[2], &fac);
	Vector vecOrigin(sp_ctof(fac[0]), sp_ctof(fac[1]), sp_ctof(fac[2]));

	CBaseEntity* pEntity = servertools->FindEntityInSphere(pStart, vecOrigin, sp_ctof(params[3]));
	if (!pEntity)
	{
		return -1;
	}
	return gamehelpers->EntityToBCompatRef(pEntity);
}

cell_t ST_FirstEntity(IPluginContext* pContext, const cell_t* params)
{
	CBaseEntity* pFirst = servertools->FirstEntity();
	if (!pFirst)
	{
		return -1;	//???
	}
	return gamehelpers->EntityToBCompatRef(pFirst);
}

cell_t ST_NextEntity(IPluginContext* pContext, const cell_t* params)
{
	CBaseEntity* pEntity;
	CELLT_TO_ENTITY(pEntity, params[1])

	CBaseEntity* pNext = servertools->NextEntity(pEntity);
	if (!pNext)
	{
		return -1;
	}
	return gamehelpers->EntityToBCompatRef(pNext);
}

cell_t ST_FindEntityByHammerID(IPluginContext* pContext, const cell_t* params)
{
	CBaseEntity* pEntity = servertools->FindEntityByHammerID(params[1]);
	if (!pEntity)
	{
		return -1;
	}
	return gamehelpers->EntityToBCompatRef(pEntity);
}

cell_t ST_GetBaseEntity(IPluginContext* pContext, const cell_t* params)
{
	CBaseEntity* pEntity = reinterpret_cast<CBaseEntity*>(params[1]);
	if (!pEntity)
	{
		return pContext->ThrowNativeError("Entity pointer cannot be NULL");
	}
	else if (reinterpret_cast<intptr_t>(pEntity) < 0x10000)
	{
		return pContext->ThrowNativeError("Address: 0x%x is poiting to a reserved memory.", pEntity);
	}

	return gamehelpers->EntityToBCompatRef(pEntity);
}


cell_t ST_ResetSequence(IPluginContext* pContext, const cell_t* params)
{
	CBaseAnimating* pEntity = (CBaseAnimating*)gamehelpers->ReferenceToEntity(params[1]);
	if (!pEntity)
	{
		return pContext->ThrowNativeError("Invalid entity index: %i", params[1]);
	}

	servertools->ResetSequence(pEntity, params[2]);
	return 1;
}

cell_t ST_ResetSequenceInfo(IPluginContext* pContext, const cell_t* params)
{
	CBaseAnimating* pEntity = (CBaseAnimating*)gamehelpers->ReferenceToEntity(params[1]);
	if (!pEntity)
	{
		return pContext->ThrowNativeError("Invalid entity index: %i", params[1]);
	}

	servertools->ResetSequenceInfo(pEntity);
	return 1;
}


cell_t ST_GetKeyValue(IPluginContext* pContext, const cell_t* params)
{
	CBaseEntity* pEntity = gamehelpers->ReferenceToEntity(params[1]);
	if (!pEntity)
	{
		return pContext->ThrowNativeError("Invalid entity index: %i", params[2]);
	}

	char* name;
	pContext->LocalToString(params[2], &name);
	
	char res[64];
	if (servertools->GetKeyValue(pEntity, name, res, sizeof(res)))
	{
		pContext->StringToLocal(params[3], params[4], res);
		return true;
	}
	return false;
}

cell_t ST_SetKeyValue(IPluginContext* pContext, const cell_t* params)
{
	CBaseEntity* pEntity = gamehelpers->ReferenceToEntity(params[1]);
	if (!pEntity)
	{
		return pContext->ThrowNativeError("Invalid entity index: %i", params[2]);
	}

	char* name;
	pContext->LocalToString(params[2], &name);
	char* val;
	pContext->LocalToString(params[3], &val);

	return servertools->SetKeyValue(pEntity, name, val);
}

cell_t ST_SetKeyValueFloat(IPluginContext* pContext, const cell_t* params)
{
	CBaseEntity* pEntity = gamehelpers->ReferenceToEntity(params[1]);
	if (!pEntity)
	{
		return pContext->ThrowNativeError("Invalid entity index: %i", params[2]);
	}

	char* name;
	pContext->LocalToString(params[2], &name);

	return servertools->SetKeyValue(pEntity, name, sp_ctof(params[3]));
}

cell_t ST_SetKeyValueVector(IPluginContext* pContext, const cell_t* params)
{
	CBaseEntity* pEntity = gamehelpers->ReferenceToEntity(params[1]);
	if (!pEntity)
	{
		return pContext->ThrowNativeError("Invalid entity index: %i", params[2]);
	}

	char* name;
	pContext->LocalToString(params[2], &name);
	cell_t* v;
	pContext->LocalToPhysAddr(params[3], &v);
	Vector vec(sp_ctof(v[0]), sp_ctof(v[1]), sp_ctof(v[2]));

	return servertools->SetKeyValue(pEntity, name, vec);
}