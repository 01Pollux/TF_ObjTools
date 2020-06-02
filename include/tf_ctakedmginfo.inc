/*
**
*/
#if defined _TAKEDMGTOOL_included
 #endinput
#endif
#define _TAKEDMGTOOL_included

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
	kCritType_Crit,
};

methodmap CTakeDamageInfo < Handle {
	/**
	 * Creates a CTakeDamageInfo object handle
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
	 * @param client : victim index
	 */
	public native void DeathNotice(int client);
	
	/**
	 * Return the address to use as CTakeDamageInfo object
	 */
	property Address Infos {
		public native get();
	}
}


public Extension __ext_TAKEDMGTOOL = 
{
	name = "[TF2] CTakeDamageInfo-Tool",
	file = "tf_ctakedmginfo.ext",
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
