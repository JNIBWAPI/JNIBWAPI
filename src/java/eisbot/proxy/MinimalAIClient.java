package eisbot.proxy;
/**
 * Example of a Java AI Client that does nothing.
 */
import eisbot.proxy.model.Unit;
import eisbot.proxy.util.BWColor;
public class MinimalAIClient implements BWAPIEventListener {

	JNIBWAPI bwapi;
	public static void main(String[] args) {
		new MinimalAIClient();
	}
	
	public MinimalAIClient() {
		bwapi = new JNIBWAPI(this);
		bwapi.start();
	} 

	public void connected() {}	
	public void gameStarted() {}
	public void gameUpdate() 
	{
		for(Unit u : bwapi.getAllUnits())
		{
			bwapi.drawCircle(u.getX(), u.getY(), 5, BWColor.RED, true, false);
		}
	}
	public void gameEnded() { }
	public void keyPressed(int keyCode) {}
	public void matchEnded(boolean winner) { }
	public void nukeDetect(int x, int y) { }
	public void nukeDetect() { }
	public void playerLeft(int id) { }
	public void unitCreate(int unitID) { }
	public void unitDestroy(int unitID) { }
	public void unitDiscover(int unitID) { }
	public void unitEvade(int unitID) { }
	public void unitHide(int unitID) { }
	public void unitMorph(int unitID) { }
	public void unitShow(int unitID) { }
}
