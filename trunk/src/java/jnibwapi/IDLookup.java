package jnibwapi;

import jnibwapi.model.Player;
import jnibwapi.model.Unit;

/**
 * An interface to the methods for getting an object reference from an ID. These methods may at some
 * point be implemented outside of the JNIBWAPI class.
 **/
public interface IDLookup {
	public Player getPlayer(int playerID);
	
	public Unit getUnit(int unitID);
}
