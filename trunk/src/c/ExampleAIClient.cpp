#include <stdio.h>
#include <jni.h>
#include <BWAPI.h>
#include <BWAPI\Client.h>
#include <BWTA.h>
#include <windows.h>
#include <string>
#include "eisbot_proxy_JNIBWAPI.h"
#include <sstream>

using namespace BWAPI;

// java callback vars
JNIEnv *jEnv;
jobject classref;
void javaPrint(char* msg);
jmethodID printCallback;

// mapping of IDs to types
std::map<int, UnitType> unitTypeMap;
std::map<int, TechType> techTypeMap;
std::map<int, UpgradeType> upgradeTypeMap;
std::map<int, WeaponType> weaponTypeMap;
std::map<int, UnitSizeType> unitSizeTypeMap;
std::map<int, BulletType> bulletTypeMap;
std::map<int, DamageType> damageTypeMap;
std::map<int, ExplosionType> explosionTypeMap;
std::map<int, UnitCommandType> unitCommandTypeMap;
std::map<int, Order> orderTypeMap;

// region IDs
std::map<BWTA::Region*, int> regionMap;

// data buffer for c++ -> Java data
jint *intBuf;
int bufferSize = 5000000;

// utility functions
void drawHealth(); 
void drawTargets();
void drawIDs();
boolean showHealth = false;
boolean showTargets = false;
boolean showIDs = false;

void reconnect();
void loadTypeData();
bool keyState[256];

// conversion ratios
double TO_DEGREES = 180.0/3.14159;
double fixedScale = 100.0;

/**
 * Entry point from Java
 */
JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_startClient(JNIEnv *env, jobject jObj, jobject classRef)
{
  // get the java callback functions
  jEnv = env;
  classref = classRef;
  jclass jc = env->GetObjectClass(classRef);
  printCallback = env->GetMethodID(jc, "javaPrint","(Ljava/lang/String;)V");

  javaPrint("BWAPI Client launched!!!");
  jmethodID connectedCallback = env->GetMethodID(jc, "connected","()V");
  jmethodID gameStartCallback = env->GetMethodID(jc, "gameStarted","()V");
  jmethodID gameUpdateCallback = env->GetMethodID(jc, "gameUpdate","()V");
  jmethodID gameEndCallback = env->GetMethodID(jc, "gameEnded","()V");
  jmethodID eventCallback = env->GetMethodID(jc, "eventOccured","(III)V");
  jmethodID keyPressCallback = env->GetMethodID(jc, "keyPressed","(I)V");

  // allocate room for return data structure
  intBuf = (jint*)malloc(bufferSize*sizeof(jint));

  // connet to BWAPI
  BWAPI::BWAPI_init();
  javaPrint( "Connecting..." );
  reconnect();
  loadTypeData();
  jEnv->CallObjectMethod(classref, connectedCallback);

  // hold on to this thread forever. Notify java through callbacks.
  while(true)
  {

    // wait for a game to start
	if(Broodwar != NULL)
	{
		javaPrint("waiting to enter match");
		while (!Broodwar->isInGame())
		{
			BWAPI::BWAPIClient.update();
			if(Broodwar == NULL)
				return;
		}
	}
	javaPrint("starting match!");
    jEnv->CallObjectMethod(classref, gameStartCallback);

	// in game
	while(Broodwar->isInGame())
    {
      jEnv->CallObjectMethod(classref, gameUpdateCallback);

	  // process events
	  if (Broodwar->getFrameCount()>1)
	  {
		  for(std::list<Event>::iterator e=Broodwar->getEvents().begin();e!=Broodwar->getEvents().end();e++)
		  {
			  switch (e->getType()) {
			  case EventType::MatchEnd:
				  jEnv->CallObjectMethod(classref, eventCallback, 0, e->isWinner() ? 1 : 0, 0);
				  break;
			  case EventType::PlayerLeft:
  				  jEnv->CallObjectMethod(classref, eventCallback, 1, e->getPlayer()->getID(), 0);
				break;
			  case EventType::NukeDetect:
				if (e->getPosition()!=Positions::Unknown) {
  				  jEnv->CallObjectMethod(classref, eventCallback, 2, e->getPosition().x(), e->getPosition().y());
				}
				else {
  				  jEnv->CallObjectMethod(classref, eventCallback, 3, 0, 0);
				}
				break;
			  case EventType::UnitDiscover:
  			    jEnv->CallObjectMethod(classref, eventCallback, 4, e->getUnit()->getID(), 0);
				break;
			  case EventType::UnitEvade:
  			    jEnv->CallObjectMethod(classref, eventCallback, 5, e->getUnit()->getID(), 0);
				break;
			  case EventType::UnitShow:
  			    jEnv->CallObjectMethod(classref, eventCallback, 6, e->getUnit()->getID(), 0);
				break;
			  case EventType::UnitHide:
  			    jEnv->CallObjectMethod(classref, eventCallback, 7, e->getUnit()->getID(), 0);
				break;
			  case EventType::UnitCreate:
  			    jEnv->CallObjectMethod(classref, eventCallback, 8, e->getUnit()->getID(), 0);
				break;
			  case EventType::UnitDestroy:
  			    jEnv->CallObjectMethod(classref, eventCallback, 9, e->getUnit()->getID(), 0);
				break;
			  case EventType::UnitMorph:
  			    jEnv->CallObjectMethod(classref, eventCallback, 10, e->getUnit()->getID(), 0);
				break;
			  // Don't currently care about these
			  case EventType::UnitRenegade:
				break;
			  case EventType::SaveGame:
				break;
			  case EventType::SendText:
				break;
			  case EventType::ReceiveText:
				break;
			  }
		  }

		  // check for key presses
		  for (int keyCode=0; keyCode<=0xff; keyCode++) {	
	  	    if (Broodwar->getKeyState(keyCode)) {	
  			  if (!keyState[keyCode]) {
			    jEnv->CallObjectMethod(classref, keyPressCallback, keyCode);
			  }
			  keyState[keyCode] = true;
			}
			else {
			  keyState[keyCode] = false;
			}
		  }
 
		  // draw commands
		  if (showHealth) drawHealth();
		  if (showTargets) drawTargets();
		  if (showIDs) drawIDs();
	  }

	  // wait for the next frame
      BWAPI::BWAPIClient.update();
      if (!BWAPI::BWAPIClient.isConnected())
      {
        javaPrint("Reconnecting...\n");
        reconnect();
      }
    }

	// game completed
    javaPrint("Game ended");
    jEnv->CallObjectMethod(classref, gameEndCallback);
  }
}

void reconnect()
{
  while(!BWAPIClient.connect())
  {
    Sleep(1000);
  }
}

void javaPrint(char* msg) 
{
  jEnv->CallObjectMethod(classref, printCallback, jEnv->NewStringUTF(msg));
}

