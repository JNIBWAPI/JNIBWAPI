package jnibwapi;

import java.util.HashSet;

import jnibwapi.model.Unit;
import jnibwapi.types.TechType;
import jnibwapi.types.TechType.TechTypes;
import jnibwapi.types.UnitType.UnitTypes;
import jnibwapi.types.UpgradeType.UpgradeTypes;

/**
 * Example Java AI Client using JNI-BWAPI.
 * 
 * Executes a 5-pool rush and cheats using perfect information.
 * 
 * Note: the agent often gets stuck when attempting to build the spawning pool. It works best on
 * maps where the overlord spawns with plenty of free space around it.
 */
public class ExampleAIClient implements BWAPIEventListener {
	
	/** reference to JNI-BWAPI */
	private JNIBWAPI bwapi;
	
	/** used for mineral splits */
	private HashSet<Integer> claimed = new HashSet<Integer>();
	
	/** has drone 5 been morphed */
	private boolean morphedDrone = false;
	
	/** has a drone been assigned to building a pool? */
	private int poolDrone = -1;
	
	/** when should the next overlord be spawned? */
	private int supplyCap = 0;
	
	/**
	 * Create a Java AI.
	 */
	public static void main(String[] args) {
		new ExampleAIClient();
	}
	
	/**
	 * Instantiates the JNI-BWAPI interface and connects to BWAPI.
	 */
	public ExampleAIClient() {
		bwapi = new JNIBWAPI(this, true);
		bwapi.start();
	}
	
	/**
	 * Connection to BWAPI established.
	 */
	@Override
	public void connected() {
		System.out.println("Connected");
	}
	
	/**
	 * Called at the beginning of a game.
	 */
	@Override
	public void matchStart() {
		System.out.println("Game Started");
		
		bwapi.enableUserInput();
		bwapi.enablePerfectInformation();
		bwapi.setGameSpeed(0);
		
		// reset agent state
		claimed.clear();
		morphedDrone = false;
		poolDrone = -1;
		supplyCap = 0;
	}
	
