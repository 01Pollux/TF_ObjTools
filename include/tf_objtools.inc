/*
**
*/
#if defined _TF_OBJTOOLS_included
 #endinput
#endif
#define _TF_OBJTOOLS_included

enum CTakeDamageOffset {
	vecReportedPosition,
	
	hInflictor,
	hAttacker,
	hWeapon,
	
	flDamage,
	flMaxDamage,
	flBaseDamage,
	bitsDamageType,

	vecDamageForce = 0,
	vecDamagePosition,
	iDamageCustom,
	iDamageStats,
	iAmmoType,
	iDamagedOtherPlayers,
	iPlayerPenetrationCount,
	flDamageBonus,
	
	hDamageBonusProvider,
	
	bForceFriendlyFire,
	flDamageForForce,
	eCritType
};

enum {
	kCritType_None,
	kCritType_MiniCrit,
	kCritType_Crit
};

methodmap CTakeDamageInfo < Handle {
	/**
	 * Creates a CTakeDamageInfo object handle
	 * Must be freed.
	 */
	public native CTakeDamageInfo();
	
	/**
	 * use the correct CTakeDamageOffset to access object's data
	 */
	 
	/**
	 * Reads an int/bool/eCritType value inside the object
	 */
	public native int ReadInt(CTakeDamageOffset offset);
	/**
	 * Stores an int/bool/eCritType value inside the object
	 */
	public native void StoreInt(CTakeDamageOffset offset, int val);
	
	/**
	 * Reads a float value inside the object
	 */
	public native float ReadFloat(CTakeDamageOffset offset);
	/**
	 * Stores a float value inside the object
	 */
	public native void StoreFloat(CTakeDamageOffset offset, float val);
	
	/**
	 * Reads a vector array inside the object
	 */
	public native void ReadVector(CTakeDamageOffset offset, float result[3]);
	/**
	 * Stores a vector array inside the object
	 */
	public native void StoreVector(CTakeDamageOffset offset, const float val[3]);
	
	/**
	 * Reads a CBaseHandle entity inside the object
	 */
	public native int ReadEnt(CTakeDamageOffset offset);
	/**
	 * Stores a CBaseHandle entity inside the object
	 */
	public native void StoreEnt(CTakeDamageOffset offset, int val);
	
	/**
	 * Fires CTFGameRules::DeathNotice event
	 *
	 * @param client: victim index
	 */
	public native void DeathNotice(int client);
	
	/**
	 * The address to use as CTakeDamageInfo object
	 */
	property Address Infos {
		public native get();
		public native set(Address addr);
	}
	
	/**
	 * Calculate Explosive damage force and store it in CTakeDamageInfo object
	 */
	public native bool CalcExplosiveDmgForce(float vecPosition[3], float vecForce[3]);
	
	/**
	 * Calculate bullet damage force and store it in CTakeDamageInfo object
	 */
	public native bool CalcBulletDamageForce(int ammoidx, float vecDirection[3], float vecForce[3], float scale);
	
	/**
	 * Calculate melee damage force and store it in CTakeDamageInfo object
	 */
	public native bool CalcMeleeDamageForce(float vecPosition[3], float vecForce[3], float scale);
	
	/**
	 * Guesses damage force and store it in CTakeDamageInfo object
	 */
	public void GuessDamageForce(int ammoidx, float vecDirection[3], float vecForce[3], float scale)
	{
		int bits = this.ReadInt(bitsDamageType)
		if (bits & (1 << 1)) {
			this.CalcBulletDamageForce(ammoidx, vecDirection, vecForce, scale);
		}
		else if (bits & (1 << 6)) {
			this.CalcExplosiveDmgForce(vecDirection, vecForce);
		}
		else {
			this.CalcMeleeDamageForce(vecDirection, vecForce, scale);
		}
	}
}

typeset CTakeDmgInfoCallback
{
	function Action(int victim, CTakeDamageInfo& infos);
	function void(int victim, CTakeDamageInfo infos);
}

enum CTakeDmgInfoType
{
	GenericPre,
	AlivePre,
	GenericPost,
	AlivePost
};

