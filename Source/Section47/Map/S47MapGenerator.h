// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "S47Room.h"
#include "Math/RandomStream.h"
#include "S47RoomLevel.h"
#include "S47MapGenerator.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMapEvent);

UENUM()
enum class ES47LevelState : uint8
{
	None,
	Generation,
	Load,
	Initialization,
	Unload,
	NbState
};

UCLASS()
class SECTION47_API AS47MapGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AS47MapGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	bool occlusionCulling;
	FRandomStream m_random;

	UPROPERTY(EditAnywhere, Category = "Procedural Generation")
	int levelWeight;
	UPROPERTY(EditAnywhere, Category = "Procedural Generation")
	bool randomSeed;
	UPROPERTY(EditAnywhere, Category = "Procedural Generation")
	bool AutoIncrementSeed;
	UPROPERTY(EditAnywhere, Category = "Procedural Generation")
	uint32 seed;


	// Rooms
	UPROPERTY(EditAnywhere, Category = "Map Rooms")
	TSubclassOf<US47RoomData> RoomSpawn;

	UPROPERTY(EditAnywhere, Category = "Map Rooms")
	TSubclassOf<US47RoomData> RoomExit;

	UPROPERTY(EditAnywhere, Category = "Map Rooms")
	TArray<TSubclassOf<US47RoomData>> RoomHeal;

	UPROPERTY(EditAnywhere, Category = "Map Rooms")
	TSubclassOf<US47RoomData> RoomSAS;

	UPROPERTY(EditAnywhere, Category = "Map Rooms")
	TArray<TSubclassOf<US47RoomData>> RoomBoss;

	UPROPERTY(EditAnywhere, Category = "Map Rooms")
	TArray<TSubclassOf<US47RoomData>> RoomDef;

	// Doors
	UPROPERTY(EditAnywhere, Category = "Map Doors")
	TSubclassOf<class AS47Door> normalDoorClass;

	UPROPERTY(EditAnywhere, Category = "Map Doors")
	TSubclassOf<class AS47Door> wallDoorClass;

	UPROPERTY(EditAnywhere, Category = "Map Doors")
	TSubclassOf<class AS47Door> bossDoorClass;

	UPROPERTY()
	TArray<US47Room*> roomList;
	UPROPERTY()
	TArray<class AS47Door*> doorList;

	int weightLeft;
	bool hasABoss;
	bool prevHasABoss;
	bool hasAnExit;
	bool hasAHeal;
	bool prevHasASAS;
	bool hasASAS;
	int additionalRoomLeft = 5;

	int minWeight = 0;
	int maxWeight = 5;

	static const int maxTry = 500;

	bool isInit = false;
	int nbInitRoom = 0;
	int nbLoadedRoom = 0;
	int nbUnloadedRoom = 0;

	ES47LevelState PreviousState = ES47LevelState::None;
	ES47LevelState State = ES47LevelState::None;


public:
	// Change the seed in S47GameState in server side
	// Do nothing on client
	UFUNCTION(BlueprintCallable)
	void Generate();

	UFUNCTION(BlueprintImplementableEvent)
		void OnPreGeneration_BP();
	UFUNCTION(BlueprintImplementableEvent)
		void OnPostGeneration_BP();

	void SetState(ES47LevelState NewState);

private:



	// Called when the seed is changed in S47GameState
	UFUNCTION()
	void BeginGeneration();

	UFUNCTION()
	void CreateMap();

	UFUNCTION()
	void LoadAllRooms();

	UFUNCTION()
	void UnloadAllRooms();

	// Instantiate a room in the scene
	void InstantiateRoom(US47Room* _room);

	// Recursive function to generate all rooms
	void AddRoom(US47Room& _room);

	UFUNCTION()
	void OnStateBegin(ES47LevelState _State);

	UFUNCTION()
	void OnStateTick(ES47LevelState _State);

	UFUNCTION()
	void OnStateEnd(ES47LevelState _State);
};
