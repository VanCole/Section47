// Fill out your copyright notice in the Description page of Project Settings.

#include "S47MapGenerator.h"
#include "S47GameInstance.h"
#include "Spawner/S47Spawner.h"
#include "Spawner/S47SpawnerEnemy.h"
#include "S47RoomData.h"
#include "S47Door.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"
#include "S47GameMode.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Section47.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NavigationSystem.h"


// Sets default values
AS47MapGenerator::AS47MapGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	occlusionCulling = true;
	levelWeight = 10;
	randomSeed = true;
	seed = 123456789; // default seed
 }

// Called when the game starts or when spawned
void AS47MapGenerator::BeginPlay()
{
	Super::BeginPlay();

	// Create a map when the S47GameState seed change
	GetWorld()->GetGameState<AS47GameState>()->OnSetSeed.AddUniqueDynamic(this, &AS47MapGenerator::BeginGeneration);
}

void AS47MapGenerator::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UnloadAllRooms();
}

// Called every frame
void AS47MapGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	OnStateTick(State);

//#if WITH_EDITOR
	// Debug key to manually generate a level
	if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::G))
	{
		AS47GameMode* GameMode = GetWorld()->GetAuthGameMode<AS47GameMode>();
		if (GameMode != nullptr)
		{
			GameMode->GenerateMap();
		}
	}
//#endif
}

void AS47MapGenerator::Generate()
{
	AS47GameMode* GameMode = GetWorld()->GetAuthGameMode<AS47GameMode>();
	if (nullptr != GameMode) // prevent client to change the map seed
	{
		if (randomSeed)
		{
			seed = GameMode->RandomSeed();
		}
		else
		{
			GameMode->SetSeed(seed);
			if (AutoIncrementSeed)
			{
				seed += 123456;
			}
		}
	}
}

void AS47MapGenerator::SetState(ES47LevelState NewState)
{
	OnStateEnd(State);
	PreviousState = State;
	State = NewState;
	OnStateBegin(State);
}


void AS47MapGenerator::BeginGeneration()
{
	SetState(ES47LevelState::Unload);
}

void AS47MapGenerator::CreateMap()
{
	// Initialize the random generator with the S47GameState seed
	m_random.Initialize(GetWorld()->GetGameState<AS47GameState>()->Seed);

	AS47GameState* GameState = GetWorld()->GetGameState<AS47GameState>();
	if (nullptr != GameState)
	{
		GameState->TimeReference = 0.0f;
	}

	isInit = false;
	nbInitRoom = 0;

	US47LevelStreamingDynamic::UniqueLevelInstanceId = 0;
	AS47Spawner::count = 0;
	AS47RoomLevel::Count = 0;
	AS47SpawnerEnemy::count = 0;
	int tries = maxTry;

	// generate level until there are a boss room and an exit room
	do {
		//UE_LOG(LogS47Level, Log, TEXT("Generation Try: %d (Current seed: %d)"), 1 + maxTry - tries, m_random.GetCurrentSeed());
		hasABoss = false;
		hasAHeal = false;
		hasAnExit = false;
		hasASAS = false;
		prevHasABoss = false;
		prevHasASAS = false;
		minWeight = 0;
		maxWeight = 1;

		roomList.Empty();
		// Build the list of rooms with recursive function
		US47Room* root = NewObject<US47Room>();		
		//root->Rename(TEXT("Coucou"));
		root->Init(RoomSpawn);
		roomList.Add(root);

		weightLeft = levelWeight;
		additionalRoomLeft = 5;
		AddRoom(*root);


		//UE_LOG(LogS47Level, Log, TEXT("Result: has %sa SAS | has %sa Boss | has %san Exit"), 
			//hasASAS ? TEXT("") : TEXT("not "), hasABoss ? TEXT("") : TEXT("not "), hasAnExit ? TEXT("") : TEXT("not "));

		tries--;
	} while (tries > 0 && (!hasABoss || !hasAnExit));
}


void AS47MapGenerator::InstantiateRoom(US47Room* _room)
{
	// Add room time to total level reference time
	AS47GameState* GameState = GetWorld()->GetGameState<AS47GameState>();
	if (nullptr != GameState)
	{
		GameState->TimeReference += _room->prefab->time;
	}

	// Instantiate room
	_room->Instantiate(GetWorld());

	for (int i = 0; i < _room->GetConnectionCount(); i++)
	{
		// Get next room
		US47Room* r = _room->GetConnection(i).Get();

		FIntVector doorCell = _room->GetDoorWorldPosition(i);
		ES47Orientation doorRot = _room->GetDoorWorldOrientation(i);

		// Don't instantiate room nor door if it's the parent
		if (r != _room->parent)
		{
			TSubclassOf<AS47Door> doorClass = wallDoorClass;
			if (nullptr != r)
			{
				switch (r->prefab->type)
				{
				case ERoomType::Boss:
					doorClass = bossDoorClass;
					break;
				default:
					doorClass = normalDoorClass;
					break;
				}
			}

			FVector instanceDoorPos = US47Room::unit * (FVector(doorCell) + 0.5f * FVector(US47Room::GetDirection(doorRot)));
			FRotator instanceDoorRot = FRotator(0, -90 * (int8)doorRot, 0);
			AS47Door* door = GetWorld()->SpawnActor<AS47Door>(doorClass, instanceDoorPos, instanceDoorRot);

			if (nullptr != door)
			{
				door->SetConnectingRooms(_room, r);
				doorList.Add(door);
			}
		}
	}
}

