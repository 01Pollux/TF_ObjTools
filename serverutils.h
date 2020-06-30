#pragma once
#include "extension.h"
#include <toolframework/itoolentity.h>

extern IServerTools* servertools;

cell_t ST_FindEntityProcedural(IPluginContext* pContext, const cell_t* params);
cell_t ST_FindEntityClassNearestFacing(IPluginContext* pContext, const cell_t* params);
cell_t ST_FindEntityNearestFacing(IPluginContext* pContext, const cell_t* params);
cell_t ST_FindEntityByClassnameWithin(IPluginContext* pContext, const cell_t* params);
cell_t ST_FindEntityByClassnameNearest(IPluginContext* pContext, const cell_t* params);
cell_t ST_FindEntityByModel(IPluginContext* pContext, const cell_t* params);
cell_t ST_FindEntityInSphere(IPluginContext* pContext, const cell_t* params);

cell_t ST_FirstEntity(IPluginContext* pContext, const cell_t* params);
cell_t ST_NextEntity(IPluginContext* pContext, const cell_t* params);
cell_t ST_FindEntityByHammerID(IPluginContext* pContext, const cell_t* params);
cell_t ST_GetBaseEntity(IPluginContext* pContext, const cell_t* params);

cell_t ST_ResetSequence(IPluginContext* pContext, const cell_t* params);
cell_t ST_ResetSequenceInfo(IPluginContext* pContext, const cell_t* params);

cell_t ST_GetKeyValue(IPluginContext* pContext, const cell_t* params);
cell_t ST_SetKeyValue(IPluginContext* pContext, const cell_t* params);
cell_t ST_SetKeyValueFloat(IPluginContext* pContext, const cell_t* params);
cell_t ST_SetKeyValueVector(IPluginContext* pContext, const cell_t* params);

const sp_nativeinfo_t g_ServerNatives[] = {
	{"ST_FindEntityProcedural", ST_FindEntityProcedural},
	{"ST_FindEntityClassNearestFacing", ST_FindEntityClassNearestFacing},
	{"ST_FindEntityNearestFacing", ST_FindEntityNearestFacing},
	{"ST_FindEntityByClassnameWithin", ST_FindEntityByClassnameWithin},
	{"ST_FindEntityByClassnameNearest", ST_FindEntityByClassnameNearest},
	{"ST_FindEntityByModel", ST_FindEntityByModel},
	{"ST_FindEntityInSphere", ST_FindEntityInSphere},

	{"ST_FirstEntity", ST_FirstEntity},
	{"ST_NextEntity", ST_NextEntity},
	{"ST_FindEntityByHammerID", ST_FindEntityByHammerID},
	{"ST_GetBaseEntity", ST_GetBaseEntity},

	{"ST_ResetSequence", ST_ResetSequence},
	{"ST_ResetSequenceInfo", ST_ResetSequenceInfo},

	{"ST_GetKeyValue", ST_GetKeyValue},
	{"ST_SetKeyValue", ST_SetKeyValue},
	{"ST_SetKeyValueFloat", ST_SetKeyValueFloat},
	{"ST_SetKeyValueVector", ST_SetKeyValueVector},
	{NULL, NULL},
};