native void HookRawOnTakeDamage(int client, CTakeDmgInfoCallback callback, CTakeDmgInfoType type);
native void UnhookRawOnTakeDamage(int client, CTakeDmgInfoCallback callback, CTakeDmgInfoType type);

enum AmmoFlags_t
{
	AMMO_FORCE_DROP_IF_CARRIED = 0x1,
	AMMO_INTERPRET_PLRDAMAGE_AS_DAMAGE_TO_PLAYER = 0x2
};

methodmap CAmmoDefs
{
	public static native int FindIndex(const char[] name);
	
	public static native void GetName(int index, char[] name, int length);
	
	public static native int GetDamageType(int index);
	
	public static native int GeteTracerType(int index);
	
	public static native float GetDamageForce(int index);
	
	public static native AmmoFlags_t GetFlags(int index);
	
	public static native int GetMaxCarry(int index);
	
	public static native Address Raw();	//CAmmoDef* GetAmmoDef();
}

methodmap ServerTools
{
	/**
	 * Finds an entity given a procedural name.
	 * 
	 * @param name: 		The procedural name to search for, should start with '!'
	 * @param searchent:	The entity to start from when doing the search
	 * @param activator:	The activator entity if this was called from an input
	 * @param caller
	 */
	public static native int FindEntityProcedural(const char[] name, int searchent, int activator = -1, int caller = -1);
		
	/**
	 * Find the nearest entity along the facing direction from the given origin
	 * within the angular threshold (ignores worldspawn).
	 * 
	 * @param origin
	 * @param facing
	 * @param threshold
	 */
	public static native int FindEntityNearestFacing(const float origin[3], const float facing[3], float threshold);
	
	/**
	 * Finds the first entity within radius distance by class name.
	 * 
	 * @param startentity:	The entity to start from when doing the search
	 * @param classname:	Entity class name
	 * @param origin:		Center of search radius
	 * @param radius:		Search radius for classname search, 0 to search everywhere
	 */
	public static native int FindEntityByClassnameWithin(int startentity, const char[] classname, const float origin[3], float radius);
	
	/**
	 * Finds the nearest entity by class name withing given search radius.
	 * 
	 * @param classname:	Entity class name
	 * @param origin:		Center of search radius
	 * @param radius:		Search radius for classname search, 0 to search everywhere
	 */
	public static native int FindEntityByClassnameNearest(const char[] classname, const float origin[3], float radius);
	
	/**
	 * Finds entity by model.
	 * 
	 * @param startentity:	The entity to start from when doing the search
	 * @param model:		Entity model
	 */
	public static native int FindEntityByModel(int startentity, const char[] model);
	
	/**
	 * Used to iterate all the entities within a sphere.
	 * 
	 * @param startentity:	The entity to start from when doing the search
	 * @param origin:		Center of search radius
	 * @param radius:		Search radius for classname search, 0 to search everywhere
	 */
	public static native int FindEntityInSphere(int startentity, const float origin[3], float radius);
	
	
	/**
	 * Returns the first entity.
	 */
	public static native int FirstEntity();
	
	/**
	 * Returns the next entity.
	 * 
	 * @param entity
	 */
	public static native int NextEntity(int entity);
	
	/**
	 * Finds entity by HammerID.
	 * 
	 * @param hammerid
	 */
	public static native int FindEntityByHammerID(int hammerid);
	
	/**
	 * Returns entity from its address.
	 * 
	 * @param hammerid
	 */
	public static native int GetBaseEntity(Address pentity);
	
	
	/**
	 * Resets entity's sequence.
	 * 
	 * @param entity
	 * @param sequence
	 */
	public static native void ResetSequence(int entity, int sequence);
	
	/**
	 * Resets entity's sequence.
	 * 
	 * @param entity
	 */
	public static native void ResetSequenceInfo(int entity);
	
	
	/**
	 * Get entity's KeyValue.
	 * 
	 * @param entity
	 * @param key
	 * @param results
	 * @param maxlen
	 */
	public static native bool GetKeyValue(int entity, const char[] key, char[] results, int maxlen);
	
	/**
	 * Set entity's KeyValue.
	 * 
	 * @param entity
	 * @param key
	 * @param value
	 */
	public static native bool SetKeyValue(int entity, const char[] key, const char[] value);
	
	/**
	 * Set entity's KeyValue.
	 * 
	 * @param entity
	 * @param key
	 * @param value
	 */
	public static native bool SetKeyValueFloat(int entity, const char[] key, const float value);
	
	/**
	 * Set entity's KeyValue.
	 * 
	 * @param entity
	 * @param key
	 * @param value
	 */
	public static native bool SetKeyValueVector(int entity, const char[] key, const float value[3]);
}


