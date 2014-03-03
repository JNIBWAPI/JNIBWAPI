package jnibwapi.model;

import java.awt.Point;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.PriorityQueue;

import jnibwapi.JNIBWAPI;
import jnibwapi.model.Position.Type;
import jnibwapi.types.UnitType;
import jnibwapi.util.BWColor;

/**
 * Stores information about a StarCraft map.
 */
public class Map {
	public static final int TILE_SIZE = 32;
	
	private final Position size;
	private final String name;
	private final String fileName;
	private final String hash;
	private final int[] heightMap;
	private final boolean[] buildable;
	private final boolean[] walkable;
	/** Walkability of build tiles */
	private final boolean[] lowResWalkable;
	
	// The following are set in initialize() method
	/** Region ID for each build tile */
	private int[] regionMap = null;
	private List<Region> regions = null;
	private List<ChokePoint> chokePoints = null;
	private List<BaseLocation> baseLocations = null;
	private HashMap<Integer, Region> idToRegion = null;
	
	public Map(int width, int height, String name, String fileName, String hash, int[] heightMap,
			int[] buildable, int[] walkable) {
		size = new Position(width, height, Type.BUILD);
		this.name = name;
		this.fileName = fileName;
		this.hash = hash;
		this.heightMap = heightMap;
		this.buildable = new boolean[buildable.length];
		this.walkable = new boolean[walkable.length];
		
		for (int i = 0; i < buildable.length; i++) {
			this.buildable[i] = (buildable[i] == 1);
		}
		
		for (int i = 0; i < walkable.length; i++) {
			this.walkable[i] = (walkable[i] == 1);
		}
		
		// Fill lowResWalkable for A* search
		lowResWalkable = new boolean[width * height];
		Arrays.fill(lowResWalkable, true);
		for (int wx = 0; wx < size.getWX(); wx++) {
			for (int wy = 0; wy < size.getWY(); wy++) {
				lowResWalkable[wx / 4 + width * (wy / 4)] &= isWalkable(
						new Position(wx, wy, Type.WALK));
			}
		}
	}
	
	/** Initialise the map with regions and base locations */
	public void initialize(int[] regionMapData, int[] regionData,
			HashMap<Integer, int[]> regionPolygons, int[] chokePointData, int[] baseLocationData) {
		// regionMap
		regionMap = regionMapData;
		
		// regions
		regions = new ArrayList<>();
		if (regionData != null) {
			for (int index = 0; index < regionData.length; index += Region.numAttributes) {
				int[] coordinates = regionPolygons.get(regionData[index]);
				Region region = new Region(regionData, index, coordinates);
				regions.add(region);
			}
		}
		idToRegion = new HashMap<>();
		for (Region region : regions) {
			idToRegion.put(region.getID(), region);
		}
		
		// choke points
		chokePoints = new ArrayList<>();
		if (chokePointData != null) {
			for (int index = 0; index < chokePointData.length; index += ChokePoint.numAttributes) {
				ChokePoint chokePoint = new ChokePoint(chokePointData, index, idToRegion);
				chokePoints.add(chokePoint);
			}
		}
		
		// base locations
		baseLocations = new ArrayList<>();
		if (baseLocationData != null) {
			for (int index = 0; index < baseLocationData.length; index += BaseLocation.numAttributes) {
				BaseLocation baseLocation = new BaseLocation(baseLocationData, index, idToRegion);
				baseLocations.add(baseLocation);
			}
		}
		
		// connect the region graph
		for (ChokePoint chokePoint : chokePoints) {
			chokePoint.getFirstRegion().addChokePoint(chokePoint);
			chokePoint.getFirstRegion().addConnectedRegion(chokePoint.getSecondRegion());
			chokePoint.getSecondRegion().addChokePoint(chokePoint);
			chokePoint.getSecondRegion().addConnectedRegion(chokePoint.getFirstRegion());
		}
	}
	
	/** Get the map size as a Position object */
	public Position getSize() {
		return size;
	}
	
	/** @deprecated Width in build tiles (32px). Use {@link #getSize()} instead. */
	public int getWidth() {
		return size.getBX();
	}
	
	/** @deprecated Height in build tiles (32px). Use {@link #getSize()} instead. */
	public int getHeight() {
		return size.getBY();
	}
	
