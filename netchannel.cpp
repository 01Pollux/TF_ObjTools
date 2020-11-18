#include <inetchannel.h>
#include "extension.h"

#define DECL_NATIVE_CALLBACK(NAME)		cell_t NC_##NAME(IPluginContext* pContext, const cell_t* params)
#define DECL_NATIVE_CALLBACK_GET(NAME)	cell_t NC_##NAME##_GET(IPluginContext* pContext, const cell_t* params)
#define DECL_NATIVE_CALLBACK_SET(NAME)	cell_t NC_##NAME##_SET(IPluginContext* pContext, const cell_t* params)


class _NetChannelNatives : public IGlobalHooks
{
public:
public:	//	IGlobalHooks
	bool OnLoad(char*, size_t) override;

public:
	INetChannel* GetChannelFromCell(IPluginContext*, cell_t);
} static smnet_channel;

static void RegNatives();

bool _NetChannelNatives::OnLoad(char*, size_t)
{
	RegNatives();
	return true;
}



inline INetChannel* _NetChannelNatives::GetChannelFromCell(IPluginContext* pContext, cell_t param)
{
	if (!param)
	{
		pContext->ThrowNativeError("NetChannel %x is NULL!", param);
		return nullptr;
	}

	return reinterpret_cast<INetChannel*>(param);
}


DECL_NATIVE_CALLBACK(GetNetInfo)
{
	cell_t client = params[1];

	IGamePlayer* pPlayer = playerhelpers->GetGamePlayer(client);
	if (!pPlayer || !pPlayer->IsInGame())
		return NULL;

	INetChannel* pNet = reinterpret_cast<INetChannel*>(engine->GetPlayerNetInfo(client));

	return reinterpret_cast<cell_t>(pNet);
}

DECL_NATIVE_CALLBACK_GET(DataRate)
{
	INetChannel* pNet = smnet_channel.GetChannelFromCell(pContext, params[1]);
	if (!pNet)
		return NULL;

	return pNet->GetDataRate();
}

DECL_NATIVE_CALLBACK_SET(DataRate)
{
	INetChannel* pNet = smnet_channel.GetChannelFromCell(pContext, params[1]);
	if (!pNet)
		return NULL;

	pNet->SetDataRate(params[2]);
	return 1;
}

DECL_NATIVE_CALLBACK_SET(TimeOut)
{
	INetChannel* pNet = smnet_channel.GetChannelFromCell(pContext, params[1]);
	if (!pNet)
		return NULL;

	pNet->SetTimeout(sp_ctof(params[2]));
	return 1;
}

DECL_NATIVE_CALLBACK_GET(BufferSize)
{
	INetChannel* pNet = smnet_channel.GetChannelFromCell(pContext, params[1]);
	if (!pNet)
		return -1;

	return pNet->GetBufferSize();
}

DECL_NATIVE_CALLBACK(Address)
{
	INetChannel* pNet = smnet_channel.GetChannelFromCell(pContext, params[1]);
	if (!pNet)
		return NULL;

	pContext->StringToLocal(params[2], params[3], pNet->GetAddress());
	return 1;
}

DECL_NATIVE_CALLBACK_GET(DropNumber)
{
	INetChannel* pNet = smnet_channel.GetChannelFromCell(pContext, params[1]);
	if (!pNet)
		return NULL;

	return pNet->GetDropNumber();
}

DECL_NATIVE_CALLBACK(AvgChoke)
{
	INetChannel* pNet = smnet_channel.GetChannelFromCell(pContext, params[1]);
	if (!pNet)
		return NULL;

	return pNet->GetAvgChoke(params[2]);
}

DECL_NATIVE_CALLBACK(AvgData)
{
	INetChannel* pNet = smnet_channel.GetChannelFromCell(pContext, params[1]);
	if (!pNet)
		return NULL;

	return pNet->GetAvgData(params[2]);
}

DECL_NATIVE_CALLBACK(AvgLatency)
{
	INetChannel* pNet = smnet_channel.GetChannelFromCell(pContext, params[1]);
	if (!pNet)
		return NULL;

	return pNet->GetAvgLatency(params[2]);
}

DECL_NATIVE_CALLBACK(AvgLoss)
{
	INetChannel* pNet = smnet_channel.GetChannelFromCell(pContext, params[1]);
	if (!pNet)
		return NULL;

	return pNet->GetAvgLoss(params[2]);
}

DECL_NATIVE_CALLBACK(AvgPackets)
{
	INetChannel* pNet = smnet_channel.GetChannelFromCell(pContext, params[1]);
	if (!pNet)
		return NULL;

	return pNet->GetAvgPackets(params[2]);
}

DECL_NATIVE_CALLBACK(SetMaxBufferSize)
{
	INetChannel* pNet = smnet_channel.GetChannelFromCell(pContext, params[1]);
	if (!pNet)
		return NULL;

	pNet->SetMaxBufferSize(params[2] ? true:false, params[3], params[4] ? true:false);
	return 1;
}

DECL_NATIVE_CALLBACK_GET(Time)
{
	INetChannel* pNet = smnet_channel.GetChannelFromCell(pContext, params[1]);
	if (!pNet)
		return NULL;

	return sp_ftoc(pNet->GetTime());
}

