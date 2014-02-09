package jnibwapi.model;

import java.awt.Point;

import jnibwapi.types.TechType.TechTypes;
import jnibwapi.types.UpgradeType.UpgradeTypes;

/**
 * Represents a StarCraft player.
 * 
 * For a description of fields see: http://code.google.com/p/bwapi/wiki/Player
 */
public class Player {
	
	public static final int numAttributes = 11;
	
	private final int ID;
	private final int raceID;
	private final int typeID;
	private final int startLocationX;
	private final int startLocationY;
	private final boolean self;
	private final boolean ally;
	private final boolean enemy;
	private final boolean neutral;
	private final boolean observer;
	private final int color;
	private final String name;
	
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
	
	private final boolean[] researching;
	private final boolean[] researched;
	private final boolean[] upgrading;
	private final int[] upgradeLevel;
	
	public Player(int[] data, int index, String name) {
		ID = data[index++];
		raceID = data[index++];
		typeID = data[index++];
		startLocationX = data[index++];
		startLocationY = data[index++];
		self = (data[index++] == 1);
		ally = (data[index++] == 1);
		enemy = (data[index++] == 1);
		neutral = (data[index++] == 1);
		observer = (data[index++] == 1);
		color = data[index++];
		this.name = name;
		// Initialise technology records
		int highestIDTechType = 0;
		for (TechTypes tt : TechTypes.values()) {
			highestIDTechType = Math.max(highestIDTechType, tt.getID());
		}
		researching = new boolean[highestIDTechType + 1];
		researched = new boolean[highestIDTechType + 1];
		int highestIDUpgradeType = 0;
		for (UpgradeTypes ut : UpgradeTypes.values()) {
			highestIDUpgradeType = Math.max(highestIDUpgradeType, ut.getID());
		}
		upgrading = new boolean[highestIDUpgradeType + 1];
		upgradeLevel = new int[highestIDUpgradeType + 1];
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
	
	public void updateResearch(int[] techData, int[] upgradeData) {
		for (int i = 0; i < techData.length; i += 3) {
			int techTypeID = techData[i];
			researched[techTypeID] = (techData[i + 1] == 1);
			researching[techTypeID] = (techData[i + 2] == 1);
		}
		
		for (int i = 0; i < upgradeData.length; i += 3) {
			int upgradeTypeID = upgradeData[i];
			upgradeLevel[upgradeTypeID] = upgradeData[i + 1];
			upgrading[upgradeTypeID] = (upgradeData[i + 2] == 1);
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
	
	/**
	 * Returns the starting tile position of the Player, or null if unknown (eg. for enemy players
	 * without complete map information).
	 */
	public Point getStartLocation() {
		if (startLocationX == 1000) {
			return null; // In the case of Invalid/None/Unknown TilePosition
		}
		return new Point(startLocationX, startLocationY);
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
	
	public boolean isObserver() {
		return observer;
	}
	
	public int getColor() {
		return color;
	}
	
	public String getName() {
		return name;
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
	
	public boolean isResearched(int techID) {
		return (techID > 0 && techID < researched.length) ? researched[techID] : false;
	}
	
	public boolean isResearching(int techID) {
		return (techID > 0 && techID < researching.length) ? researching[techID] : false;
	}
	
	public int getUpgradeLevel(int upgradeID) {
		return (upgradeID > 0 && upgradeID < upgradeLevel.length) ?
				upgradeLevel[upgradeID] : 0;
	}
	
	public boolean isUpgrading(int upgradeID) {
		return (upgradeID > 0 && upgradeID < upgrading.length) ? upgrading[upgradeID] : false;
	}
}
