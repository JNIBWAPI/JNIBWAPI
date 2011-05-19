package eisbot.proxy.types;
/**
 * Represents a StarCraft explosion type.
 * 
 * For a description of fields see: http://code.google.com/p/bwapi/wiki/Order
 */
public class OrderType {

	public static final int numAttributes = 1;

	private String name;	
	private int ID;
	
	public enum OrderTypeTypes {
	    Die,
	    Stop,
	    Guard,
	    PlayerGuard,
	    TurretGuard,
	    BunkerGuard,
	    Move,
	    AttackUnit,
	    AttackTile,
	    Hover,
	    AttackMove,
	    InfestedCommandCenter,
	    UnusedNothing,
	    UnusedPowerup,
	    TowerGuard,
	    VultureMine,
	    Nothing,
	    Nothing3,
	    CastInfestation,
	    InfestingCommandCenter,
	    PlaceBuilding,
	    BuildProtoss2,
	    ConstructingBuilding,
	    Repair,
	    PlaceAddon,
	    BuildAddon,
	    Train,
	    RallyPointUnit,
	    RallyPointTile,
	    ZergBirth,
	    ZergUnitMorph,
	    ZergBuildingMorph,
	    IncompleteBuilding,
	    BuildNydusExit,
	    EnterNydusCanal,
	    Follow,
	    Carrier,
	    ReaverCarrierMove,
	    CarrierIgnore2,
	    Reaver,
	    TrainFighter,
	    InterceptorAttack,
	    ScarabAttack,
	    RechargeShieldsUnit,
	    RechargeShieldsBattery,
	    ShieldBattery,
	    InterceptorReturn,
	    BuildingLand,
	    BuildingLiftOff,
	    DroneLiftOff,
	    LiftingOff,
	    ResearchTech,
	    Upgrade,
	    Larva,
	    SpawningLarva,
	    Harvest1,
	    Harvest2,
	    MoveToGas, //- Unit is moving to refinery
	    WaitForGas, // - Unit is waiting to enter the refinery (another unit is currently in it)
	    HarvestGas, //- Unit is in refinery
	    ReturnGas, //- Unit is returning gas to center
	    MoveToMinerals,// - Unit is moving to mineral patch
	    WaitForMinerals,// - Unit is waiting to use the mineral patch (another unit is currently mining from it)
	    MiningMinerals,// - Unit is mining minerals from mineral patch
	    Harvest3,
	    Harvest4,
	    ReturnMinerals, // - Unit is returning minerals to center
	    Interrupted,
	    EnterTransport,
	    PickupIdle,
	    PickupTransport,
	    PickupBunker,
	    Pickup4,
	    PowerupIdle,
	    Sieging,
	    Unsieging,
	    InitCreepGrowth,
	    SpreadCreep,
	    StoppingCreepGrowth,
	    GuardianAspect,
	    ArchonWarp,
	    CompletingArchonsummon,
	    HoldPosition,
	    Cloak,
	    Decloak,
	    Unload,
	    MoveUnload,
	    FireYamatoGun,
	    CastLockdown,
	    Burrowing,
	    Burrowed,
	    Unburrowing,
	    CastDarkSwarm,
	    CastParasite,
	    CastSpawnBroodlings,
	    CastEMPShockwave,
	    NukeWait,
	    NukeTrain,
	    NukeLaunch,
	    NukeUnit,
	    CastNuclearStrike,
	    NukeTrack,
	    CloakNearbyUnits,
	    PlaceMine,
	    RightClickAction,
	    CastRecall,
	    TeleporttoLocation,
	    CastScannerSweep,
	    Scanner,
	    CastDefensiveMatrix,
	    CastPsionicStorm,
	    CastIrradiate,
	    CastPlague,
	    CastConsume,
	    CastEnsnare,
	    CastStasisField,
	    CastHallucination,
	    Hallucination2,
	    ResetCollision,
	    Patrol,
	    CTFCOPInit,
	    CTFCOP1,
	    CTFCOP2,
	    ComputerAI,
	    AtkMoveEP,
	    HarassMove,
	    AIPatrol,
	    GuardPost,
	    RescuePassive,
	    Neutral,
	    ComputerReturn,
	    SelfDestrucing,
	    Critter,
	    HiddenGun,
	    OpenDoor,
	    CloseDoor,
	    HideTrap,
	    RevealTrap,
	    Enabledoodad,
	    Disabledoodad,
	    Warpin,
	    Medic,
	    MedicHeal1,
	    HealMove,
	    MedicHeal2,
	    CastRestoration,
	    CastDisruptionWeb,
	    CastMindControl,
	    DarkArchonMeld,
	    CastFeedback,
	    CastOpticalFlare,
	    CastMaelstrom,
	    JunkYardDog,
	    Fatal,
	    None,
	    Unknown,
	};

	
	public OrderType(int[] data, int index) {
		ID = data[index++];	
	}
	
	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public int getID() {
		return ID;
	}
}
