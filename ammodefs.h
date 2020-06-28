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
	{"CAmmoDefs_FindIndex", CAmmoDefs_FindIndex},
	{"CAmmoDefs_GetName", CAmmoDefs_GetName},
	{"CAmmoDefs_DamageType", CAmmoDefs_GetDamageType},
	{"CAmmoDefs_eTracerType", CAmmoDefs_GeteTracerType},
	{"CAmmoDefs_DamageForce", CAmmoDefs_GetDamageForce},
	{"CAmmoDefs_GetFlags", CAmmoDefs_GetFlags},
	{"CAmmoDefs_GetMaxCarry", CAmmoDefs_GetMaxCarry},
	{"CAmmoDefs_Raw", CAmmoDefs_GetRaw},
	{NULL, NULL},
};