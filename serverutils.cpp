#include "extension.h"
#include <toolframework/itoolentity.h>

#define DECL_NATIVE_CALLBACK(NAME)		cell_t ST_##NAME(IPluginContext* pContext, const cell_t* params)

void RegNatives();

extern IServerTools* servertools;

class _ServerToolsNatives: public IGlobalHooks
{
public:
public:	//	IGlobalHooks
	bool OnLoad(char*, size_t) override;
} _server_tools;


bool _ServerToolsNatives::OnLoad(char*, size_t)
{
	RegNatives();
	return true;
}


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


DECL_NATIVE_CALLBACK(FindEntityProcedural)
{
	CBaseEntity* pSearch;
	CELLT_TO_ENTITY(pSearch, params[2]);

	CBaseEntity* pActivator;
	CELLT_TO_ENTITY(pActivator, params[3]);

	CBaseEntity* pCaller;
	CELLT_TO_ENTITY(pCaller, params[4]);

		char* name;
	pContext->LocalToString(params[1], &name);

	CBaseEntity* pEntity = servertools->FindEntityProcedural(name, pSearch, pActivator, pCaller);
	if (!pEntity)
		return -1;
	return gamehelpers->EntityToBCompatRef(pEntity);
}

DECL_NATIVE_CALLBACK(FindEntityClassNearestFacing)
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
		return -1;

	return gamehelpers->EntityToBCompatRef(pEntity);
}

DECL_NATIVE_CALLBACK(FindEntityNearestFacing)
{
	cell_t* org;
	pContext->LocalToPhysAddr(params[1], &org);
	Vector vecOrigin(sp_ctof(org[0]), sp_ctof(org[1]), sp_ctof(org[2]));

	cell_t* fac;
	pContext->LocalToPhysAddr(params[2], &fac);
	Vector vecFacing(sp_ctof(fac[0]), sp_ctof(fac[1]), sp_ctof(fac[2]));

	CBaseEntity* pEntity = servertools->FindEntityNearestFacing(vecOrigin, vecFacing, sp_ctof(params[3]));
	if (!pEntity)
		return -1;

	return gamehelpers->EntityToBCompatRef(pEntity);
}

DECL_NATIVE_CALLBACK(FindEntityByClassnameWithin)
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
		return -1;

	return gamehelpers->EntityToBCompatRef(pEntity);
}

DECL_NATIVE_CALLBACK(FindEntityByClassnameNearest)
{
	char* name;
	pContext->LocalToString(params[1], &name);

	cell_t* fac;
	pContext->LocalToPhysAddr(params[2], &fac);
	Vector vecOrigin(sp_ctof(fac[0]), sp_ctof(fac[1]), sp_ctof(fac[2]));

	CBaseEntity* pEntity = servertools->FindEntityByClassnameNearest(name, vecOrigin, sp_ctof(params[3]));
	if (!pEntity)
		return -1;

	return gamehelpers->EntityToBCompatRef(pEntity);
}

DECL_NATIVE_CALLBACK(FindEntityByModel)
{
	CBaseEntity* pStart;
	CELLT_TO_ENTITY(pStart, params[1]);

	char* model;
	pContext->LocalToString(params[2], &model);

	CBaseEntity* pEntity = servertools->FindEntityByModel(pStart, model);
	if (!pEntity)
		return -1;

	return gamehelpers->EntityToBCompatRef(pEntity);
}

DECL_NATIVE_CALLBACK(FindEntityInSphere)
{
	CBaseEntity* pStart;
	CELLT_TO_ENTITY(pStart, params[1]);

	cell_t* fac;
	pContext->LocalToPhysAddr(params[2], &fac);
	Vector vecOrigin(sp_ctof(fac[0]), sp_ctof(fac[1]), sp_ctof(fac[2]));

	CBaseEntity* pEntity = servertools->FindEntityInSphere(pStart, vecOrigin, sp_ctof(params[3]));
	if (!pEntity)
		return -1;

	return gamehelpers->EntityToBCompatRef(pEntity);
}

DECL_NATIVE_CALLBACK(FirstEntity)
{
	CBaseEntity* pFirst = servertools->FirstEntity();
	if (!pFirst)
		return -1;

	return gamehelpers->EntityToBCompatRef(pFirst);
}

DECL_NATIVE_CALLBACK(NextEntity)
{
	CBaseEntity* pEntity;
	CELLT_TO_ENTITY(pEntity, params[1]);

	CBaseEntity* pNext = servertools->NextEntity(pEntity);
	if (!pNext)
		return -1;

	return gamehelpers->EntityToBCompatRef(pNext);
}

DECL_NATIVE_CALLBACK(FindEntityByHammerID)
{
	CBaseEntity* pEntity = servertools->FindEntityByHammerID(params[1]);
	if (!pEntity)
		return -1;

	return gamehelpers->EntityToBCompatRef(pEntity);
}

