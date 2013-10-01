package jnibwapi.types;

/**
 * Represents a StarCraft event type.
 * 
 * For a description of fields see: http://code.google.com/p/bwapi/wiki/Event
 */
public enum EventType {
	MatchStart,
	MatchEnd,
	MatchFrame,
	MenuFrame,
	SendText,
	ReceiveText,
	PlayerLeft,
	NukeDetect,
	UnitDiscover,
	UnitEvade,
	UnitShow,
	UnitHide,
	UnitCreate,
	UnitDestroy,
	UnitMorph,
	UnitRenegade,
	SaveGame,
	UnitComplete,
	// TriggerAction,
	PlayerDropped, // Will be removed in later versions of BWAPI
	None;
	public int getID() {
		return ordinal();
	}
}
