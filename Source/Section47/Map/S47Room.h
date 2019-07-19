// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Map/S47LevelStreamingDynamic.h"
#include "GameFramework/Actor.h"
#include "S47Types.h"
#include "S47Room.generated.h"

class AS47RoomLevel;
class US47RoomData;
class AS47Door;

UCLASS()
class SECTION47_API US47Room : public UObject
{
	GENERATED_BODY()
private:
	UPROPERTY()
	TArray<TWeakObjectPtr<US47Room>> connections;

public:
	static bool occlusionCulling;
	static FVector unit;

	UPROPERTY()
	US47LevelStreamingDynamic* instance;
	US47Room* parent;
	FIntVector position;
	ES47Orientation orientation;

	UPROPERTY()
	US47RoomData* prefab = nullptr;

private:
	TSubclassOf<US47RoomData> prefabClass;

public:
	US47Room();
	void Init(TSubclassOf<US47RoomData> _prefab, US47Room* _parent = nullptr);
	~US47Room();

	bool IsConnected(int _index);
	void SetConnection(int _index, US47Room* _room);
	TWeakObjectPtr<US47Room> GetConnection(int _index);
	int GetConnectionIndex(US47Room& _room);

	void Instantiate(UWorld* world);
	void Destroy(UWorld* world);
	AS47RoomLevel* GetLevelScript();

	ES47Orientation GetDoorWorldOrientation(int _doorIndex);
	FIntVector GetDoorWorldPosition(int _doorIndex);
	int GetConnectionCount() { return connections.Num(); }

	FIntVector WorldToRoom(FIntVector worldPos);
	FIntVector RoomToWorld(FIntVector roomPos);
	void SetRotationFromDoor(int doorIndex, ES47Orientation worldRot);
	void SetPositionFromDoor(int doorIndex, FIntVector worldPos);
	void SetPositionAndRotationFromDoor(int doorIndex, FIntVector worldPos, ES47Orientation worldRot);
	bool IsOccupied(FIntVector cell);

	void ConnectTo(int _thisDoorIndex, US47Room& _otherRoom, int _otherDoorIndex);


	// AABB Overlapping
	static bool Overlap(US47Room& A, US47Room& B);
	static bool Overlap(US47Room& _room, TArray<US47Room*>& _roomList);
	static ES47Orientation Add(ES47Orientation A, ES47Orientation B);
	static ES47Orientation Sub(ES47Orientation A, ES47Orientation B);
	static FIntVector GetDirection(ES47Orientation O);
	static FIntVector Rotate(FIntVector pos, ES47Orientation rot);
};