methodmap NetChannel
{
	/**
	 * Get client's Net Channel.
	 * 
	 * @param client
	 */
	public static native NetChannel GetNetInfo(int client);
	
	/**
	 * data rate in byte/sec
	 */
	property int DataRate {
		public native get();
		public native set(const int rate);
	}
	
	property float TimeOut {
		public native set(const float sec);
	}
	
	/**
	 * channel's packet size
	 */
	property int BufferSize {
		public native get();
	}
	
	/**
	 * packets lost before getting last update
	 */
	property int DropNumber {
		public native get();
	}
	
	property float Time {
		public native get();
	}
	
	/**
	 * TimeConnected = Time - (time since connect)
 	 */
	property float TimeConnected {
		public native get();
	}
	
	property bool IsOverflowed {
		public native get();
	}
	
	property bool IsLoopback {
		public native get();
	}
	
	property bool IsNull {
		public native get();
	}
	
	property bool IsTimingOut {
		public native get();
	}
	
	property Address MsgHandler {
		public native get();
	}
	
	/**
	 * channel's IP address as string
	 */
	public native void Address(char[] buffer, int maxlen);
	
	public native float AvgChoke(NetFlow flow);
	public native float AvgData(NetFlow flow);
	public native float AvgLatency(NetFlow flow);
	public native float AvgLoss(NetFlow flow);
	public native float AvgPackets(NetFlow flow);
	
	public native void SetMaxBufferSize(bool reliable, int bytes, bool voice);
	
	/**
	 * time when packet was sent
	 */
	public native float GetPacketTime(NetFlow flow, int frame);
	
	/**
	 * true if packet was not lost, dropped, chocked or flushed
	 */
	public native bool IsValidPacket(NetFlow flow, int frame);
	
	public native float SendData(Address bfwrite_data);
	public native void Reset();
	/*
	MarkNativeAsOptional("NetChannel.IsValidPacket");
	MarkNativeAsOptional("NetChannel.SendData");
	MarkNativeAsOptional("NetChannel.Reset");
	*/
}