	/**
	 * Called each game cycle.
	 */
	@Override
	public void matchFrame() {
		// print out some info about any upgrades or research happening
		String msg = "=";
		for (TechTypes t : TechTypes.values()) {
			if (bwapi.getSelf().isResearching(t.getID())) {
				msg += "Researching " + t.name() + "=";
			}
			// Exclude tech that is given at the start of the game
			TechType tt = bwapi.getTechType(t.getID());
			if (tt == null) {
				continue;
			} else {
				int whatResearches = bwapi.getTechType(t.getID()).getWhatResearchesTypeID();
				if (whatResearches == UnitTypes.None.getID()) {
					continue;
				}
			}
			if (bwapi.getSelf().isResearched(t.getID())) {
				msg += "Researched " + t.name() + "=";
			}
		}
		for (UpgradeTypes t : UpgradeTypes.values()) {
			if (bwapi.getSelf().isUpgrading(t.getID())) {
				msg += "Upgrading " + t.name() + "=";
			}
			if (bwapi.getSelf().getUpgradeLevel(t.getID()) > 0) {
				int level = bwapi.getSelf().getUpgradeLevel(t.getID());
				msg += "Upgraded " + t.name() + " to level " + level + "=";
			}
		}
		bwapi.drawText(0, 20, msg, true);
		
		// spawn a drone
		for (Unit unit : bwapi.getMyUnits()) {
			if (unit.getTypeID() == UnitTypes.Zerg_Larva.getID()) {
				if (bwapi.getSelf().getMinerals() >= 50 && !morphedDrone) {
					bwapi.morph(unit.getID(), UnitTypes.Zerg_Drone.getID());
					morphedDrone = true;
				}
			}
		}
		
		// collect minerals
		for (Unit unit : bwapi.getMyUnits()) {
			if (unit.getTypeID() == UnitTypes.Zerg_Drone.getID()) {
				if (unit.isIdle() && unit.getID() != poolDrone) {
					
					for (Unit minerals : bwapi.getNeutralUnits()) {
						if (minerals.getTypeID() == UnitTypes.Resource_Mineral_Field.getID()
								&& !claimed.contains(minerals.getID())) {
							double distance = Math.sqrt(Math.pow(minerals.getX() - unit.getX(), 2)
									+ Math.pow(minerals.getY() - unit.getY(), 2));
							
							if (distance < 300) {
								bwapi.rightClick(unit.getID(), minerals.getID());
								claimed.add(minerals.getID());
								break;
							}
						}
					}
				}
			}
		}
		
		// build a spawning pool
		if (bwapi.getSelf().getMinerals() >= 200 && poolDrone < 0) {
			for (Unit unit : bwapi.getMyUnits()) {
				if (unit.getTypeID() == UnitTypes.Zerg_Drone.getID()) {
					poolDrone = unit.getID();
					break;
				}
			}
			
			// build the pool under the overlord
			for (Unit unit : bwapi.getMyUnits()) {
				if (unit.getTypeID() == UnitTypes.Zerg_Overlord.getID()) {
					bwapi.build(poolDrone, unit.getTileX(), unit.getTileY(),
							UnitTypes.Zerg_Spawning_Pool.getID());
				}
			}
		}
		
		// spawn overlords
		if (bwapi.getSelf().getSupplyUsed() + 2 >= bwapi.getSelf().getSupplyTotal()
				&& bwapi.getSelf().getSupplyTotal() > supplyCap) {
			if (bwapi.getSelf().getMinerals() >= 100) {
				for (Unit larva : bwapi.getMyUnits()) {
					if (larva.getTypeID() == UnitTypes.Zerg_Larva.getID()) {
						bwapi.morph(larva.getID(), UnitTypes.Zerg_Overlord.getID());
						supplyCap = bwapi.getSelf().getSupplyTotal();
					}
				}
			}
		}
		
		// spawn zerglings
		else if (bwapi.getSelf().getMinerals() >= 50) {
			for (Unit unit : bwapi.getMyUnits()) {
				if (unit.getTypeID() == UnitTypes.Zerg_Spawning_Pool.getID() && unit.isCompleted()) {
					for (Unit larva : bwapi.getMyUnits()) {
						if (larva.getTypeID() == UnitTypes.Zerg_Larva.getID()) {
							bwapi.morph(larva.getID(), UnitTypes.Zerg_Zergling.getID());
						}
					}
				}
			}
		}
		
		// attack move toward an enemy
		for (Unit unit : bwapi.getMyUnits()) {
			if (unit.getTypeID() == UnitTypes.Zerg_Zergling.getID() && unit.isIdle()) {
				for (Unit enemy : bwapi.getEnemyUnits()) {
					bwapi.attack(unit.getID(), enemy.getX(), enemy.getY());
					break;
				}
			}
		}
	}
	
	@Override
	public void keyPressed(int keyCode) {}
	@Override
	public void matchEnd(boolean winner) {}
	@Override
	public void sendText(String text) {}
	@Override
	public void receiveText(String text) {}
	@Override
	public void nukeDetect(int x, int y) {}
	@Override
	public void nukeDetect() {}
	@Override
	public void playerLeft(int playerID) {}
	@Override
	public void unitCreate(int unitID) {}
	@Override
	public void unitDestroy(int unitID) {}
	@Override
	public void unitDiscover(int unitID) {}
	@Override
	public void unitEvade(int unitID) {}
	@Override
	public void unitHide(int unitID) {}
	@Override
	public void unitMorph(int unitID) {}
	@Override
	public void unitShow(int unitID) {}
	@Override
	public void unitRenegade(int unitID) {}
	@Override
	public void saveGame(String gameName) {}
	@Override
	public void unitComplete(int unitID) {}
	@Override
	public void playerDropped(int playerID) {}
}