void AS47MapGenerator::AddRoom(US47Room& _room)
{
	int nbDoor = _room.prefab->GetNbDoor();
	int tries = maxTry;

	for (int i = 0; i < nbDoor; i++)
	{
		if (!_room.IsConnected(i))
		{
			TSubclassOf<US47RoomData> def;
			US47RoomData* defaultObject = nullptr;

			if (RoomSAS != nullptr && !hasASAS && weightLeft <= 0)
			{
				def = RoomSAS;
			}
			else if (((RoomSAS == nullptr && weightLeft <= 0) || (RoomSAS != nullptr && hasASAS != prevHasASAS)) && !hasABoss)
			{
				def = RoomBoss[m_random.RandRange(0, RoomBoss.Num() - 1)];
			}
			else if (!hasAnExit && hasABoss != prevHasABoss)
			{
				def = RoomExit;
			}
			else if (hasAnExit && !hasAHeal && m_random.RandRange(0, 10 - 1) == 0)
			{
				def = RoomHeal[m_random.RandRange(0, RoomHeal.Num() - 1)];
			}
			else
			{
				// Get random room def
				int n = -1;
				do {
					n = m_random.RandRange(0, RoomDef.Num() - 1);
					defaultObject = RoomDef[n].GetDefaultObject();
					tries--;
				} while (tries > 0 
					&& ((!hasABoss && defaultObject->GetNbDoor() < 2)
						|| (defaultObject->weight < minWeight
							|| defaultObject->weight > maxWeight)));

				def = RoomDef[n];
				if (defaultObject->weight <= 0)
				{
					minWeight = 1;
				}
				else
				{
					minWeight = 0;
					if (maxWeight <= 3)
					{
						maxWeight += defaultObject->weight;
					}
					else
					{
						maxWeight -= defaultObject->weight;
					}
				}
				if (minWeight < 0) minWeight = 0;
				if (maxWeight < 1) maxWeight = 1;
			}

			defaultObject = def.GetDefaultObject();

			prevHasABoss = hasABoss; // ensure that exit is after boss room;
			prevHasASAS = hasASAS;

			// Create room from roomdef and set connections with current room
			US47Room* newRoom = NewObject<US47Room>();
		//	newRoom->Rename(*FString::Printf(TEXT("Coucou %d"),roomList.Num()));
			newRoom->Init(def, &_room);
			int doorIndex = defaultObject->type == ERoomType::Boss ? 0 : m_random.RandRange(0, newRoom->prefab->GetNbDoor() - 1);
			

			// Set position, rotation and connections between new room and parent room
			newRoom->ConnectTo(doorIndex, _room, i);

			if (!US47Room::Overlap(*newRoom, roomList))
			{
				roomList.Add(newRoom);

				switch (defaultObject->type)
				{
				case ERoomType::Boss:
					hasABoss = true;
					break;
				case ERoomType::Exit:
					hasAnExit = true;
					break;
				case ERoomType::Heal:
					hasAHeal = true;
					break;
				case ERoomType::SAS:
					hasASAS = true;
					break;
				default:
					break;
				}

				if (((RoomSAS != nullptr && !hasASAS) || (RoomSAS == nullptr && !hasABoss)) && weightLeft > 0)
				{
					weightLeft -= defaultObject->weight;
				}
				else if (hasAnExit)
				{
					additionalRoomLeft--;
				}

				if ((hasAnExit && additionalRoomLeft > 0) || (RoomSAS != nullptr && !hasASAS) || (!hasABoss && (RoomSAS == nullptr || hasASAS)) || (hasABoss && !hasAnExit))
				{
					AddRoom(*newRoom);
				}
			}
			else
			{
				delete newRoom;
				_room.SetConnection(i, nullptr);
			}
		}
	}
}


