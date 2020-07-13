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
	{"ServerTools.FindEntityProcedural", ST_FindEntityProcedural},
	{"ServerTools.FindEntityClassNearestFacing", ST_FindEntityClassNearestFacing},
	{"ServerTools.FindEntityNearestFacing", ST_FindEntityNearestFacing},
	{"ServerTools.FindEntityByClassnameWithin", ST_FindEntityByClassnameWithin},
	{"ServerTools.FindEntityByClassnameNearest", ST_FindEntityByClassnameNearest},
	{"ServerTools.FindEntityByModel", ST_FindEntityByModel},
	{"ServerTools.FindEntityInSphere", ST_FindEntityInSphere},

	{"ServerTools.FirstEntity", ST_FirstEntity},
	{"ServerTools.NextEntity", ST_NextEntity},
	{"ServerTools.FindEntityByHammerID", ST_FindEntityByHammerID},
	{"ServerTools.GetBaseEntity", ST_GetBaseEntity},

	{"ServerTools.ResetSequence", ST_ResetSequence},
	{"ServerTools.ResetSequenceInfo", ST_ResetSequenceInfo},

	{"ServerTools.GetKeyValue", ST_GetKeyValue},
	{"ServerTools.SetKeyValue", ST_SetKeyValue},
	{"ServerTools.SetKeyValueFloat", ST_SetKeyValueFloat},
	{"ServerTools.SetKeyValueVector", ST_SetKeyValueVector},
	{NULL, NULL},
};
