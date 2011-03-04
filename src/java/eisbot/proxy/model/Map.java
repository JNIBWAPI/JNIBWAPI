package eisbot.proxy.model;

import java.util.ArrayList;

/**
 * Stores information about a StarCraft map.
 */
public class Map {

	private int width;
	private int height;
	private int walkWidth;
	private int walkHeight;
	private String name;
	private String hash;
	private int[] heightMap;
	private boolean[] buildable;
	private boolean[] walkable;
	
	private ArrayList<Region> regions = new ArrayList<Region>();
	private ArrayList<ChokePoint> chokePoints = new ArrayList<ChokePoint>();
	private ArrayList<BaseLocation> baseLocations = new ArrayList<BaseLocation>();
	
	public Map(int width, int height, String name, String hash, int[] heightMap, int[] buildable, int[] walkable) {
		this.width = width;
		this.height = height;
		this.walkWidth = 4*width;
		this.walkHeight = 4*height;
		this.name = name;
		this.hash = hash;
		this.heightMap = heightMap;
		this.buildable = new boolean[buildable.length];
		this.walkable = new boolean[walkable.length];
		
		for (int i=0; i<buildable.length; i++) {
			this.buildable[i] = (buildable[i] == 1);
		}
		
		for (int i=0; i<walkable.length; i++) {
			this.walkable[i] = (walkable[i] == 1);
		}
	}
	
	public int getWidth() {
		return width;
	}
	
	public int getHeight() {
		return height;
	}
	
	public int getWalkWidth() {
		return walkWidth;
	}
	
	public int getWalkHeight() {
		return walkHeight;
	}
	
	public String getName() {
		return name;
	}
	
	public String getHash() {
		return hash;
	}
	
	public int getHeight(int tx, int ty) {
		if (tx < width && ty < height && tx >= 0 && ty >= 0) {
			return heightMap[tx + width*ty];
		}
		else {
			return 0;
		}
	}
	
	public boolean isBuildable(int tx, int ty) {
		if (tx < width && ty < height && tx >= 0 && ty >= 0) {
			return buildable[tx + width*ty];
		}
		else {
			return false;
		}
	}
	
	public boolean isWalkable(int wx, int wy) {
		if (wx < walkWidth && wy < walkHeight && wx >= 0 && wy >= 0) {
			return walkable[wx + walkWidth*wy];
		}
		else {
			return false;
		}
	}

	public ArrayList<Region> getRegions() {
		return regions;
	}

	public ArrayList<ChokePoint> getChokePoints() {
		return chokePoints;
	}

	public ArrayList<BaseLocation> getBaseLocations() {
		return baseLocations;
	}
}