	/** @deprecated Height in build tiles (32px). Use {@link #getSize()} instead. */
	public int getWalkWidth() {
		return size.getWX();
	}
	
	/** @deprecated Height in build tiles (32px). Use {@link #getSize()} instead. */
	public int getWalkHeight() {
		return size.getWY();
	}
	
	/** The name of the current map */
	public String getName() {
		return name;
	}
	
	/** The file name of the current map / replay file */
	public String getFileName() {
		return fileName;
	}
	
	public String getHash() {
		return hash;
	}
	
	/** Converts a position to a 1-dimensional build tile array index for this map */
	private int getBuildTileArrayIndex(Position p) {
		return p.getBX() + size.getBX() * p.getBY();
	}
	
	public int getGroundHeight(Position p) {
		if (p.isValid(this)) {
			return heightMap[getBuildTileArrayIndex(p)];
		}
		else {
			return 0;
		}
	}
	
	/** Works only after initialize(). Returns null if the specified position is invalid. */
	public Region getRegion(Position p) {
		if (p.isValid(this)) {
			return idToRegion.get(regionMap[getBuildTileArrayIndex(p)]);
		} else {
			return null;
		}
	}
	
	public boolean isBuildable(Position p) {
		if (p.isValid(this)) {
			return buildable[getBuildTileArrayIndex(p)];
		} else {
			return false;
		}
	}
	
	public boolean isWalkable(Position p) {
		if (p.isValid(this)) {
			return walkable[p.getWX() + size.getWX() * p.getWY()];
		} else {
			return false;
		}
	}
	
	/** Checks whether all 16 walk tiles in a build tile are walkable */
	public boolean isLowResWalkable(Position p) {
		if (p.isValid(this)) {
			return lowResWalkable[getBuildTileArrayIndex(p)];
		} else {
			return false;
		}
	}
	
	/** Works only after initialize() */
	public List<Region> getRegions() {
		return Collections.unmodifiableList(regions);
	}
	
	/** Works only after initialize() */
	public Region getRegion(int regionID) {
		return idToRegion.get(regionID);
	}
	
	/** Works only after initialize() */
	public List<ChokePoint> getChokePoints() {
		return Collections.unmodifiableList(chokePoints);
	}
	
	/** Works only after initialize() */
	public List<BaseLocation> getBaseLocations() {
		return Collections.unmodifiableList(baseLocations);
	}
	
	/** Works only after initialize() */
	public List<BaseLocation> getStartLocations() {
		List<BaseLocation> startLocations = new ArrayList<>();
		for (BaseLocation bl : baseLocations) {
			if (bl.isStartLocation()) {
				startLocations.add(bl);
			}
		}
		return startLocations;
	}
	
	/**
	 * Find the shortest walkable distance, in pixels, between two tile positions or -1 if not
	 * reachable. Works only after initialize(). Ported from BWTA.
	 */
	public double getGroundDistance(Position start, Position end) {
		if (!isConnected(start, end))
			return -1;
		return aStarSearchDistance(start.getBX(), start.getBY(), end.getBX(), end.getBY());
	}
	
	/**
	 * Based on map connectedness only. Ignores buildings. Works only after initialize(). Ported
	 * from BWTA.
	 */
	public boolean isConnected(Position start, Position end) {
		if (getRegion(start) == null)
			return false;
		if (getRegion(end) == null)
			return false;
		return getRegion(start).getAllConnectedRegions().contains(getRegion(end));
	}
	
