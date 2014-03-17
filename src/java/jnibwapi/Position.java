package jnibwapi;


/** Generalised representation of a position for JNIBWAPI. Immutable. */
public class Position {
	
	public static class Positions {
		public static final Position Invalid = new Position(1000, 1000, PosType.BUILD);
		public static final Position None = new Position(1000, 1001, PosType.BUILD);
		public static final Position Unknown = new Position(1000, 1002, PosType.BUILD);
	}
	
	public static enum PosType {
		PIXEL(1),
		WALK(8),
		BUILD(32);
		
		/** Length in pixels */
		public final int scale;
		
		private PosType(int size) {
			this.scale = size;
		}
	};
	
	private final int x;
	private final int y;
	
	/**
	 * Creates a new Position representing the given x and y as Pixel, Walk Tile, or Build Tile
	 * coordinates (depending on the PosType given).
	 */
	public Position(int x, int y, PosType posType) {
		this.x = x * posType.scale;
		this.y = y * posType.scale;
	}
	
	/** Creates a new Position representing the given x and y as Pixel coordinates. */
	public Position(int x, int y) {
		this(x, y, PosType.PIXEL);
	}
	
	/**
	 * Returns the x-coordinate, in the scale appropriate for the given type
	 * 
	 * @see {@link #getPX()}, {@link #getBX()}, {@link #getWX()}
	 */
	public int getX(PosType posType) {
		return x / posType.scale;
	}
	
	/**
	 * Returns the y-coordinate, in the scale appropriate for the given type
	 * 
	 * @see {@link #getPY()}, {@link #getBY()}, {@link #getWY()}
	 */
	public int getY(PosType posType) {
		return y / posType.scale;
	}
	
	/** Returns the x-coordinate, in pixels */
	public int getPX() {
		return x / PosType.PIXEL.scale;
	}
	
	/** Returns the y-coordinate, in pixels */
	public int getPY() {
		return y / PosType.PIXEL.scale;
	}
	
	/** Returns the x-coordinate, in walk tiles */
	public int getWX() {
		return x / PosType.WALK.scale;
	}
	
	/** Returns the y-coordinate, in walk tiles */
	public int getWY() {
		return y / PosType.WALK.scale;
	}
	
	/** Returns the x-coordinate, in build tiles */
	public int getBX() {
		return x / PosType.BUILD.scale;
	}
	
	/** Returns the y-coordinate, in build tiles */
	public int getBY() {
		return y / PosType.BUILD.scale;
	}
	
	/**
	 * Returns the distance to the target position in Pixel coordinates.
	 * 
	 * @see #getApproxPDistance(Position)
	 */
	public double getPDistance(Position target) {
		int dx = x - target.x;
		int dy = y - target.y;
		return Math.sqrt(dx * dx + dy * dy);
	}
	
	public double getWDistance(Position target) {
		return getPDistance(target) / PosType.WALK.scale;
	}
	
	public double getBDistance(Position target) {
		return getPDistance(target) / PosType.BUILD.scale;
	}
	
	/**
	 * Get an approximate distance to the target position in Pixel coordinates.
	 * 
	 * Uses Starcraft's approximated distance function, which is reasonably accurate yet avoids a
	 * sqrt operation and saves some CPU cycles.
	 * 
	 * @see #getPDistance(Position)
	 **/
	public int getApproxPDistance(Position target) {
		int min = Math.abs(x - target.x);
		int max = Math.abs(y - target.y);
		if (max < min) {
			int temp = max;
			max = min;
			min = temp;
		}
		
		if (min < (max >> 2))
			return max;
		
		int minCalc = (3 * min) >> 3;
		return ((minCalc >> 5) + minCalc + max - (max >> 4) - (max >> 6));
	}
	
	public int getApproxWDistance(Position target) {
		return getApproxPDistance(target) / PosType.WALK.scale;
	}
	
	public int getApproxBDistance(Position target) {
		return getApproxPDistance(target) / PosType.BUILD.scale;
	}
	
	/**
	 * Returns true if the position is on the map. Note: if map info is unavailable, this function
	 * will check validity against the largest (256x256) map size.
	 */
	public boolean isValid() {
		if (x < 0 || y < 0)
			return false;
		Map map;
		if (JNIBWAPI.getInstance() != null)
			map = JNIBWAPI.getInstance().getMap();
		else
			map = null;
		if (map == null)
			return getBX() < 256 && getBY() < 256;
		else
			return x < map.getSize().getPX() && y < map.getSize().getPY();
	}
	
	/**
	 * Returns a <b>new</b> Position in the closest valid map position. Worked out at Build Tile
	 * resolution, like in BWAPI 3. Note: if map info is unavailable, this function will check
	 * validity against the largest (256x256) map size.
	 */
	public Position makeValid() {
		if (isValid())
			return this;
		// 
		int newBtX = Math.max(getBX(), 0);
		int newBtY = Math.max(getBY(), 0);
		Map map;
		if (JNIBWAPI.getInstance() != null)
			map = JNIBWAPI.getInstance().getMap();
		else
			map = null;
		if (map == null) {
			newBtX = Math.min(newBtX, 256 - 1);
			newBtY = Math.min(newBtY, 256 - 1);
		} else {
			newBtX = Math.min(newBtX, map.getSize().getBX() - 1);
			newBtY = Math.min(newBtY, map.getSize().getBY() - 1);
		}
		return new Position(newBtX, newBtY, PosType.BUILD);
	}
	
	/**
	 * Returns a <b>new</b> Position that represents the effect of moving this position by delta
	 * (treated as a vector from the origin)
	 */
	public Position translated(Position delta) {
		return new Position(x + delta.x, y + delta.y);
	}
	
	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + x;
		result = prime * result + y;
		return result;
	}
	
	/** Positions are the same as long as they have the same Pixel coordinates. */
	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		Position other = (Position) obj;
		if (x != other.x)
			return false;
		if (y != other.y)
			return false;
		return true;
	}
	
	@Override
	public String toString() {
		return getClass().getName() + "[x=" + x + ",y=" + y + "]";
	}
}
