package eisbot.proxy;
/**
 * Example of a Java AI Client that does nothing.
 */
public class MinimalAIClient implements BWAPIEventListener {

	public static void main(String[] args) {
		new MinimalAIClient();
	}
	
	public MinimalAIClient() {
		new JNIBWAPI(this).start();
	} 

	public void connected() {}	
	public void gameStarted() {}
	public void gameUpdate() {}
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