	/**
	 * Performs an A* search. Intended to be called from
	 * {@link #getGroundDistance(int, int, int, int)}. Ported from BWTA.
	 */
	private double aStarSearchDistance(int startTx, int startTy, int endTx, int endTy) {
		// Distance of 10 per build tile, or sqrt(10^2 + 10^2) ~= 14 diagonally
		final int mvmtCost = 10;
		final int mvmtCostDiag = 14;
		PriorityQueue<AStarTile> openTiles = new PriorityQueue<AStarTile>(); // min heap
		// Map from tile to distance
		HashMap<Point, Integer> gmap = new HashMap<Point, Integer>();
		HashSet<Point> closedTiles = new HashSet<Point>();
		Point start = new Point(startTx, startTy);
		Point end = new Point(endTx, endTy);
		openTiles.add(new AStarTile(start, 0));
		gmap.put(start, 0);
		while (!openTiles.isEmpty()) {
			Point p = openTiles.poll().tilePos;
			if (p.equals(end))
				return gmap.get(p) * TILE_SIZE / (double) mvmtCost;
			int gvalue = gmap.get(p);
			closedTiles.add(p);
			// Explore the neighbours of p
			int minx = Math.max(p.x - 1, 0);
			int maxx = Math.min(p.x + 1, size.getBX() - 1);
			int miny = Math.max(p.y - 1, 0);
			int maxy = Math.min(p.y + 1, size.getBY() - 1);
			for (int x = minx; x <= maxx; x++)
				for (int y = miny; y <= maxy; y++) {
					if (!isLowResWalkable(new Position(x, y, Type.BUILD)))
						continue;
					if (p.x != x && p.y != y
							&& !isLowResWalkable(new Position(p.x, y, Type.BUILD))
							&& !isLowResWalkable(new Position(x, p.y, Type.BUILD)))
						continue; // Not diagonally accessible
					Point t = new Point(x, y);
					if (closedTiles.contains(t))
						continue;
					
					int g = gvalue + mvmtCost;
					if (x != p.x && y != p.y)
						g = gvalue + mvmtCostDiag;
					int dx = Math.abs(x - end.x);
					int dy = Math.abs(y - end.y);
					// Heuristic for remaining distance:
					// min(dx, dy) is the minimum diagonal distance, so costs mvmtCostDiag
					// abs(dx - dy) is the rest of the distance, so costs mvmtCost
					int h = Math.abs(dx - dy) * mvmtCost + Math.min(dx, dy) * mvmtCostDiag;
					int f = g + h;
					if (!gmap.containsKey(t) || gmap.get(t) > g) {
						gmap.put(t, g);
						for (Iterator<AStarTile> it = openTiles.iterator(); it.hasNext();)
							if (it.next().tilePos.equals(t))
								it.remove();
						openTiles.add(new AStarTile(t, f));
					}
				}
		}
		// Not found
		return -1;
	}
	
	private static class AStarTile implements Comparable<AStarTile> {
		Point tilePos;
		int distPlusCost;
		
		public AStarTile(Point tile, int distance) {
			tilePos = tile;
			distPlusCost = distance;
		}
		
		@Override
		public int compareTo(AStarTile o) {
			return Integer.compare(distPlusCost, o.distPlusCost);
		}
	}
	
	/**
	 * Debugging method to check terrain has been analysed properly. Taken from BWAPI's
	 * ExampleAIClient
	 */
	public void drawTerrainData(JNIBWAPI bwapi) {
		// iterate through all the base locations and draw their outlines
		for (BaseLocation bl : bwapi.getMap().getBaseLocations()) {
			Position p = bl.getPosition();
			
			// draw outline of base location
			Position otherCorner = p.translated(new Position(4, 3, Type.BUILD));
			bwapi.drawBox(p, otherCorner, BWColor.Blue, false, false);
			
			// if this is an island expansion, draw a yellow circle around the base location
			if (bl.isIsland()) {
				bwapi.drawCircle(p.translated(new Position(2, 1, Type.BUILD)), 80, BWColor.Yellow,
						false, false);
			}
			
			// draw a circle at each mineral patch and a box at each vespene geyser
			for (Unit u : bwapi.getNeutralUnits()) {
				UnitType ut = u.getType();
				if (ut.isResourceContainer()) {
					if (ut.isMineralField()) {
						// Minerals
						bwapi.drawCircle(u.getPosition(), 30, BWColor.Cyan, false, false);
					} else {
						// Geysers
						bwapi.drawBox(u.getTopLeft(), u.getBottomRight(), BWColor.Orange, false,
								false);
					}
				}
			}
		}
		
		// Iterate through all the regions and draw the polygon outline of it in green.
		for (Region r : getRegions()) {
			Position[] polygon = r.getPolygon();
			for (int i = 0; i < polygon.length; i++) {
				Position point1 = polygon[i];
				Position point2 = polygon[(i + 1) % polygon.length];
				bwapi.drawLine(point1, point2, BWColor.Green, false);
			}
		}
		
		// Visualise the chokepoints with red lines
		for (ChokePoint cp : getChokePoints()) {
			Position point1 = cp.getFirstSide();
			Position point2 = cp.getSecondSide();
			bwapi.drawLine(point1, point2, BWColor.Red, false);
		}
	}
	
}