DECL_NATIVE_CALLBACK_GET(TimeConnected)
{
	INetChannel* pNet = smnet_channel.GetChannelFromCell(pContext, params[1]);
	if (!pNet)
		return NULL;

	return sp_ftoc(pNet->GetTimeConnected());
}

DECL_NATIVE_CALLBACK(GetPacketTime)
{
	INetChannel* pNet = smnet_channel.GetChannelFromCell(pContext, params[1]);
	if (!pNet)
		return NULL;

	return sp_ftoc(pNet->GetPacketTime(params[2], params[3]));
}

DECL_NATIVE_CALLBACK_GET(IsOverflowed)
{
	INetChannel* pNet = smnet_channel.GetChannelFromCell(pContext, params[1]);
	if (!pNet)
		return NULL;

	return pNet->IsOverflowed() ? 1 : 0;
}

DECL_NATIVE_CALLBACK_GET(IsLoopback)
{
	INetChannel* pNet = smnet_channel.GetChannelFromCell(pContext, params[1]);
	if (!pNet)
		return NULL;

	return pNet->IsLoopback() ? 1 : 0;
}

DECL_NATIVE_CALLBACK_GET(IsNull)
{
	INetChannel* pNet = smnet_channel.GetChannelFromCell(pContext, params[1]);
	if (!pNet)
		return NULL;

	return pNet->IsNull() ? 1 : 0;
}

DECL_NATIVE_CALLBACK_GET(IsTimingOut)
{
	INetChannel* pNet = smnet_channel.GetChannelFromCell(pContext, params[1]);
	if (!pNet)
		return NULL;

	return pNet->IsTimingOut() ? 1 : 0;
}

DECL_NATIVE_CALLBACK_GET(MsgHandler)
{
	INetChannel* pNet = smnet_channel.GetChannelFromCell(pContext, params[1]);
	if (!pNet)
		return NULL;

	return reinterpret_cast<cell_t>(pNet->GetMsgHandler());
}

DECL_NATIVE_CALLBACK(IsValidPacket)
{
	INetChannel* pNet = smnet_channel.GetChannelFromCell(pContext, params[1]);
	if (!pNet)
		return NULL;

	return pNet->IsValidPacket(params[2], params[3]) ? 1 : 0;
}

DECL_NATIVE_CALLBACK(SendData)
{
	INetChannel* pNet = smnet_channel.GetChannelFromCell(pContext, params[1]);
	if (!pNet)
		return NULL;
	
	bf_write& buf = reinterpret_cast<bf_write&>(const_cast<cell_t*>(params)[2]);
	return pNet->SendData(buf, params[3] ? true : false) ? 1 : 0;
}

DECL_NATIVE_CALLBACK(Reset)
{
	INetChannel* pNet = smnet_channel.GetChannelFromCell(pContext, params[1]);
	if (!pNet)
		return NULL;
	
	pNet->Reset();
	return 1;
}


#define DECL_NATIVE(NAME)		{ "NetChannel."###NAME,			NC_##NAME }
#define DECL_NATIVE_GET(NAME)	{ "NetChannel."###NAME##".get", NC_##NAME##_GET }
#define DECL_NATIVE_SET(NAME)	{ "NetChannel."###NAME##".set", NC_##NAME##_SET }

const sp_nativeinfo_t nc_natives[] = {
	DECL_NATIVE(GetNetInfo),
	DECL_NATIVE_GET(DataRate),
	DECL_NATIVE_SET(DataRate),
	DECL_NATIVE_SET(TimeOut),
	DECL_NATIVE_GET(BufferSize),
	DECL_NATIVE(Address),
	DECL_NATIVE_GET(DropNumber),
	DECL_NATIVE(AvgChoke),
	DECL_NATIVE(AvgData),
	DECL_NATIVE(AvgLatency),
	DECL_NATIVE(AvgLoss),
	DECL_NATIVE(AvgPackets),
	DECL_NATIVE(SetMaxBufferSize),
	DECL_NATIVE_GET(Time),
	DECL_NATIVE_GET(TimeConnected),
	DECL_NATIVE(GetPacketTime),
	DECL_NATIVE_GET(IsOverflowed),
	DECL_NATIVE_GET(IsLoopback),
	DECL_NATIVE_GET(IsNull),
	DECL_NATIVE_GET(IsTimingOut),
	DECL_NATIVE_GET(MsgHandler),
	DECL_NATIVE(IsValidPacket),
	DECL_NATIVE(SendData),
	DECL_NATIVE(Reset),
//	{ "HookNetChannel", NC_HookNetChannel },
//	{ "UnhookNetChannel", NC_UnhookNetChannel },
	{NULL, NULL},
};

#undef DECL_NATIVE
#undef DECL_NATIVE_SET
#undef DECL_NATIVE_GET
#undef DECL_NATIVE_CALLBACK
#undef DECL_NATIVE_CALLBACK_GET
#undef DECL_NATIVE_CALLBACK_SET

inline void RegNatives()
{
	sharesys->AddNatives(myself, nc_natives);
}