// build type mappings
void loadTypeData() 
{
  std::set<UnitType> types = UnitTypes::allUnitTypes();
  for(std::set<UnitType>::iterator i=types.begin();i!=types.end();i++)
  {
	  unitTypeMap[i->getID()] = (*i);
  }

  std::set<TechType> techTypes = TechTypes::allTechTypes();
  for(std::set<TechType>::iterator i=techTypes.begin();i!=techTypes.end();i++)
  {
	  techTypeMap[i->getID()] = (*i);
  }

  std::set<UpgradeType> upgradeTypes = UpgradeTypes::allUpgradeTypes();
  for(std::set<UpgradeType>::iterator i=upgradeTypes.begin();i!=upgradeTypes.end();i++)
  {
	  upgradeTypeMap[i->getID()] = (*i);
  }

  std::set<WeaponType> weaponTypes = WeaponTypes::allWeaponTypes();
  for(std::set<WeaponType>::iterator i=weaponTypes.begin();i!=weaponTypes.end();i++)
  {
	  weaponTypeMap[i->getID()] = (*i);
  }

  std::set<UnitSizeType> unitSizeTypes = UnitSizeTypes::allUnitSizeTypes();
  for(std::set<UnitSizeType>::iterator i=unitSizeTypes.begin();i!=unitSizeTypes.end();i++)
  {
	  unitSizeTypeMap[i->getID()] = (*i);
  }

  std::set<BulletType> bulletTypes = BulletTypes::allBulletTypes();
  for(std::set<BulletType>::iterator i=bulletTypes.begin();i!=bulletTypes.end();i++)
  {
	  bulletTypeMap[i->getID()] = (*i);
  }

  std::set<DamageType> damageTypes = DamageTypes::allDamageTypes();
  for(std::set<DamageType>::iterator i=damageTypes.begin();i!=damageTypes.end();i++)
  {
	  damageTypeMap[i->getID()] = (*i);
  }

  std::set<ExplosionType> explosionTypes = ExplosionTypes::allExplosionTypes();
  for(std::set<ExplosionType>::iterator i=explosionTypes.begin();i!=explosionTypes.end();i++)
  {
	  explosionTypeMap[i->getID()] = (*i);
  }

  std::set<UnitCommandType> unitCommandTypes = UnitCommandTypes::allUnitCommandTypes();
  for(std::set<UnitCommandType>::iterator i=unitCommandTypes.begin();i!=unitCommandTypes.end();i++)
  {
	  unitCommandTypeMap[i->getID()] = (*i);
  }

  std::set<Order> orders = Orders::allOrders();
  for(std::set<Order>::iterator i=orders.begin();i!=orders.end();i++)
  {
	  orderTypeMap[i->getID()] = (*i);
  }
}