#if !defined REQUIRE_EXTENSIONS
public __ext_INTERFACE_SetNTVOptional()
{
	MarkNativeAsOptional("CTakeDamageInfo.CTakeDamageInfo");
	MarkNativeAsOptional("CTakeDamageInfo.ReadInt");
	MarkNativeAsOptional("CTakeDamageInfo.StoreInt");
	MarkNativeAsOptional("CTakeDamageInfo.ReadFloat");
	MarkNativeAsOptional("CTakeDamageInfo.StoreFloat");
	MarkNativeAsOptional("CTakeDamageInfo.ReadVector");
	MarkNativeAsOptional("CTakeDamageInfo.StoreVector");
	MarkNativeAsOptional("CTakeDamageInfo.ReadEnt");
	MarkNativeAsOptional("CTakeDamageInfo.StoreEnt");
	MarkNativeAsOptional("CTakeDamageInfo.DeathNotice");
	MarkNativeAsOptional("CTakeDamageInfo.Infos.set");
	MarkNativeAsOptional("CTakeDamageInfo.Infos.set");
	MarkNativeAsOptional("CTakeDamageInfo.CalcExplosiveDmgForce");
	MarkNativeAsOptional("CTakeDamageInfo.CalcBulletDamageForce");
	MarkNativeAsOptional("CTakeDamageInfo.CalcMeleeDamageForce");
	
	MarkNativeAsOptional("HookRawOnTakeDamage");
	MarkNativeAsOptional("UnhookRawOnTakeDamage");
	
	MarkNativeAsOptional("CAmmoDefs.FindIndex");
	MarkNativeAsOptional("CAmmoDefs.GetName");
	MarkNativeAsOptional("CAmmoDefs.GetDamageType");
	MarkNativeAsOptional("CAmmoDefs.GeteTracerType");
	MarkNativeAsOptional("CAmmoDefs.GetDamageForce");
	MarkNativeAsOptional("CAmmoDefs.GetFlags");
	MarkNativeAsOptional("CAmmoDefs.GetMaxCarry");
	MarkNativeAsOptional("CAmmoDefs.Raw");
	
	MarkNativeAsOptional("ServerTools.FindEntityProcedural");
	MarkNativeAsOptional("ServerTools.FindEntityClassNearestFacing");
	MarkNativeAsOptional("ServerTools.FindEntityNearestFacing");
	MarkNativeAsOptional("ServerTools.FindEntityByClassnameWithin");
	MarkNativeAsOptional("ServerTools.FindEntityByClassnameNearest");
	MarkNativeAsOptional("ServerTools.FindEntityByModel");
	MarkNativeAsOptional("ServerTools.FindEntityInSphere");
	MarkNativeAsOptional("ServerTools.FirstEntity");
	MarkNativeAsOptional("ServerTools.NextEntity");
	MarkNativeAsOptional("ServerTools.FindEntityByHammerID");
	MarkNativeAsOptional("ServerTools.GetEntityFromAddress");
	MarkNativeAsOptional("ServerTools.ResetSequence");
	MarkNativeAsOptional("ServerTools.ResetSequenceInfo");
	MarkNativeAsOptional("ServerTools.GetKeyValue");
	MarkNativeAsOptional("ServerTools.ClearMultiDamage");
	MarkNativeAsOptional("ServerTools.SetKeyValue");
	MarkNativeAsOptional("ServerTools.SetKeyValueFloat");
	MarkNativeAsOptional("ServerTools.SetKeyValueVector");
	
	MarkNativeAsOptional("NetChannel.GetNetInfo");
	MarkNativeAsOptional("NetChannel.DataRate.get");
	MarkNativeAsOptional("NetChannel.DataRate.set");
	MarkNativeAsOptional("NetChannel.TimeOut.set");
	MarkNativeAsOptional("NetChannel.BufferSize.get");
	MarkNativeAsOptional("NetChannel.Address");
	MarkNativeAsOptional("NetChannel.DropNumber.get");
	MarkNativeAsOptional("NetChannel.AvgChoke");
	MarkNativeAsOptional("NetChannel.AvgData");
	MarkNativeAsOptional("NetChannel.AvgLatency");
	MarkNativeAsOptional("NetChannel.AvgLoss");
	MarkNativeAsOptional("NetChannel.AvgPackets");
	MarkNativeAsOptional("NetChannel.SetMaxBufferSize");
	MarkNativeAsOptional("NetChannel.Time.get");
	MarkNativeAsOptional("NetChannel.TimeConnected.get");
	MarkNativeAsOptional("NetChannel.GetPacketTime");
	MarkNativeAsOptional("NetChannel.IsOverflowed.get");
	MarkNativeAsOptional("NetChannel.IsLoopback.get");
	MarkNativeAsOptional("NetChannel.IsNull.get");
	MarkNativeAsOptional("NetChannel.IsTimingOut.get");
	MarkNativeAsOptional("NetChannel.MsgHandler.get");
	MarkNativeAsOptional("NetChannel.IsValidPacket");
	MarkNativeAsOptional("NetChannel.SendData");
	MarkNativeAsOptional("NetChannel.Reset");
}
#endif

public Extension __ext_TF_OBJTOOLS = 
{
	name = "[TF2] ObjTools",
	file = "tf_objtools.ext",
#if defined AUTOLOAD_EXTENSIONS
	autoload = 1,
#else
	autoload = 0,
#endif
#if defined REQUIRE_EXTENSIONS
	required = 1,
#else
	required = 0,
#endif
};