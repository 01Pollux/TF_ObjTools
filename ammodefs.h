#pragma once

#include <tier1/utlvector.h>
#include <mathlib/vector.h>
#include <ammodef.h>
#include "extension.h"

cell_t CAmmoDefs_FindIndex(IPluginContext* pContext, const cell_t* params);
cell_t CAmmoDefs_GetName(IPluginContext* pContext, const cell_t* params);
cell_t CAmmoDefs_GetDamageType(IPluginContext* pContext, const cell_t* params);
cell_t CAmmoDefs_GeteTracerType(IPluginContext* pContext, const cell_t* params);
cell_t CAmmoDefs_GetDamageForce(IPluginContext* pContext, const cell_t* params);
cell_t CAmmoDefs_GetFlags(IPluginContext* pContext, const cell_t* params);
cell_t CAmmoDefs_GetMaxCarry(IPluginContext* pContext, const cell_t* params);
cell_t CAmmoDefs_GetRaw(IPluginContext* pContext, const cell_t* params);

const sp_nativeinfo_t g_AmmoNatives[] = {
	{"CAmmoDefs.FindIndex", CAmmoDefs_FindIndex},
	{"CAmmoDefs.GetName", CAmmoDefs_GetName},
	{"CAmmoDefs.DamageType", CAmmoDefs_GetDamageType},
	{"CAmmoDefs.eTracerType", CAmmoDefs_GeteTracerType},
	{"CAmmoDefs.DamageForce", CAmmoDefs_GetDamageForce},
	{"CAmmoDefs.GetFlags", CAmmoDefs_GetFlags},
	{"CAmmoDefs.GetMaxCarry", CAmmoDefs_GetMaxCarry},
	{"CAmmoDefs.Raw", CAmmoDefs_GetRaw},
	{NULL, NULL},
};
