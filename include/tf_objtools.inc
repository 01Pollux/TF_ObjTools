/*
**
*/
#if defined _TF_OBJTOOLS_included
 #endinput
#endif
#define _TF_OBJTOOLS_included

enum CTakeDamageOffset {
	vecDamageForce = 0,
	vecDamagePosition,
	vecReportedPosition,
	
	hInflictor,
	hAttacker,
	hWeapon,
	
	flDamage,
	flMaxDamage,
	flBaseDamage,
	bitsDamageType,
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

enum AmmoFlags_t
{
	AMMO_FORCE_DROP_IF_CARRIED = 0x1,
	AMMO_INTERPRET_PLRDAMAGE_AS_DAMAGE_TO_PLAYER = 0x2
};

native int CAmmoDefs_FindIndex(const char[] name);
native void CAmmoDefs_GetName(int index, char[] name, int length);
native int CAmmoDefs_GetDamageType(int index);
native int CAmmoDefs_GeteTracerType(int index);
native float CAmmoDefs_GetDamageForce(int index);
native AmmoFlags_t CAmmoDefs_GetFlags(int index);
native int CAmmoDefs_GetMaxCarry(int index);
native Address CAmmoDefs_Raw();	//CAmmoDef* GetAmmoDef();


/**
 * Finds an entity given a procedural name.
 * 
 * @param name: 		The procedural name to search for, should start with '!'
 * @param searchent:	The entity to start from when doing the search
 * @param activator:	The activator entity if this was called from an input
 * @param caller
 */
native int ST_FindEntityProcedural(const char[] name, int searchent, int activator = -1, int caller = -1);

/**
 * Find the nearest entity along the facing direction from the given origin
 * within the angular threshold (ignores worldspawn) with the given classname.
 * 
 * @param origin
 * @param facing
 * @param threshold
 * @param classname
 */
native int ST_FindEntityClassNearestFacing(const float origin[3], const float facing[3], float threshold, const char[] classname);

/**
 * Find the nearest entity along the facing direction from the given origin
 * within the angular threshold (ignores worldspawn).
 * 
 * @param origin
 * @param facing
 * @param threshold
 */
native int ST_FindEntityNearestFacing(const float origin[3], const float facing[3], float threshold);

/**
 * Finds the first entity within radius distance by class name.
 * 
 * @param startentity:	The entity to start from when doing the search
 * @param classname:	Entity class name
 * @param origin:		Center of search radius
 * @param radius:		Search radius for classname search, 0 to search everywhere
 */
native int ST_FindEntityByClassnameWithin(int startentity, const char[] classname, const float origin[3], float radius);

/**
 * Finds the nearest entity by class name withing given search radius.
 * 
 * @param classname:	Entity class name
 * @param origin:		Center of search radius
 * @param radius:		Search radius for classname search, 0 to search everywhere
 */
native int ST_FindEntityByClassnameNearest(const char[] classname, const float origin[3], float radius);

/**
 * Finds entity by model.
 * 
 * @param startentity:	The entity to start from when doing the search
 * @param model:		Entity model
 */
native int ST_FindEntityByModel(int startentity, const char[] model);

/**
 * Used to iterate all the entities within a sphere.
 * 
 * @param startentity:	The entity to start from when doing the search
 * @param origin:		Center of search radius
 * @param radius:		Search radius for classname search, 0 to search everywhere
 */
native int ST_FindEntityInSphere(int startentity, const float origin[3], float radius);


/**
 * Returns the first entity.
 */
native int ST_FirstEntity();

/**
 * Returns the next entity.
 * 
 * @param entity
 */
native int ST_NextEntity(int entity);

/**
 * Finds entity by HammerID.
 * 
 * @param hammerid
 */
native int ST_FindEntityByHammerID(int hammerid);

/**
 * Returns entity from its address.
 * 
 * @param pentity
 */
native int ST_GetBaseEntity(Address pentity);


/**
 * Resets entity's sequence.
 * 
 * @param entity
 * @param sequence
 */
native void ST_ResetSequence(int entity, int sequence);

/**
 * Resets entity's sequence.
 * 
 * @param entity
 */
native void ST_ResetSequenceInfo(int entity);


/**
 * Get entity's KeyValue.
 * 
 * @param entity
 * @param key
 * @param results
 * @param maxlen
 */
native bool ST_GetKeyValue(int entity, const char[] key, char[] results, int maxlen);

/**
 * Set entity's KeyValue.
 * 
 * @param entity
 * @param key
 * @param value
 */
native bool ST_SetKeyValue(int entity, const char[] key, const char[] value);

/**
 * Set entity's KeyValue.
 * 
 * @param entity
 * @param key
 * @param value
 */
native bool ST_SetKeyValueFloat(int entity, const char[] key, const float value);

/**
 * Set entity's KeyValue.
 * 
 * @param entity
 * @param key
 * @param value
 */
native bool ST_SetKeyValueVector(int entity, const char[] key, const float value[3]);


#if !defined REQUIRE_EXTENSIONS
public __ext_INTERFACE_SetNTVOptional()
{
	MarkNativeAsOptional("CAmmoDefs_FindIndex");
	MarkNativeAsOptional("CAmmoDefs_GetName");
	MarkNativeAsOptional("CAmmoDefs_GetDamageType");
	MarkNativeAsOptional("CAmmoDefs_GeteTracerType");
	MarkNativeAsOptional("CAmmoDefs_GetDamageForce");
	MarkNativeAsOptional("CAmmoDefs_GetFlags");
	MarkNativeAsOptional("CAmmoDefs_GetMaxCarry");
	MarkNativeAsOptional("CAmmoDefs_Raw");
	
	MarkNativeAsOptional("ST_FindEntityProcedural");
	MarkNativeAsOptional("ST_FindEntityClassNearestFacing");
	MarkNativeAsOptional("ST_FindEntityNearestFacing");
	MarkNativeAsOptional("ST_FindEntityByClassnameWithin");
	MarkNativeAsOptional("ST_FindEntityByClassnameNearest");
	MarkNativeAsOptional("ST_FindEntityByModel");
	MarkNativeAsOptional("ST_FindEntityInSphere");
	MarkNativeAsOptional("ST_FirstEntity");
	MarkNativeAsOptional("ST_NextEntity");
	MarkNativeAsOptional("ST_FindEntityByHammerID");
	MarkNativeAsOptional("ST_GetEntityFromAddress");
	MarkNativeAsOptional("ST_ResetSequence");
	MarkNativeAsOptional("ST_ResetSequenceInfo");
	MarkNativeAsOptional("ST_ClearMultiDamage");
	MarkNativeAsOptional("ST_ApplyMultiDamage");
	MarkNativeAsOptional("ST_AddMultiDamage");
	MarkNativeAsOptional("ST_RadiusDamage");
	MarkNativeAsOptional("ST_GetKeyValue");
	MarkNativeAsOptional("ST_ClearMultiDamage");
	MarkNativeAsOptional("ST_SetKeyValue");
	MarkNativeAsOptional("ST_SetKeyValueFloat");
	MarkNativeAsOptional("ST_SetKeyValueVector");
}
#endif

public Extension __ext_TF_OBJTOOLS = 
{
	name = "[TF2] CTakeDamageInfo and AmmoDef Tools",
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
