// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "Spawner/S47SpawnerWeapon.h"
#include "S47RoomLevel.generated.h"

class US47Room;

/**
 * 
 */
UCLASS()
class SECTION47_API AS47RoomLevel : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Data", meta = ( BlueprintBaseOnly /* Don't work... */ ) )
	TSubclassOf<class US47RoomData> Data;

// ========================= ROOM INSTANTIATED =========================

public:
	static uint32 Count;

	UPROPERTY(EditAnywhere, Category = "Weapon Spawners", meta = (DisplayName = "Minimum"))
	int MinSpawner = 0;
	UPROPERTY(EditAnywhere, Category = "Weapon Spawners", meta = (DisplayName = "Maximum"))
	int MaxSpawner = 0;

	UPROPERTY()
	TArray<AS47SpawnerWeapon*> WeaponSpawners;

	FVector Center;
	FVector HalfExtents;

	US47Room* Room = nullptr;
	bool PlayerInside = false;
	bool IsHidden = false;
	bool IsInit = false;
	bool PendingInit = false;
	bool IsLocked = false;

	UPROPERTY(VisibleAnywhere)
	uint32 Id;

	FRandomStream Random;

	UPROPERTY(EditAnywhere)
	bool AlwaysVisible = false;

public:
	AS47RoomLevel(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;
	virtual bool ShouldTickIfViewportsOnly() const override { return true; }

	void Init(US47Room* _room);

private:
	bool IsPlayerInside();
	void Display();

	UPROPERTY()
	TArray<AActor*> ActorsInLevel;
	FTransform Transform;
};