DECL_NATIVE_CALLBACK(GetBaseEntity)
{
	CBaseEntity* pEntity = reinterpret_cast<CBaseEntity*>(params[1]);
	if (!pEntity || reinterpret_cast<intptr_t>(pEntity) < 0x10000)
		return pContext->ThrowNativeError("Invalid Entity Pointer: %p", params[1]);

	return gamehelpers->EntityToBCompatRef(pEntity);
}


DECL_NATIVE_CALLBACK(ResetSequence)
{
	CBaseAnimating* pEntity = reinterpret_cast<CBaseAnimating*>(gamehelpers->ReferenceToEntity(params[1]));
	if (!pEntity)
		return pContext->ThrowNativeError("Invalid entity index: %i", params[1]);

	servertools->ResetSequence(pEntity, params[2]);
	return 1;
}

DECL_NATIVE_CALLBACK(ResetSequenceInfo)
{
	CBaseAnimating* pEntity = reinterpret_cast<CBaseAnimating*>(gamehelpers->ReferenceToEntity(params[1]));
	if (!pEntity)
		return pContext->ThrowNativeError("Invalid entity index: %i", params[1]);

	servertools->ResetSequenceInfo(pEntity);
	return 1;
}


DECL_NATIVE_CALLBACK(GetKeyValue)
{
	CBaseEntity* pEntity = gamehelpers->ReferenceToEntity(params[1]);
	if (!pEntity)
		return pContext->ThrowNativeError("Invalid entity index: %i", params[1]);

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

DECL_NATIVE_CALLBACK(SetKeyValue)
{
	CBaseEntity* pEntity = gamehelpers->ReferenceToEntity(params[1]);
	if (!pEntity)
		return pContext->ThrowNativeError("Invalid entity index: %i", params[1]);

	char* name;
	pContext->LocalToString(params[2], &name);
	char* val;
	pContext->LocalToString(params[3], &val);

	return servertools->SetKeyValue(pEntity, name, val);
}

DECL_NATIVE_CALLBACK(SetKeyValueFloat)
{
	CBaseEntity* pEntity = gamehelpers->ReferenceToEntity(params[1]);
	if (!pEntity)
		return pContext->ThrowNativeError("Invalid entity index: %i", params[1]);

	char* name;
	pContext->LocalToString(params[2], &name);

	return servertools->SetKeyValue(pEntity, name, sp_ctof(params[3]));
}

DECL_NATIVE_CALLBACK(SetKeyValueVector)
{
	CBaseEntity* pEntity = gamehelpers->ReferenceToEntity(params[1]);
	if (!pEntity)
		return pContext->ThrowNativeError("Invalid entity index: %i", params[1]);

	char* name;
	pContext->LocalToString(params[2], &name);
	cell_t* v;
	pContext->LocalToPhysAddr(params[3], &v);
	Vector vec(sp_ctof(v[0]), sp_ctof(v[1]), sp_ctof(v[2]));

	return servertools->SetKeyValue(pEntity, name, vec);
}

DECL_NATIVE_CALLBACK(IsEntityPtr)
{
	void* ptr = reinterpret_cast<void*>(params[1]);
	if (!ptr || reinterpret_cast<uintptr_t>(ptr) < 0x10000)
		return pContext->ThrowNativeError("Invalid Entity Pointer: %p", params[1]);

	return servertools->IsEntityPtr(ptr) ? 1 : 0;
}


#define DECL_NATIVE(NAME)		{ "ServerTools."###NAME,		 ST_##NAME }
#define DECL_NATIVE_GET(NAME)	{ "ServerTools."###NAME##".get", ST_##NAME##_GET }
#define DECL_NATIVE_SET(NAME)	{ "ServerTools."###NAME##".set", ST_##NAME##_SET }

static const sp_nativeinfo_t st_natives[] = {
	DECL_NATIVE(FindEntityProcedural),
	DECL_NATIVE(FindEntityClassNearestFacing),
	DECL_NATIVE(FindEntityNearestFacing),
	DECL_NATIVE(FindEntityByClassnameWithin),
	DECL_NATIVE(FindEntityByClassnameNearest),
	DECL_NATIVE(FindEntityByModel),
	DECL_NATIVE(FindEntityInSphere),
	DECL_NATIVE(FindEntityByHammerID),

	DECL_NATIVE(FirstEntity),
	DECL_NATIVE(NextEntity),
	DECL_NATIVE(GetBaseEntity),

	DECL_NATIVE(ResetSequence),
	DECL_NATIVE(ResetSequenceInfo),

	DECL_NATIVE(GetKeyValue),
	DECL_NATIVE(SetKeyValue),
	DECL_NATIVE(SetKeyValueFloat),
	DECL_NATIVE(SetKeyValueVector),
	DECL_NATIVE(IsEntityPtr),

	{NULL, NULL},
};

#undef DECL_NATIVE
#undef DECL_NATIVE_SET
#undef DECL_NATIVE_GET
#undef DECL_NATIVE_CALLBACK


inline void RegNatives()
{
	sharesys->AddNatives(myself, st_natives);
}