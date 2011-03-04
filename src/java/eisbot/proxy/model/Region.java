package eisbot.proxy.model;

import java.util.ArrayList;
/**
 * Represents a region in a StarCraft map.
 * 
 * For a description of fields see: http://code.google.com/p/bwta/wiki/Region
 */
public class Region {

	public static final int numAttributes = 3;

	private int ID;
	private int centerX;
	private int centerY;
	private int[] coordinates;
	private ArrayList<Region> connectedRegions = new ArrayList<Region>();
	private ArrayList<ChokePoint> chokePoints = new ArrayList<ChokePoint>();
	
	public Region(int[] data, int index) {
		ID = data[index++];	
		centerX = data[index++];	
		centerY = data[index++];	
	}

	public int[] getCoordinates() {
		return coordinates;
	}

	public void setCoordinates(int[] coordinates) {
		this.coordinates = coordinates;
	}

	public int getID() {
		return ID;
	}

	public int getCenterX() {
		return centerX;
	}

	public int getCenterY() {
		return centerY;
	}

	public ArrayList<Region> getConnectedRegions() {
		return connectedRegions;
	}

	public ArrayList<ChokePoint> getChokePoints() {
		return chokePoints;
	}
}
