package jnibwapi.model;

import java.util.Map;

/**
 * Represents a choke point in a StarCraft map.
 * 
 * For a description of fields see: http://code.google.com/p/bwta/wiki/Chokepoint
 */
public class ChokePoint {
	
	public static final int numAttributes = 9;
	public static final double fixedScale = 100.0;
	
	private final int centerX;
	private final int centerY;
	private final double radius;
	private final int firstRegionID;
	private final int secondRegionID;
	private final int firstSideX;
	private final int firstSideY;
	private final int secondSideX;
	private final int secondSideY;
	private final Region firstRegion;
	private final Region secondRegion;
	
	public ChokePoint(int[] data, int index, Map<Integer, Region> idToRegion) {
		centerX = data[index++];
		centerY = data[index++];
		radius = data[index++] / fixedScale;
		firstRegionID = data[index++];
		secondRegionID = data[index++];
		firstSideX = data[index++];
		firstSideY = data[index++];
		secondSideX = data[index++];
		secondSideY = data[index++];
		firstRegion = idToRegion.get(firstRegionID);
		secondRegion = idToRegion.get(secondRegionID);
	}
	
	public Region getOtherRegion(Region region) {
		return region.equals(firstRegion) ? secondRegion : firstRegion;
	}
	
	public Region getFirstRegion() {
		return firstRegion;
	}
	
	public Region getSecondRegion() {
		return secondRegion;
	}
	
	public int getCenterX() {
		return centerX;
	}
	
	public int getCenterY() {
		return centerY;
	}
	
	public double getRadius() {
		return radius;
	}
	
	public int getFirstRegionID() {
		return firstRegionID;
	}
	
	public int getSecondRegionID() {
		return secondRegionID;
	}
	
	public int getFirstSideX() {
		return firstSideX;
	}
	
	public int getFirstSideY() {
		return firstSideY;
	}
	
	public int getSecondSideX() {
		return secondSideX;
	}
	
	public int getSecondSideY() {
		return secondSideY;
	}
}
