package eisbot.proxy;
/**
 * Interface for BWAPI callback methods;
 * 
 * For BWAPI specific events see: http://code.google.com/p/bwapi/wiki/AIModule
 */
public interface BWAPIEventListener {

	/** connected to bridge */
	public void connected();

	/** game has just started, game settings can be turned on here */
	public void gameStarted();

	/** perform AI logic here */
	public void gameUpdate();
	
	/** game has just terminated */
	public void gameEnded();
	
	/** keyPressed from within StarCraft */
	public void keyPressed(int keyCode);
	
	// BWAPI callbacks
	public void matchEnded(boolean winner);
	public void playerLeft(int id);
	public void nukeDetect(int x, int y);
	public void nukeDetect();
	public void unitDiscover(int unitID);
	public void unitEvade(int unitID);
	public void unitShow(int unitID);
	public void unitHide(int unitID);
	public void unitCreate(int unitID);
	public void unitDestroy(int unitID);	
	public void unitMorph(int unitID);
}
