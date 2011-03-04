package eisbot.proxy.model;
/**
 * Represents a StarCraft player.
 * 
 * For a description of fields see: http://code.google.com/p/bwapi/wiki/Player
 */
public class Player {

	public static final int numAttributes = 8;

	private int ID;
	private int raceID;
	private int typeID;
	private boolean self;
	private boolean ally;
	private boolean enemy;
	private boolean neutral;
	private int color;
	
	private int minerals;
	private int gas;
	private int supplyUsed;
	private int supplyTotal;
	private int cumulativeMinerals;
	private int cumulativeGas;
	private int unitScore;
	private int killScore;
	private int buildingScore;
	private int razingScore;
	
	private boolean[] researching = null;
	private boolean[] researched = null;
	private boolean[] upgrading = null;
	private int[] upgradeLevel = null;
		
	public Player(int[] data, int index) {
		ID = data[index++];	
		raceID = data[index++];	
		typeID = data[index++];	
		self = (data[index++] == 1);	
		ally = (data[index++] == 1);	
		enemy = (data[index++] == 1);	
		neutral = (data[index++] == 1);	
		color = data[index++];	
	}

	public void update(int[] data) {
		int index = 0;
		minerals = data[index++];
		gas = data[index++];
		supplyUsed = data[index++];
		supplyTotal = data[index++];
		cumulativeMinerals = data[index++];
		cumulativeGas = data[index++];
		unitScore = data[index++];
		killScore = data[index++];
		buildingScore = data[index++];
		razingScore = data[index++];
	}
	
	public void updateResearch(int[] researchData, int[] upgradeData) {
		researched = new boolean[researchData.length/2];
		researching = new boolean[researchData.length/2];
		
		for (int i=0; i<researchData.length; i+=2) {
			researched[i/2] = (researchData[i] == 1);
			researching[i/2] = (researchData[i + 1] == 1);
		}
		
		upgradeLevel = new int[upgradeData.length/2];
		upgrading = new boolean[upgradeData.length/2];
		
		for (int i=0; i<upgradeData.length; i+=2) {
			upgradeLevel[i/2] = upgradeData[i];
			upgrading[i/2] = (upgradeData[i + 1] == 1);
		}
	}
	
	public int getID() {
		return ID;
	}

	public int getRaceID() {
		return raceID;
	}

	public int getTypeID() {
		return typeID;
	}

	public boolean isSelf() {
		return self;
	}

	public boolean isAlly() {
		return ally;
	}

	public boolean isEnemy() {
		return enemy;
	}

	public boolean isNeutral() {
		return neutral;
	}

	public int getColor() {
		return color;
	}
	
	public int getMinerals() {
		return minerals;
	}

	public int getGas() {
		return gas;
	}

	public int getSupplyUsed() {
		return supplyUsed;
	}

	public int getSupplyTotal() {
		return supplyTotal;
	}

	public int getCumulativeMinerals() {
		return cumulativeMinerals;
	}

	public int getCumulativeGas() {
		return cumulativeGas;
	}

	public int getUnitScore() {
		return unitScore;
	}

	public int getKillScore() {
		return killScore;
	}

	public int getBuildingScore() {
		return buildingScore;
	}

	public int getRazingScore() {
		return razingScore;
	}
	
	public boolean hasResearched(int techID) {
		return (researched != null && techID < researched.length) ? researched[techID] : false;
	}
	
	public boolean isResearching(int techID) {
		return (researching != null && techID < researching.length) ? researching[techID] : false;
	}

	public int upgradeLevel(int upgradeID) {
		return (upgradeLevel != null && upgradeID < upgradeLevel.length) ? upgradeLevel[upgradeID] : 0;
	}

	public boolean isUpgrading(int upgradeID) {
		return (upgrading != null && upgradeID < upgrading.length) ? upgrading[upgradeID] : false;
	}
}
