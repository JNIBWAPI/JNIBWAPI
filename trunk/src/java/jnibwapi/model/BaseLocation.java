package jnibwapi.model;

/**
 * Represents a StarCraft base location.
 * 
 * For a description of fields see: http://code.google.com/p/bwta/wiki/BaseLocation
 */
public class BaseLocation {
	
	public static final int numAttributes = 10;
	
	private final int x;
	private final int y;
	private final int tx;
	private final int ty;
	private final int regionID;
	private final int minerals;
	private final int gas;
	private final boolean island;
	private final boolean mineralOnly;
	private final boolean startLocation;
	
	public BaseLocation(int[] data, int index) {
		x = data[index++];
		y = data[index++];
		tx = data[index++];
		ty = data[index++];
		regionID = data[index++];
		minerals = data[index++];
		gas = data[index++];
		island = (data[index++] == 1);
		mineralOnly = (data[index++] == 1);
		startLocation = (data[index++] == 1);
	}
	
	public int getX() {
		return x;
	}
	
	public int getY() {
		return y;
	}
	
	public int getTx() {
		return tx;
	}
	
	public int getTy() {
		return ty;
	}
	
	public int getRegionID() {
		return regionID;
	}
	
	public int getMinerals() {
		return minerals;
	}
	
	public int getGas() {
		return gas;
	}
	
	public boolean isIsland() {
		return island;
	}
	
	public boolean isMineralOnly() {
		return mineralOnly;
	}
	
	public boolean isStartLocation() {
		return startLocation;
	}
}
