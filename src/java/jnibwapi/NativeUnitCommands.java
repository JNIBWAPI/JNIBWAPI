package jnibwapi;

/**
 * Interface for callbacks from Unit classes to JNIBWAPI native unit command methods. See:
 * http://code.google.com/p/bwapi/wiki/Unit
 */
public interface NativeUnitCommands {
	// TODO Switch to using just issueCommand and canIssueCommand
	public boolean attack(int unitID, int x, int y);
	public boolean attack(int unitID, int targetID);
	public boolean build(int unitID, int tx, int ty, int typeID);
	public boolean buildAddon(int unitID, int typeID);
	public boolean train(int unitID, int typeID);
	public boolean morph(int unitID, int typeID);
	public boolean research(int unitID, int techID);
	public boolean upgrade(int unitID, int updateID);
	public boolean setRallyPoint(int unitID, int x, int y);
	public boolean setRallyPoint(int unitID, int targetID);
	public boolean move(int unitID, int x, int y);
	public boolean patrol(int unitID, int x, int y);
	public boolean holdPosition(int unitID);
	public boolean stop(int unitID);
	public boolean follow(int unitID, int targetID);
	public boolean gather(int unitID, int targetID);
	public boolean returnCargo(int unitID);
	public boolean repair(int unitID, int targetID);
	public boolean burrow(int unitID);
	public boolean unburrow(int unitID);
	public boolean cloak(int unitID);
	public boolean decloak(int unitID);
	public boolean siege(int unitID);
	public boolean unsiege(int unitID);
	public boolean lift(int unitID);
	public boolean land(int unitID, int tx, int ty);
	public boolean load(int unitID, int targetID);
	public boolean unload(int unitID, int targetID);
	public boolean unloadAll(int unitID);
	public boolean unloadAll(int unitID, int x, int y);
	public boolean rightClick(int unitID, int x, int y);
	public boolean rightClick(int unitID, int targetID);
	public boolean haltConstruction(int unitID);
	public boolean cancelConstruction(int unitID);
	public boolean cancelAddon(int unitID);
	public boolean cancelTrain(int unitID, int slot);
	public boolean cancelMorph(int unitID);
	public boolean cancelResearch(int unitID);
	public boolean cancelUpgrade(int unitID);
	public boolean useTech(int unitID, int typeID);
	public boolean useTech(int unitID, int typeID, int x, int y);
	public boolean useTech(int unitID, int typeID, int targetID);
	public boolean placeCOP(int unitID, int tx, int ty);
}
