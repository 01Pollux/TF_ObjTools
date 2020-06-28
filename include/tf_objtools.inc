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
/**
 * CAmmoDef* GetAmmoDef();
 */
native Address CAmmoDefs_Raw();

methodmap CAmmoDefs
{
	public static int FindIndex(const char[] name) {
		return CAmmoDefs_FindIndex(name);
	}
	public static void Name(int index, char[] name, int length) {
		CAmmoDefs_GetName(index, name, length);
	}
	public static int DamageType(int index) {
		return CAmmoDefs_GetDamageType(index, name, length);
	}
	public static int eTracerType(int index) {
		return CAmmoDefs_GeteTracerType(index);
	}
	public static float DamageForce(int index) {
		return CAmmoDefs_GetDamageForce(index);
	}
	public static AmmoFlags_t Flags(int index) {
		return CAmmoDefs_GetFlags(index);
	}
	public static int MaxCarry(int index) {
		return CAmmoDefs_GetMaxCarry(index);
	}
	/**
	 * CAmmoDef* GetAmmoDef();
	 */
	public static Address Raw() {
		return CAmmoDefs_Raw();
	}
}

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
}
#endif

public Extension __ext_TF_OBJTOOLS = 
{
	name = "[TF2] CTakeDamageInfo and AmmoDef Tools",
	file = "tf_objexpose.ext",
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