void AS47MapGenerator::OnStateBegin(ES47LevelState _State)
{
	switch (State)
	{
	case ES47LevelState::Unload:
		UE_LOG(LogS47Level, Log, TEXT("======= Begin Unload All Levels ======="));
		UnloadAllRooms();
		nbUnloadedRoom = 0;
		break;
	case ES47LevelState::Generation:
		GetWorld()->GetGameState<AS47GameState>()->OnPreGeneration.Broadcast();
		OnPreGeneration_BP();
		UE_LOG(LogS47Level, Log, TEXT("======= Begin Map Generation ======="));
		CreateMap();
		break;
	case ES47LevelState::Load:
		UE_LOG(LogS47Level, Log, TEXT("======= Begin Load All Levels ======="));
		LoadAllRooms();
		nbLoadedRoom = 0;
		break;
	case ES47LevelState::Initialization:
		UE_LOG(LogS47Level, Log, TEXT("======= Begin Init All Levels ======="));
		UE_LOG(LogS47Level, Log, TEXT("Nb Room To Initialize: %d"), roomList.Num());
		break;
	default:
		break;
	}
}

void AS47MapGenerator::OnStateTick(ES47LevelState _State)
{
	int tmp = 0;
	switch (State)
	{
	case ES47LevelState::Unload:

		// Count nb level loaded
		for (int i = 0; i < roomList.Num(); i++)
		{
			if (roomList[i]->instance->IsLevelUnloaded())
			{
				tmp++;
			}
		}
		// Change state when all levels are loaded
		if (tmp == roomList.Num())
		{
			SetState(ES47LevelState::Generation);
		}
		break;
	case ES47LevelState::Generation:
		SetState(ES47LevelState::Load);
		break;
	case ES47LevelState::Load:

		// Count nb level loaded
		for (int i = 0; i < roomList.Num(); i++)
		{
			if (roomList[i]->instance->IsLevelLoaded())
			{
				tmp++;
			}
		}
		// Change state when all levels are loaded
		if (tmp == roomList.Num())
		{
			SetState(ES47LevelState::Initialization);
		}

		break;
	case ES47LevelState::Initialization:
		// While initialization in't done, try to initialize all rooms
		if (!isInit)
		{
			isInit = true;
			for (TWeakObjectPtr<US47Room> room : roomList)
			{
				AS47RoomLevel* script = room->GetLevelScript();

				if (nullptr != script)
				{
					isInit &= script->IsInit;
					if (!script->IsInit && !script->PendingInit)
					{
						nbInitRoom++;
						script->Room = nullptr;
						script->Init(room.Get());

						UE_LOG(LogS47Level, Log, TEXT("Room Initialization: %d/%d"), nbInitRoom, roomList.Num());
					}
				}
				else
				{
					isInit = false;
				}
			}

			if (isInit)
			{
				SetState(ES47LevelState::None);
			}
			return;
		}
		break;
	
	default:
		break;
	}
}

void AS47MapGenerator::OnStateEnd(ES47LevelState _State)
{
	FTimerHandle handle;
	UNavigationSystemV1* nav = nullptr;
	switch (State)
	{
	case ES47LevelState::Unload:
		/*for (int i = 0; i < roomList.Num(); i++)
		{
			delete roomList[i].Get();
		}*/
		roomList.Empty();
		GetWorld()->FlushLevelStreaming();
		GEngine->ForceGarbageCollection(true);
		UE_LOG(LogS47Level, Log, TEXT("======= End Unload All Levels ======="));
		break;
	case ES47LevelState::Generation:
		UE_LOG(LogS47Level, Log, TEXT("======= End Map Generation ======="));
		break;
	case ES47LevelState::Load:
		UE_LOG(LogS47Level, Log, TEXT("======= End Load All Levels ======="));
		break;
	case ES47LevelState::Initialization:
		UE_LOG(LogS47Level, Log, TEXT("======= End Init All Levels ======="));

		// Try to rebuild the navmesh
		//UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), "showlog");
		nav = UNavigationSystemV1::GetCurrent(GetWorld());
		if (nullptr != nav)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Build navmesh"));
			UE_LOG(LogS47Level, Log, TEXT("Build nav mesh"));
			nav->CancelBuild();
			nav->Build();
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Error: Navmesh not found"));
			UE_LOG(LogS47Level, Error, TEXT("Error: Navmesh not found"));
		}
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("You Die: %s"), *Cast< UNavigationSystemV1>(GetWorld()->GetNavigationSystem())->MainNavData->GetFName().ToString()));
		
		// Invoke Post Generation Event when initialization is done
		GetWorld()->GetGameState<AS47GameState>()->OnPostGeneration.Broadcast();
		OnPostGeneration_BP();
		break;
	default:
		break;
	}
}

void AS47MapGenerator::LoadAllRooms()
{
	// When a level is correct, load all rooms
	for (int i = 0; i < roomList.Num(); i++)
	{
		InstantiateRoom(roomList[i]);
	}
}

void AS47MapGenerator::UnloadAllRooms()
{
	for (int i = 0; i < doorList.Num(); i++)
	{
		doorList[i]->Destroy();
	}
	doorList.Empty();

	for (int i = 0; i < roomList.Num(); i++)
	{
		roomList[i]->Destroy(GetWorld());
	}
}
