package eisbot.proxy.types;
/**
 * Represents a StarCraft upgrade type.
 * 
 * For a description of fields see: http://code.google.com/p/bwapi/wiki/UpgradeType
 */
public class UpgradeType { 
	
	public static final int numAttributes = 10;

	private String name;	
	private int ID;
	private int raceID;
	private int mineralPriceBase;
	private int mineralPriceFactor;
	private int gasPriceBase;
	private int gasPriceFactor;
	private int upgradeTimeBase;
	private int upgradeTimeFactor;
	private int maxRepeats;
	private int whatUpgradesTypeID;
	
	public enum UpgradeTypes {
		Terran_Infantry_Armor,
		Terran_Vehicle_Plating,
		Terran_Ship_Plating,
		Zerg_Carapace,
		Zerg_Flyer_Carapace,
		Protoss_Ground_Armor,
		Protoss_Air_Armor,
		Terran_Infantry_Weapons,
		Terran_Vehicle_Weapons,
		Terran_Ship_Weapons,
		Zerg_Melee_Attacks,
		Zerg_Missile_Attacks,
		Zerg_Flyer_Attacks,
		Protoss_Ground_Weapons,
		Protoss_Air_Weapons,
		Protoss_Plasma_Shields,
		U_238_Shells,
		Ion_Thrusters,
		Undefined18,
		Titan_Reactor,
		Ocular_Implants,
		Moebius_Reactor,
		Apollo_Reactor,
		Colossus_Reactor,
		Ventral_Sacs,
		Antennae,
		Pneumatized_Carapace,
		Metabolic_Boost,
		Adrenal_Glands,
		Muscular_Augments,
		Grooved_Spines,
		Gamete_Meiosis,
		Metasynaptic_Node,
		Singularity_Charge,
		Leg_Enhancements,
		Scarab_Damage,
		Reaver_Capacity,
		Gravitic_Drive,
		Sensor_Array,
		Gravitic_Boosters,
		Khaydarin_Amulet,
		Apial_Sensors,
		Gravitic_Thrusters,
		Carrier_Capacity,
		Khaydarin_Core,
		Undefined45,
		Undefined46,
		Argus_Jewel,
		Undefined48,
		Argus_Talisman,
		Undefined50,
		Caduceus_Reactor,
		Chitinous_Plating,
		Anabolic_Synthesis,
		Charon_Boosters,
		Undefined55,
		Undefined56,
		Undefined57,
		Undefined58,
		Undefined59,
		Undefined60,
		None,
		Unknown,		
	};
	
	public UpgradeType(int[] data, int index) {
		ID = data[index++];		
		raceID = data[index++];
		mineralPriceBase = data[index++];
		mineralPriceFactor = data[index++];
		gasPriceBase = data[index++];
		gasPriceFactor = data[index++];
		upgradeTimeBase = data[index++];
		upgradeTimeFactor = data[index++];
		maxRepeats = data[index++];
		whatUpgradesTypeID = data[index++];
	}

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public int getID() {
		return ID;
	}

	public int getRaceID() {
		return raceID;
	}

	public int getMineralPriceBase() {
		return mineralPriceBase;
	}

	public int getMineralPriceFactor() {
		return mineralPriceFactor;
	}

	public int getGasPriceBase() {
		return gasPriceBase;
	}

	public int getGasPriceFactor() {
		return gasPriceFactor;
	}

	public int getUpgradeTimeBase() {
		return upgradeTimeBase;
	}

	public int getUpgradeTimeFactor() {
		return upgradeTimeFactor;
	}

	public int getMaxRepeats() {
		return maxRepeats;
	}

	public int getWhatUpgradesTypeID() {
		return whatUpgradesTypeID;
	}
}