/*****************************************************************************************************************/
// Game options
/*****************************************************************************************************************/

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_drawHealth(JNIEnv *env, jobject jObj, jboolean enable)
{
	showHealth = enable;
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_drawTargets(JNIEnv *env, jobject jObj, jboolean enable)
{
	showTargets = enable;
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_drawIDs(JNIEnv *env, jobject jObj, jboolean enable)
{
	showIDs = enable;
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_enableUserInput(JNIEnv *env, jobject jObj)
{
	Broodwar->enableFlag(Flag::UserInput);
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_enablePerfectInformation(JNIEnv *env, jobject jObj) 
{
	Broodwar->enableFlag(Flag::CompleteMapInformation);
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_setGameSpeed(JNIEnv *env, jobject jObj, jint speed)
{
	Broodwar->setLocalSpeed(speed);
}

/*****************************************************************************************************************/
// Game state queries
/*****************************************************************************************************************/

JNIEXPORT jint JNICALL Java_eisbot_proxy_JNIBWAPI_getGameFrame(JNIEnv *env, jobject jObj) 
{
	return Broodwar->getFrameCount();
}

JNIEXPORT jintArray JNICALL Java_eisbot_proxy_JNIBWAPI_getPlayerInfo(JNIEnv *env, jobject jObj)
{
  int index = 0;

  std::set<Player*> players = Broodwar->getPlayers();
  if(Broodwar->isReplay())
  {
	  for(std::set<Player*>::iterator i=players.begin();i!=players.end();i++) {
		intBuf[index++] = (*i)->getID();
		intBuf[index++] = (*i)->getRace().getID();
		intBuf[index++] = (*i)->getType().getID();
		intBuf[index++] = 0;// there is no self in replay((*i)->getID() == Broodwar->self()->getID()) ? 1 : 0;	// is self?
		intBuf[index++] = 0;// as there is no self in replays, there is also no allies(*i)->isAlly(Broodwar->self()) ? 1 : 0;
		intBuf[index++] = 0;// as there is no self in replays, there is also no enemies(*i)->isEnemy(Broodwar->self()) ? 1 : 0;
		intBuf[index++] = (*i)->isNeutral();
		intBuf[index++] = (*i)->getColor().getID();
	  }
  }
  else
  {
	  for(std::set<Player*>::iterator i=players.begin();i!=players.end();i++) {
		intBuf[index++] = (*i)->getID();
		intBuf[index++] = (*i)->getRace().getID();
		intBuf[index++] = (*i)->getType().getID();
		intBuf[index++] = ((*i)->getID() == Broodwar->self()->getID()) ? 1 : 0;	// is self?
		intBuf[index++] = (*i)->isAlly(Broodwar->self()) ? 1 : 0;
		intBuf[index++] = (*i)->isEnemy(Broodwar->self()) ? 1 : 0;
		intBuf[index++] = (*i)->isNeutral();
		intBuf[index++] = (*i)->getColor().getID();
	  }
  }

  jintArray result =env->NewIntArray(index);
  env->SetIntArrayRegion(result, 0, index, intBuf);
  return result;
}
 
JNIEXPORT jintArray JNICALL Java_eisbot_proxy_JNIBWAPI_getPlayerUpdate(JNIEnv *env, jobject jObj, jint playerID)
{
  int index = 0;
  Player* p = Broodwar->getPlayer(playerID);
  intBuf[index++] = p->minerals();
  intBuf[index++] = p->gas();
  intBuf[index++] = p->supplyUsed();
  intBuf[index++] = p->supplyTotal();
  intBuf[index++] = p->gatheredMinerals();
  intBuf[index++] = p->gatheredGas();
  intBuf[index++] = p->getUnitScore();
  intBuf[index++] = p->getKillScore();
  intBuf[index++] = p->getBuildingScore();
  intBuf[index++] = p->getRazingScore();

  jintArray result =env->NewIntArray(index);
  env->SetIntArrayRegion(result, 0, index, intBuf);
  return result;
} 
 
JNIEXPORT jintArray JNICALL Java_eisbot_proxy_JNIBWAPI_getResearchStatus(JNIEnv *env, jobject jObj, jint playerID)
{
  int index = 0;
  Player* p = Broodwar->getPlayer(playerID);

  std::set<TechType> techTypes = TechTypes::allTechTypes();
  for(std::set<TechType>::iterator i=techTypes.begin();i!=techTypes.end();i++) {
	  intBuf[index++] = p->hasResearched((*i)) ? 1 : 0;
	  intBuf[index++] = p->isResearching((*i)) ? 1 : 0;
  }

  jintArray result =env->NewIntArray(index);
  env->SetIntArrayRegion(result, 0, index, intBuf);
  return result;
}

JNIEXPORT jintArray JNICALL Java_eisbot_proxy_JNIBWAPI_getUpgradeStatus(JNIEnv *env, jobject jObj, jint playerID)
{
  int index = 0;
  Player* p = Broodwar->getPlayer(playerID);

  std::set<UpgradeType> upTypes = UpgradeTypes::allUpgradeTypes();
  for(std::set<UpgradeType>::iterator i=upTypes.begin();i!=upTypes.end();i++) {
    intBuf[index++] = p->getUpgradeLevel((*i));
	intBuf[index++] = p->isUpgrading((*i)) ? 1 : 0;
  }
 
  jintArray result =env->NewIntArray(index);
  env->SetIntArrayRegion(result, 0, index, intBuf);
  return result;
}

JNIEXPORT jintArray JNICALL Java_eisbot_proxy_JNIBWAPI_getUnitTypes(JNIEnv *env, jobject jObj) 
{
  int index = 0;

  std::set<UnitType> types = UnitTypes::allUnitTypes();
  for(std::set<UnitType>::iterator i=types.begin();i!=types.end();i++)
  {
	intBuf[index++] = i->getID();
	intBuf[index++] = i->getRace().getID();
	intBuf[index++] = i->whatBuilds().first.getID();
	intBuf[index++] = i->armorUpgrade().getID();
	intBuf[index++] = i->maxHitPoints();
	intBuf[index++] = i->maxShields();
	intBuf[index++] = i->maxEnergy();
	intBuf[index++] = i->armor();
	intBuf[index++] = i->mineralPrice();
	intBuf[index++] = i->gasPrice();
	intBuf[index++] = i->buildTime();
	intBuf[index++] = i->supplyRequired();
	intBuf[index++] = i->supplyProvided();
	intBuf[index++] = i->spaceRequired();
	intBuf[index++] = i->spaceProvided();
	intBuf[index++] = i->buildScore();
	intBuf[index++] = i->destroyScore();
	intBuf[index++] = i->size().getID();
	intBuf[index++] = i->tileWidth();
	intBuf[index++] = i->tileHeight();
	intBuf[index++] = i->dimensionLeft();
	intBuf[index++] = i->dimensionUp();
	intBuf[index++] = i->dimensionRight();
	intBuf[index++] = i->dimensionDown();
	intBuf[index++] = i->seekRange();
	intBuf[index++] = i->sightRange();
	intBuf[index++] = i->groundWeapon().getID();
	intBuf[index++] = i->maxGroundHits();
	intBuf[index++] = i->airWeapon().getID();
	intBuf[index++] = i->maxAirHits();
	intBuf[index++] = (int)(i->topSpeed()*fixedScale);
	intBuf[index++] = i->acceleration();
	intBuf[index++] = i->haltDistance();
	intBuf[index++] = i->turnRadius();
	intBuf[index++] = i->canProduce() ? 1 : 0;
	intBuf[index++] = i->canAttack() ? 1 : 0;
	intBuf[index++] = i->canMove() ? 1 : 0;
	intBuf[index++] = i->isFlyer() ? 1 : 0;
	intBuf[index++] = i->regeneratesHP() ? 1 : 0;
	intBuf[index++] = i->isSpellcaster() ? 1 : 0;
	intBuf[index++] = i->isInvincible() ? 1 : 0;
	intBuf[index++] = i->isOrganic() ? 1 : 0;
	intBuf[index++] = i->isMechanical() ? 1 : 0;
	intBuf[index++] = i->isRobotic() ? 1 : 0;
	intBuf[index++] = i->isDetector() ? 1 : 0;
	intBuf[index++] = i->isResourceContainer() ? 1 : 0;
	intBuf[index++] = i->isRefinery() ? 1 : 0;
	intBuf[index++] = i->isWorker() ? 1 : 0;
	intBuf[index++] = i->requiresPsi() ? 1 : 0;
	intBuf[index++] = i->requiresCreep() ? 1 : 0;
	intBuf[index++] = i->isBurrowable() ? 1 : 0;
	intBuf[index++] = i->isCloakable() ? 1 : 0;
	intBuf[index++] = i->isBuilding() ? 1 : 0;
	intBuf[index++] = i->isAddon() ? 1 : 0;
	intBuf[index++] = i->isFlyingBuilding() ? 1 : 0;
	intBuf[index++] = i->isSpell() ? 1 : 0;

	// requiredUnits
	// requiredTech
	// cloakingTech
	// abilities
	// upgrades
	// isMineralField
	// isTwoUnitsInOneEgg
	// isNeutral
	// isHero
	// isPowerup
	// isBeacon
	// isFlagBeacon
	// isSpecialBuilding
  }

  jintArray result =env->NewIntArray(index);
  env->SetIntArrayRegion(result, 0, index, intBuf);
  return result;
}

JNIEXPORT jstring JNICALL Java_eisbot_proxy_JNIBWAPI_getUnitTypeName(JNIEnv *env, jobject jObj, jint typeID) 
{
	return jEnv->NewStringUTF(unitTypeMap[typeID].getName().c_str());
}

JNIEXPORT jintArray JNICALL Java_eisbot_proxy_JNIBWAPI_getTechTypes(JNIEnv *env, jobject jObj)
{
  int index = 0;

  std::set<TechType> techTypes = TechTypes::allTechTypes();
  for(std::set<TechType>::iterator i=techTypes.begin();i!=techTypes.end();i++)
  {
	intBuf[index++] = i->getID();
	intBuf[index++] = i->getRace().getID();
	intBuf[index++] = i->mineralPrice();
	intBuf[index++] = i->gasPrice();
	intBuf[index++] = i->researchTime();
	intBuf[index++] = i->energyUsed();
	intBuf[index++] = i->whatResearches().getID();
	intBuf[index++] = i->getWeapon().getID();
	intBuf[index++] = i->targetsUnit() ? 1 : 0;
	intBuf[index++] = i->targetsPosition() ? 1 : 0;

	// whatUses
  }

  jintArray result =env->NewIntArray(index);
  env->SetIntArrayRegion(result, 0, index, intBuf);
  return result;
}

JNIEXPORT jstring JNICALL Java_eisbot_proxy_JNIBWAPI_getTechTypeName(JNIEnv *env, jobject jObj, jint techID)
{
	return jEnv->NewStringUTF(techTypeMap[techID].getName().c_str());
}

JNIEXPORT jintArray JNICALL Java_eisbot_proxy_JNIBWAPI_getUpgradeTypes(JNIEnv *env, jobject jObj)
{
  int index = 0;

  std::set<UpgradeType> upgradeTypes = UpgradeTypes::allUpgradeTypes();
  for(std::set<UpgradeType>::iterator i=upgradeTypes.begin();i!=upgradeTypes.end();i++)
  {
	intBuf[index++] = i->getID();
	intBuf[index++] = i->getRace().getID();
	intBuf[index++] = i->mineralPrice();
	intBuf[index++] = i->mineralPriceFactor();
	intBuf[index++] = i->gasPrice();
	intBuf[index++] = i->gasPriceFactor(); 
	intBuf[index++] = i->upgradeTime();
	intBuf[index++] = i->upgradeTimeFactor();
	intBuf[index++] = i->maxRepeats();
	intBuf[index++] = i->whatUpgrades().getID();
  }
  
  jintArray result =env->NewIntArray(index);
  env->SetIntArrayRegion(result, 0, index, intBuf);
  return result;
}

JNIEXPORT jstring JNICALL Java_eisbot_proxy_JNIBWAPI_getUpgradeTypeName(JNIEnv *env, jobject jObj, jint upgradeID)
{
	return jEnv->NewStringUTF(upgradeTypeMap[upgradeID].getName().c_str());
}

JNIEXPORT jintArray JNICALL Java_eisbot_proxy_JNIBWAPI_getWeaponTypes(JNIEnv *env, jobject jObj)
{
  int index = 0;

  std::set<WeaponType> weaponTypes = WeaponTypes::allWeaponTypes();
  for(std::set<WeaponType>::iterator i=weaponTypes.begin();i!=weaponTypes.end();i++)
  {
	intBuf[index++] = i->getID();
	intBuf[index++] = i->getTech().getID();
	intBuf[index++] = i->whatUses().getID();
	intBuf[index++] = i->damageAmount();
	intBuf[index++] = i->damageBonus();
	intBuf[index++] = i->damageCooldown();
	intBuf[index++] = i->damageFactor();
	intBuf[index++] = i->upgradeType().getID();
	intBuf[index++] = i->damageType().getID();
	intBuf[index++] = i->explosionType().getID();
	intBuf[index++] = i->minRange();
	intBuf[index++] = i->maxRange();
	intBuf[index++] = i->innerSplashRadius();
	intBuf[index++] = i->medianSplashRadius();
	intBuf[index++] = i->outerSplashRadius();
	intBuf[index++] = i->targetsAir() ? 1 : 0;
	intBuf[index++] = i->targetsGround() ? 1 : 0;
	intBuf[index++] = i->targetsMechanical() ? 1 : 0;
	intBuf[index++] = i->targetsOrganic() ? 1 : 0;
	intBuf[index++] = i->targetsNonBuilding() ? 1 : 0;
	intBuf[index++] = i->targetsNonRobotic() ? 1 : 0;
	intBuf[index++] = i->targetsTerrain() ? 1 : 0;
	intBuf[index++] = i->targetsOrgOrMech() ? 1 : 0;
	intBuf[index++] = i->targetsOwn() ? 1 : 0;
  }
  
  jintArray result =env->NewIntArray(index);
  env->SetIntArrayRegion(result, 0, index, intBuf);
  return result;
}

JNIEXPORT jstring JNICALL Java_eisbot_proxy_JNIBWAPI_getWeaponTypeName(JNIEnv *env, jobject jObj, jint weaponID)
{
	return jEnv->NewStringUTF(weaponTypeMap[weaponID].getName().c_str());
}

JNIEXPORT jintArray JNICALL Java_eisbot_proxy_JNIBWAPI_getUnitSizeTypes(JNIEnv *env, jobject jObj) 
{
  int index = 0;

  std::set<UnitSizeType> unitSizeTypes = UnitSizeTypes::allUnitSizeTypes();
  for(std::set<UnitSizeType>::iterator i=unitSizeTypes.begin();i!=unitSizeTypes.end();i++)
  {
	intBuf[index++] = i->getID();
  }
  
  jintArray result =env->NewIntArray(index);
  env->SetIntArrayRegion(result, 0, index, intBuf);
  return result;
}

JNIEXPORT jstring JNICALL Java_eisbot_proxy_JNIBWAPI_getUnitSizeTypeName(JNIEnv *env, jobject jObj, jint sizeID)
{
	return jEnv->NewStringUTF(unitSizeTypeMap[sizeID].getName().c_str());
}

JNIEXPORT jintArray JNICALL Java_eisbot_proxy_JNIBWAPI_getBulletTypes(JNIEnv *env, jobject jObj)
{
  int index = 0;

  std::set<BulletType> bulletTypes = BulletTypes::allBulletTypes();
  for(std::set<BulletType>::iterator i=bulletTypes.begin();i!=bulletTypes.end();i++)
  {
	intBuf[index++] = i->getID();
  }
  
  jintArray result =env->NewIntArray(index);
  env->SetIntArrayRegion(result, 0, index, intBuf);
  return result;
}

JNIEXPORT jstring JNICALL Java_eisbot_proxy_JNIBWAPI_getBulletTypeName(JNIEnv *env, jobject jObj, jint bulletID)
{
	return jEnv->NewStringUTF(bulletTypeMap[bulletID].getName().c_str());
}

JNIEXPORT jintArray JNICALL Java_eisbot_proxy_JNIBWAPI_getDamageTypes(JNIEnv *env, jobject jObj)
{
  int index = 0;

  std::set<DamageType> damageTypes = DamageTypes::allDamageTypes();
  for(std::set<DamageType>::iterator i=damageTypes.begin();i!=damageTypes.end();i++)
  {
	intBuf[index++] = i->getID();
  }
  
  jintArray result =env->NewIntArray(index);
  env->SetIntArrayRegion(result, 0, index, intBuf);
  return result;
}

JNIEXPORT jstring JNICALL Java_eisbot_proxy_JNIBWAPI_getDamageTypeName(JNIEnv *env, jobject jObj, jint damageID)
{
	return jEnv->NewStringUTF(damageTypeMap[damageID].getName().c_str());
}

JNIEXPORT jintArray JNICALL Java_eisbot_proxy_JNIBWAPI_getExplosionTypes(JNIEnv *env, jobject jObj)
{
  int index = 0;

  std::set<ExplosionType> explosionTypes = ExplosionTypes::allExplosionTypes();
  for(std::set<ExplosionType>::iterator i=explosionTypes.begin();i!=explosionTypes.end();i++)
  {
	intBuf[index++] = i->getID();
  }
  
  jintArray result =env->NewIntArray(index);
  env->SetIntArrayRegion(result, 0, index, intBuf);
  return result;
}

JNIEXPORT jstring JNICALL Java_eisbot_proxy_JNIBWAPI_getExplosionTypeName(JNIEnv *env, jobject jObj, jint explosionID)
{
	return jEnv->NewStringUTF(explosionTypeMap[explosionID].getName().c_str());
}

JNIEXPORT jstring JNICALL Java_eisbot_proxy_JNIBWAPI_getUnitCommandTypeName(JNIEnv *env, jobject jObj, jint unitCommandID)
{
	return jEnv->NewStringUTF(unitCommandTypeMap[unitCommandID].getName().c_str());
}

JNIEXPORT jintArray JNICALL Java_eisbot_proxy_JNIBWAPI_getUnitCommandTypes(JNIEnv *env, jobject jObj)
{
  int index = 0;

  std::set<UnitCommandType> unitCommandTypes = UnitCommandTypes::allUnitCommandTypes();
  for(std::set<UnitCommandType>::iterator i=unitCommandTypes.begin();i!=unitCommandTypes.end();i++)
  {
	intBuf[index++] = i->getID();
  }

  jintArray result =env->NewIntArray(index);
  env->SetIntArrayRegion(result, 0, index, intBuf);
  return result;
}


JNIEXPORT jstring JNICALL Java_eisbot_proxy_JNIBWAPI_getOrderTypeName(JNIEnv *env, jobject jObj, jint unitCommandID)
{
	return jEnv->NewStringUTF(orderTypeMap[unitCommandID].getName().c_str());
}

JNIEXPORT jintArray JNICALL Java_eisbot_proxy_JNIBWAPI_getOrderTypes(JNIEnv *env, jobject jObj)
{
  int index = 0;

  std::set<Order> orders = Orders::allOrders();
  for(std::set<Order>::iterator i=orders.begin();i!=orders.end();i++)
  {
	intBuf[index++] = i->getID();
  }

  jintArray result =env->NewIntArray(index);
  env->SetIntArrayRegion(result, 0, index, intBuf);
  return result;
}


/**
 * Returns the list of active units in the game. 
 *
 * Each unit takes up a fixed number of integer values. Currently: 112
 */
JNIEXPORT jintArray JNICALL Java_eisbot_proxy_JNIBWAPI_getUnits(JNIEnv *env, jobject jObj) 
{
  int index = 0;

  std::set<Unit*> units = Broodwar->getAllUnits();
  for(std::set<Unit*>::iterator i=units.begin();i!=units.end();i++) {
	  intBuf[index++] = (*i)->getID();
	  intBuf[index++] = (*i)->getPlayer()->getID();
	  intBuf[index++] = (*i)->getType().getID();
	  intBuf[index++] = (*i)->getPosition().x();
	  intBuf[index++] = (*i)->getPosition().y();
	  intBuf[index++] = (*i)->getTilePosition().x();
	  intBuf[index++] = (*i)->getTilePosition().y();
	  intBuf[index++] = (int)(TO_DEGREES*(*i)->getAngle());
	  intBuf[index++] = (int)(fixedScale*(*i)->getVelocityX());
	  intBuf[index++] = (int)(fixedScale*(*i)->getVelocityY());
	  intBuf[index++] = (*i)->getHitPoints();
	  intBuf[index++] = (*i)->getShields();
	  intBuf[index++] = (*i)->getEnergy();
	  intBuf[index++] = (*i)->getResources();
	  intBuf[index++] = (*i)->getResourceGroup();
	  intBuf[index++] = (*i)->getLastCommandFrame();
	  intBuf[index++] = (*i)->getLastCommand().getType().getID();
	  intBuf[index++] = (*i)->getInitialType().getID();
	  intBuf[index++] = (*i)->getInitialPosition().x();
	  intBuf[index++] = (*i)->getInitialPosition().y();
	  intBuf[index++] = (*i)->getInitialTilePosition().x();
	  intBuf[index++] = (*i)->getInitialTilePosition().y();
	  intBuf[index++] = (*i)->getInitialHitPoints();
	  intBuf[index++] = (*i)->getInitialResources();
	  intBuf[index++] = (*i)->getKillCount();
	  intBuf[index++] = (*i)->getAcidSporeCount();
	  intBuf[index++] = (*i)->getInterceptorCount();
	  intBuf[index++] = (*i)->getScarabCount();
	  intBuf[index++] = (*i)->getSpiderMineCount();
	  intBuf[index++] = (*i)->getGroundWeaponCooldown();
	  intBuf[index++] = (*i)->getAirWeaponCooldown();
	  intBuf[index++] = (*i)->getSpellCooldown();
	  intBuf[index++] = (*i)->getDefenseMatrixPoints();
	  intBuf[index++] = (*i)->getDefenseMatrixTimer();
	  intBuf[index++] = (*i)->getEnsnareTimer();
	  intBuf[index++] = (*i)->getIrradiateTimer();
	  intBuf[index++] = (*i)->getLockdownTimer();
	  intBuf[index++] = (*i)->getMaelstromTimer();
	  intBuf[index++] = (*i)->getOrderTimer();
	  intBuf[index++] = (*i)->getPlagueTimer();
	  intBuf[index++] = (*i)->getRemoveTimer();
	  intBuf[index++] = (*i)->getStasisTimer();
	  intBuf[index++] = (*i)->getStimTimer();
	  intBuf[index++] = (*i)->getBuildType().getID();
	  intBuf[index++] = (*i)->getTrainingQueue().size();
	  intBuf[index++] = (*i)->getTech().getID();
	  intBuf[index++] = (*i)->getUpgrade().getID();
	  intBuf[index++] = (*i)->getRemainingBuildTime();
	  intBuf[index++] = (*i)->getRemainingTrainTime();
	  intBuf[index++] = (*i)->getRemainingResearchTime();
	  intBuf[index++] = (*i)->getRemainingUpgradeTime();
	  intBuf[index++] = ((*i)->getBuildUnit() != NULL) ? (*i)->getBuildUnit()->getID() : -1;
	  intBuf[index++] = ((*i)->getTarget() != NULL) ? (*i)->getTarget()->getID() : -1;
	  intBuf[index++] = (*i)->getTargetPosition().x();
	  intBuf[index++] = (*i)->getTargetPosition().y();
	  intBuf[index++] = (*i)->getOrder().getID();
	  intBuf[index++] = ((*i)->getOrderTarget() != NULL) ? (*i)->getOrderTarget()->getID() : -1;
	  intBuf[index++] = (*i)->getSecondaryOrder().getID();
	  intBuf[index++] = (*i)->getRallyPosition().x();
	  intBuf[index++] = (*i)->getRallyPosition().y();
	  intBuf[index++] = ((*i)->getRallyUnit() != NULL) ? (*i)->getRallyUnit()->getID() : -1;
	  intBuf[index++] = ((*i)->getAddon() != NULL) ? (*i)->getAddon()->getID() : -1;
	  intBuf[index++] = ((*i)->getTransport() != NULL) ? (*i)->getTransport()->getID() : -1;
	  intBuf[index++] = (*i)->getLoadedUnits().size();
	  intBuf[index++] = (*i)->getLarva().size();
	  intBuf[index++] = (*i)->exists() ? 1 : 0;
	  intBuf[index++] = (*i)->hasNuke() ? 1 : 0;
	  intBuf[index++] = (*i)->isAccelerating() ? 1 : 0;
	  intBuf[index++] = (*i)->isAttacking() ? 1 : 0;
	  intBuf[index++] = (*i)->isAttackFrame() ? 1 : 0;
	  intBuf[index++] = (*i)->isBeingConstructed() ? 1 : 0;
	  intBuf[index++] = (*i)->isBeingGathered() ? 1 : 0;
	  intBuf[index++] = (*i)->isBeingHealed() ? 1 : 0;
	  intBuf[index++] = (*i)->isBlind() ? 1 : 0;
	  intBuf[index++] = (*i)->isBraking() ? 1 : 0;
	  intBuf[index++] = (*i)->isBurrowed() ? 1 : 0;
	  intBuf[index++] = (*i)->isCarryingGas() ? 1 : 0;
	  intBuf[index++] = (*i)->isCarryingMinerals() ? 1 : 0;
	  intBuf[index++] = (*i)->isCloaked() ? 1 : 0;
	  intBuf[index++] = (*i)->isCompleted() ? 1 : 0;
	  intBuf[index++] = (*i)->isDefenseMatrixed() ? 1 : 0;
	  intBuf[index++] = (*i)->isDetected() ? 1 : 0;
	  intBuf[index++] = (*i)->isEnsnared() ? 1 : 0;
	  intBuf[index++] = (*i)->isFollowing() ? 1 : 0;
	  intBuf[index++] = (*i)->isGatheringGas() ? 1 : 0;
	  intBuf[index++] = (*i)->isGatheringMinerals() ? 1 : 0;
	  intBuf[index++] = (*i)->isHallucination() ? 1 : 0;
	  intBuf[index++] = (*i)->isHoldingPosition() ? 1 : 0;
	  intBuf[index++] = (*i)->isIdle() ? 1 : 0;
	  intBuf[index++] = (*i)->isInterruptible() ? 1 : 0;
	  intBuf[index++] = (*i)->isInvincible() ? 1 : 0;
	  intBuf[index++] = (*i)->isIrradiated() ? 1 : 0;
	  intBuf[index++] = (*i)->isLifted() ? 1 : 0;
	  intBuf[index++] = (*i)->isLoaded() ? 1 : 0;
	  intBuf[index++] = (*i)->isLockedDown() ? 1 : 0;
	  intBuf[index++] = (*i)->isMaelstrommed() ? 1 : 0;
	  intBuf[index++] = (*i)->isMorphing() ? 1 : 0;
	  intBuf[index++] = (*i)->isMoving() ? 1 : 0;
	  intBuf[index++] = (*i)->isParasited() ? 1 : 0;
	  intBuf[index++] = (*i)->isPatrolling() ? 1 : 0;
	  intBuf[index++] = (*i)->isPlagued() ? 1 : 0;
	  intBuf[index++] = (*i)->isRepairing() ? 1 : 0;
	  intBuf[index++] = (*i)->isSieged() ? 1 : 0;
	  intBuf[index++] = (*i)->isStartingAttack() ? 1 : 0;
	  intBuf[index++] = (*i)->isStasised() ? 1 : 0;
	  intBuf[index++] = (*i)->isStimmed() ? 1 : 0;
	  intBuf[index++] = (*i)->isStuck() ? 1 : 0;
	  intBuf[index++] = (*i)->isTraining() ? 1 : 0;
	  intBuf[index++] = (*i)->isUnderStorm() ? 1 : 0;
	  intBuf[index++] = (*i)->isUnpowered() ? 1 : 0;
	  intBuf[index++] = (*i)->isUpgrading() ? 1 : 0;
	  intBuf[index++] = (*i)->isVisible() ? 1 : 0;
	  // last attacking player
	  // Nydus exit
	  // Carrier
	  // Interceptors
	  // Hatchery
      // Power up
	  // intBuf[index++] = (*i)->isUnderAttack() ? 1 : 0;
	  // intBuf[index++] = (*i)->isUnderDarkSwarm() ? 1 : 0;
	  // intBuf[index++] = (*i)->sUnderDisruptionWeb() ? 1 : 0;
  }

  jintArray result = env->NewIntArray(index);
  env->SetIntArrayRegion(result, 0, index, intBuf);
  return result;
}

/*****************************************************************************************************************/
// Map queries
/*****************************************************************************************************************/

JNIEXPORT jint JNICALL Java_eisbot_proxy_JNIBWAPI_getMapWidth(JNIEnv *env, jobject jObj)
{
	return Broodwar->mapWidth();
}

JNIEXPORT jint JNICALL Java_eisbot_proxy_JNIBWAPI_getMapHeight(JNIEnv *env, jobject jObj)
{
	return Broodwar->mapHeight();
}

JNIEXPORT jstring JNICALL Java_eisbot_proxy_JNIBWAPI_getMapName(JNIEnv *env, jobject jObj)
{
	return jEnv->NewStringUTF(Broodwar->mapFileName().c_str());
}

JNIEXPORT jstring JNICALL Java_eisbot_proxy_JNIBWAPI_getMapHash(JNIEnv *env, jobject jObj)
{
	return jEnv->NewStringUTF(Broodwar->mapHash().c_str());
}

JNIEXPORT jintArray JNICALL Java_eisbot_proxy_JNIBWAPI_getHeightData(JNIEnv *env, jobject jObj)
{
  int index = 0;
  int width = Broodwar->mapWidth();
  int height = Broodwar->mapHeight();

  for (int ty=0; ty<height; ty++) {
	  for (int tx=0; tx<width; tx++) {
		  intBuf[index++] = Broodwar->getGroundHeight(tx, ty);
	  }
  }

  jintArray result =env->NewIntArray(index);
  env->SetIntArrayRegion(result, 0, index, intBuf);
  return result;
}

JNIEXPORT jintArray JNICALL Java_eisbot_proxy_JNIBWAPI_getWalkableData(JNIEnv *env, jobject jObj)
{
  // Note: walk tiles are 8x8 pixels, build tiles are 32x32 pixels
  int index = 0;	
  int width = 4*Broodwar->mapWidth();
  int height = 4*Broodwar->mapHeight();

  for (int ty=0; ty<height; ty++) {
	  for (int tx=0; tx<width; tx++) {
		  intBuf[index++] = Broodwar->isWalkable(tx, ty) ? 1 : 0;
	  }
  }
 
  jintArray result =env->NewIntArray(index);
  env->SetIntArrayRegion(result, 0, index, intBuf);
  return result;
}

JNIEXPORT jintArray JNICALL Java_eisbot_proxy_JNIBWAPI_getBuildableData(JNIEnv *env, jobject jObj)
{
  int index = 0;
  int width = Broodwar->mapWidth();
  int height = Broodwar->mapHeight();

  for (int ty=0; ty<height; ty++) {
	  for (int tx=0; tx<width; tx++) {
		  intBuf[index++] = Broodwar->isBuildable(tx, ty) ? 1 : 0;
	  }
  }

  jintArray result =env->NewIntArray(index);
  env->SetIntArrayRegion(result, 0, index, intBuf);
  return result;
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_analyzeTerrain(JNIEnv *env, jobject jObj)
{
	regionMap.clear();
	BWTA::readMap();
	BWTA::analyze();

	// assign IDs to regions
    int regionID = 1;
	std::set<BWTA::Region*> regions = BWTA::getRegions();
	for (std::set<BWTA::Region*>::iterator i=regions.begin();i!=regions.end();i++)
	{
		regionMap[(*i)] = regionID;
		regionID++;
	}
}

JNIEXPORT jintArray JNICALL Java_eisbot_proxy_JNIBWAPI_getChokePoints(JNIEnv *env, jobject jObj)
{
  int index = 0;

  std::set<BWTA::Chokepoint*> chokepoints = BWTA::getChokepoints();
  for (std::set<BWTA::Chokepoint*>::iterator i=chokepoints.begin();i!=chokepoints.end();i++)
  {
	  intBuf[index++] = (*i)->getCenter().x();
	  intBuf[index++] = (*i)->getCenter().y();
	  intBuf[index++] = (int)((*i)->getWidth()*fixedScale);
	  intBuf[index++] = regionMap[(*i)->getRegions().first];
	  intBuf[index++] = regionMap[(*i)->getRegions().second];
	  intBuf[index++] = (*i)->getSides().first.x();
	  intBuf[index++] = (*i)->getSides().first.y();
	  intBuf[index++] = (*i)->getSides().second.x();
	  intBuf[index++] = (*i)->getSides().second.y();
  }

  jintArray result =env->NewIntArray(index);
  env->SetIntArrayRegion(result, 0, index, intBuf);
  return result;
}

JNIEXPORT jintArray JNICALL Java_eisbot_proxy_JNIBWAPI_getRegions(JNIEnv *env, jobject jObj)
{
  int index = 0;

  std::set<BWTA::Region*> regions = BWTA::getRegions();
  for (std::set<BWTA::Region*>::iterator i=regions.begin();i!=regions.end();i++)
  {
	  intBuf[index++] = regionMap[(*i)];
	  intBuf[index++] = (*i)->getCenter().x();
	  intBuf[index++] = (*i)->getCenter().y();
  }

  jintArray result =env->NewIntArray(index);
  env->SetIntArrayRegion(result, 0, index, intBuf);
  return result;
} 

JNIEXPORT jintArray JNICALL Java_eisbot_proxy_JNIBWAPI_getPolygon(JNIEnv *env, jobject jObj, jint regionID)
{
  int index = 0;
  std::set<BWTA::Region*> regions = BWTA::getRegions();
  for (std::set<BWTA::Region*>::iterator i=regions.begin();i!=regions.end();i++)
  {
    if (regionID == regionMap[(*i)]) {
	  for (unsigned int j=0; j<(*i)->getPolygon().size(); j++) {
		intBuf[index++] = (*i)->getPolygon()[j].x();
		intBuf[index++] = (*i)->getPolygon()[j].y();
  	  }
	}
  }

  jintArray result =env->NewIntArray(index);
  env->SetIntArrayRegion(result, 0, index, intBuf);
  return result;
}

JNIEXPORT jintArray JNICALL Java_eisbot_proxy_JNIBWAPI_getBaseLocations(JNIEnv *env, jobject jObj)
{
  int index = 0;

  std::set<BWTA::BaseLocation*> baseLocation =  BWTA::getBaseLocations();
  for (std::set<BWTA::BaseLocation*>::iterator i=baseLocation.begin();i!=baseLocation.end();i++)
  {
	  intBuf[index++] = (*i)->getPosition().x();
	  intBuf[index++] = (*i)->getPosition().y();
	  intBuf[index++] = (*i)->getTilePosition().x();
	  intBuf[index++] = (*i)->getTilePosition().y();
	  intBuf[index++] = regionMap[(*i)->getRegion()];
	  intBuf[index++] = (*i)->minerals();
	  intBuf[index++] = (*i)->gas();
	  intBuf[index++] = (*i)->isIsland() ? 1 : 0;
	  intBuf[index++] = (*i)->isMineralOnly() ? 1 : 0;
	  intBuf[index++] = (*i)->isStartLocation() ? 1 : 0;
  }

  jintArray result =env->NewIntArray(index);
  env->SetIntArrayRegion(result, 0, index, intBuf);
  return result;
}

/*****************************************************************************************************************/
// Unit Commands
/*****************************************************************************************************************/

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_attack__III(JNIEnv *env, jobject jObj, jint unitID, jint x, jint y) {
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		unit->attack(BWAPI::Position(x, y), false); 
	}
}
 
JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_attack__II(JNIEnv *env, jobject jObj, jint unitID, jint targetID){
	Unit* unit = Broodwar->getUnit(unitID);
	Unit* target = Broodwar->getUnit(targetID);
	if (unit != NULL && target != NULL) {
		unit->attack(target, false);
	}
}
 
JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_build(JNIEnv *env, jobject jObj, jint unitID, jint tx, jint ty, jint typeID){ 
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		if (unitTypeMap.count(typeID) > 0) {
			unit->build(BWAPI::TilePosition(tx, ty), unitTypeMap[typeID]);
		}
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_buildAddon(JNIEnv *env, jobject jObj, jint unitID, jint typeID){ 
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		if (unitTypeMap.count(typeID) > 0) {
			unit->buildAddon(unitTypeMap[typeID]);
		}
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_train(JNIEnv *env, jobject jObj, jint unitID, jint typeID){ 
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		if (unitTypeMap.count(typeID) > 0)
		{
			unit->train(unitTypeMap[typeID]);
		}
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_morph(JNIEnv *env, jobject jObj, jint unitID, jint typeID){ 
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		if (unitTypeMap.count(typeID) > 0) {
			unit->morph(unitTypeMap[typeID]);
		}
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_research(JNIEnv *env, jobject jObj, jint unitID, jint techID){ 
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		if (techTypeMap.count(techID) > 0) {
			unit->research(techTypeMap[techID]);
		}
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_upgrade(JNIEnv *env, jobject jObj, jint unitID, jint upgradeID){ 
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		if (upgradeTypeMap.count(upgradeID) > 0) {
			unit->upgrade(upgradeTypeMap[upgradeID]);
		}
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_setRallyPoint__III(JNIEnv *env, jobject jObj, jint unitID, jint x, jint y){ 
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		unit->setRallyPoint(BWAPI::Position(x, y));
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_setRallyPoint__II(JNIEnv *env, jobject jObj, jint unitID, jint targetID){ 
	Unit* unit = Broodwar->getUnit(unitID);
	Unit* target = Broodwar->getUnit(targetID);
	if (unit != NULL && target != NULL) {
		unit->setRallyPoint(target);
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_move(JNIEnv *env, jobject jObj, jint unitID, jint x, jint y){ 
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		unit->move(BWAPI::Position(x, y));
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_patrol(JNIEnv *env, jobject jObj, jint unitID, jint x, jint y){ 
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		unit->patrol(BWAPI::Position(x, y));
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_holdPosition(JNIEnv *env, jobject jObj, jint unitID){ 
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		unit->holdPosition();
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_stop(JNIEnv *env, jobject jObj, jint unitID){ 
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		unit->stop();
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_follow(JNIEnv *env, jobject jObj, jint unitID, jint targetID){ 
	Unit* unit = Broodwar->getUnit(unitID);
	Unit* target = Broodwar->getUnit(targetID);
	if (unit != NULL && target != NULL) {
		unit->follow(target);
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_gather(JNIEnv *env, jobject jObj, jint unitID, jint targetID){ 
	Unit* unit = Broodwar->getUnit(unitID);
	Unit* target = Broodwar->getUnit(targetID);
	if (unit != NULL && target != NULL) {
		unit->gather(target);
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_returnCargo(JNIEnv *env, jobject jObj, jint unitID){ 
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		unit->returnCargo();
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_repair(JNIEnv *env, jobject jObj, jint unitID, jint targetID){ 
	Unit* unit = Broodwar->getUnit(unitID);
	Unit* target = Broodwar->getUnit(targetID);
	if (unit != NULL && target != NULL) {
		unit->repair(target);
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_burrow(JNIEnv *env, jobject jObj, jint unitID){ 
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		unit->burrow();
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_unburrow(JNIEnv *env, jobject jObj, jint unitID){ 
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		unit->unburrow();
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_cloak(JNIEnv *env, jobject jObj, jint unitID){ 
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		unit->cloak();
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_decloak(JNIEnv *env, jobject jObj, jint unitID){ 
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		unit->decloak();
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_siege(JNIEnv *env, jobject jObj, jint unitID){ 
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		unit->siege();
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_unsiege(JNIEnv *env, jobject jObj, jint unitID){ 
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		unit->unsiege();
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_lift(JNIEnv *env, jobject jObj, jint unitID){ 
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		unit->lift();
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_land(JNIEnv *env, jobject jObj, jint unitID, jint tx, jint ty){ 
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		unit->land(BWAPI::TilePosition(tx, ty));
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_load(JNIEnv *env, jobject jObj, jint unitID, jint targetID){ 
	Unit* unit = Broodwar->getUnit(unitID);
	Unit* target = Broodwar->getUnit(targetID);
	if (unit != NULL && target != NULL) {
		unit->load(target);
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_unload(JNIEnv *env, jobject jObj, jint unitID, jint targetID){ 
	Unit* unit = Broodwar->getUnit(unitID);
	Unit* target = Broodwar->getUnit(targetID);
	if (unit != NULL && target != NULL) {
		unit->unload(target);
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_unloadAll__I(JNIEnv *env, jobject jObj, jint unitID){ 
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		unit->unloadAll();
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_unloadAll__III(JNIEnv *env, jobject jObj, jint unitID, jint x, jint y){ 
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		unit->unloadAll(BWAPI::Position(x, y));
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_rightClick__III(JNIEnv *env, jobject jObj, jint unitID, jint x, jint y){ 
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		unit->rightClick(BWAPI::Position(x, y));
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_rightClick__II(JNIEnv *env, jobject jObj, jint unitID, jint targetID){ 
	Unit* unit = Broodwar->getUnit(unitID);
	Unit* target = Broodwar->getUnit(targetID);
	if (unit != NULL && target != NULL) {
		unit->rightClick(target);
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_haltConstruction(JNIEnv *env, jobject jObj, jint unitID){ 
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		unit->haltConstruction();
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_cancelConstruction(JNIEnv *env, jobject jObj, jint unitID){ 
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		unit->cancelConstruction();
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_cancelAddon(JNIEnv *env, jobject jObj, jint unitID){ 
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		unit->cancelAddon();
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_cancelTrain(JNIEnv *env, jobject jObj, jint unitID, jint slot){ 
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		unit->cancelTrain(slot);
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_cancelMorph(JNIEnv *env, jobject jObj, jint unitID){ 
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		unit->cancelMorph();
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_cancelResearch(JNIEnv *env, jobject jObj, jint unitID){ 
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		unit->cancelResearch();
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_cancelUpgrade(JNIEnv *env, jobject jObj, jint unitID){ 
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		unit->cancelUpgrade();
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_useTech__II(JNIEnv *env, jobject jObj, jint unitID, jint techID){ 
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		if (techTypeMap.count(techID) > 0) {
			unit->useTech(techTypeMap[techID]);
		}
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_useTech__IIII(JNIEnv *env, jobject jObj, jint unitID, jint techID, jint x, jint y){ 
	Unit* unit = Broodwar->getUnit(unitID);
	if (unit != NULL) {
		if (techTypeMap.count(techID) > 0) {
			unit->useTech(techTypeMap[techID], BWAPI::Position(x, y));
		}			
	}
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_useTech__III(JNIEnv *env, jobject jObj, jint unitID, jint techID, jint targetID){ 
	Unit* unit = Broodwar->getUnit(unitID);
	Unit* target = Broodwar->getUnit(targetID);
	if (unit != NULL && target != NULL) {
		if (techTypeMap.count(techID) > 0) {
			unit->useTech(techTypeMap[techID], target);
		}
	}
}

/*****************************************************************************************************************/
// Utility functions
/*****************************************************************************************************************/

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_drawBox(JNIEnv *env, jobject jObj, jint left, jint top, jint right, jint bottom, jint color, jboolean fill, jboolean screenCoords)
{
  if (screenCoords) {
	  Broodwar->drawBoxScreen(left, top, right, bottom, BWAPI::Color(color), fill ? true : false);
  }
  else {
	  Broodwar->drawBoxMap(left, top, right, bottom, BWAPI::Color(color), fill ? true : false);
  }
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_drawCircle(JNIEnv *env, jobject jObj, jint x, jint y, jint radius, jint color, jboolean fill, jboolean screenCoords)
{
  if (screenCoords) {
	  Broodwar->drawCircleScreen(x, y, radius, BWAPI::Color(color), fill ? true : false);
  }
  else {
	  Broodwar->drawCircleMap(x, y, radius, BWAPI::Color(color), fill ? true : false);
  }
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_drawLine(JNIEnv *env, jobject jObj, jint x1, jint y1, jint x2, jint y2, jint color, jboolean screenCoords)
{
  if (screenCoords) {
	  Broodwar->drawLineScreen(x1, y1, x2, y2, BWAPI::Color(color));
  }
  else {
	  Broodwar->drawLineMap(x1, y1, x2, y2, BWAPI::Color(color));
  }
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_drawDot(JNIEnv *env, jobject jObj, jint x, jint y, jint color, jboolean screenCoords)
{
  if (screenCoords) {
	  Broodwar->drawDotScreen(x, y, BWAPI::Color(color));	  
  }
  else {
	  Broodwar->drawDotMap(x, y, BWAPI::Color(color));	  
  }
}

JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_drawText(JNIEnv *env, jobject jObj, jint x, jint y, jstring msg, jboolean screenCoords)
{
  const char *message = env->GetStringUTFChars(msg, 0);

  if (screenCoords) {
	  Broodwar->drawTextScreen(x, y, message);
  }
  else {
	  Broodwar->drawTextMap(x, y, message);
  }

  env->ReleaseStringUTFChars(msg, message);
}

/**
 * Draws health boxes for units
 */
void drawHealth() 
{
	std::set<Unit*> units;
	if(Broodwar->isReplay())
	{
		units = Broodwar->self()->getUnits();
	}
	else
	{
		units = Broodwar->getAllUnits();
	}
	for(std::set<Unit*>::iterator i=units.begin();i!=units.end();i++) {
		int health = (*i)->getHitPoints();

		if (health > 0) {
			int x = (*i)->getPosition().x();
			int y = (*i)->getPosition().y();
			int l = (*i)->getType().dimensionLeft();
			int t = (*i)->getType().dimensionUp();
			int r = (*i)->getType().dimensionRight();
			int b = (*i)->getType().dimensionDown();
			int max = (*i)->getType().maxHitPoints();
			int width = ((r + l)*health)/max;

			if (health*3<max) {
				Broodwar->drawBoxMap(x - l, y - t - 5, x - l + width, y - t, BWAPI::Colors::Red, true);
			}
			else if (health*3<2*max) {
				Broodwar->drawBoxMap(x - l, y - t - 5, x - l + width, y - t, BWAPI::Colors::Yellow, true);
			}
			else {
				Broodwar->drawBoxMap(x - l, y - t - 5, x - l + width, y - t, BWAPI::Colors::Green, true);
			}

			Broodwar->drawBoxMap(x - l, y - t - 5, x + r, y - t, BWAPI::Colors::White, false);
			Broodwar->drawBoxMap(x - l, y - t, x + r, y + b, BWAPI::Colors::White, false);
		}
	}

	units = Broodwar->enemy()->getUnits();
	for(std::set<Unit*>::iterator i=units.begin();i!=units.end();i++) {
		int health = (*i)->getHitPoints();

		if (health > 0) {
			int x = (*i)->getPosition().x();
			int y = (*i)->getPosition().y();
			int l = (*i)->getType().dimensionLeft();
			int t = (*i)->getType().dimensionUp();
			int r = (*i)->getType().dimensionRight();
			int b = (*i)->getType().dimensionDown();
			int max = (*i)->getType().maxHitPoints();
			int width = ((r + l)*health)/max;

			if (health*3<max) {
				Broodwar->drawBoxMap(x - l, y - t - 5, x - l + width, y - t, BWAPI::Colors::Red, true);
			}
			else if (health*3<2*max) {
				Broodwar->drawBoxMap(x - l, y - t - 5, x - l + width, y - t, BWAPI::Colors::Yellow, true);
			}
			else {
				Broodwar->drawBoxMap(x - l, y - t - 5, x - l + width, y - t, BWAPI::Colors::Green, true);
			}

			Broodwar->drawBoxMap(x - l, y - t - 5, x + r, y - t, BWAPI::Colors::Red, false);
			Broodwar->drawBoxMap(x - l, y - t, x + r, y + b, BWAPI::Colors::Red, false);
			Broodwar->drawTextMap(x - l, y - t, (*i)->getType().getName().c_str());
		}
	}
}
 
/**
 * Draws the targets of each unit.
 */
void drawTargets() {
	std::set<Unit*> units;
	if(!Broodwar->isReplay())
	{
		units = Broodwar->self()->getUnits();
	}
	else
	{
		units = Broodwar->getAllUnits();
	}
	for(std::set<Unit*>::iterator i=units.begin();i!=units.end();i++) {
		Unit* target = (*i)->getTarget(); 
		if (target != NULL) {
			Broodwar->drawLineMap((*i)->getPosition().x(), (*i)->getPosition().y(), 
				target->getPosition().x(), target->getPosition().y(), BWAPI::Colors::Yellow);
		}

		target = (*i)->getOrderTarget(); 
		if (target != NULL) {
			Broodwar->drawLineMap((*i)->getPosition().x(), (*i)->getPosition().y(), 
				target->getPosition().x(), target->getPosition().y(), BWAPI::Colors::Yellow);
		}

		Position position = (*i)->getTargetPosition();
		Broodwar->drawLineMap((*i)->getPosition().x(), (*i)->getPosition().y(), 
			position.x(), position.y(), BWAPI::Colors::Yellow);
	}	

	units = Broodwar->enemy()->getUnits();
	for(std::set<Unit*>::iterator i=units.begin();i!=units.end();i++) {
		Unit* target = (*i)->getTarget(); 
		if (target != NULL) {
			Broodwar->drawLineMap((*i)->getPosition().x(), (*i)->getPosition().y(), 
				target->getPosition().x(), target->getPosition().y(), BWAPI::Colors::Purple);
		}

		target = (*i)->getOrderTarget(); 
		if (target != NULL) {
			Broodwar->drawLineMap((*i)->getPosition().x(), (*i)->getPosition().y(), 
				target->getPosition().x(), target->getPosition().y(), BWAPI::Colors::Purple);
		}

		Position position = (*i)->getTargetPosition(); 
		Broodwar->drawLineMap((*i)->getPosition().x(), (*i)->getPosition().y(), 
			position.x(), position.y(), BWAPI::Colors::Purple);
	}	
}

/**
 * Draws the IDs of each unit.
 */
void drawIDs() {

	std::set<Unit*> units = Broodwar->getAllUnits();
	for(std::set<Unit*>::iterator i=units.begin();i!=units.end();i++) {
		int x = (*i)->getPosition().x();
		int y = (*i)->getPosition().y();

		Broodwar->drawTextMap(x, y, "%i", (*i)->getID());
	}	
}


/////////////HAS CREEP DEFINITION///////////////
JNIEXPORT jboolean JNICALL Java_eisbot_proxy_JNIBWAPI_hasCreep(JNIEnv *env, jobject jObj, jint tx, jint ty)
{
	return Broodwar->hasCreep(tx, ty);
}

/////////////canBuildHere Definition/////////////
JNIEXPORT jboolean JNICALL Java_eisbot_proxy_JNIBWAPI_canBuildHere(JNIEnv *env, jobject jObj, jint unitID, jint tx, jint ty, jint utypeID, jboolean checkExplored)
{
	if(unitTypeMap.count(utypeID) > 0)
	{
		BWAPI::UnitType typ = unitTypeMap[utypeID];
		BWAPI::Unit* un = Broodwar->getUnit(unitID);
		bool checkex = false;
		if(checkExplored)
		{
		    checkex = true;
		}
		return Broodwar->canBuildHere(un, TilePosition(tx, ty), typ, checkex);
		
	}
	else
	{
		return false;
	}
}

/////////////isReplay defn//////////////
JNIEXPORT jboolean JNICALL Java_eisbot_proxy_JNIBWAPI_isReplay
  (JNIEnv *, jobject jObj)
{
	return Broodwar->isReplay();
}

/*
 * Class:     eisbot_proxy_JNIBWAPI
 * Method:    printText
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_printText
  (JNIEnv * env, jobject jObj, jstring message)
{
        const char *messagechars = env->GetStringUTFChars(message, 0);
        Broodwar->printf(messagechars);
        env->ReleaseStringUTFChars(message, messagechars);
}

/*
 * Class:     eisbot_proxy_JNIBWAPI
 * Method:    sendText
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_sendText
  (JNIEnv * env, jobject jObj, jstring message)
{
        const char *messagechars = env->GetStringUTFChars(message, 0);
		Broodwar->sendText(messagechars);
		env->ReleaseStringUTFChars(message, messagechars);
}


/*
 * Class:     eisbot_proxy_JNIBWAPI
 * Method:    setCommandOptimizationLevel
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_eisbot_proxy_JNIBWAPI_setCommandOptimizationLevel (JNIEnv * env, jobject jObj, jint level)
{
	Broodwar->setCommandOptimizationLevel(level);
}