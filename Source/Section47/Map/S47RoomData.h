// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "S47Types.h"
#include "S47RoomData.generated.h"

/**
 * 
 */
UCLASS()
class SECTION47_API US47RoomData : public UPrimaryDataAsset
{
	GENERATED_BODY()

	friend class US47LevelStreamingDynamic;

private:
	UPROPERTY(EditAnywhere, Category = "Level")
	TSoftObjectPtr<UWorld> level;

public:
	UPROPERTY(EditAnywhere, Category = "Doors")
	TArray<FS47DoorDef> doors;

	UPROPERTY(EditAnywhere, Category = "Room")
	ERoomType type;

	UPROPERTY(EditAnywhere, Category = "Room")
	FIntVector size;

	UPROPERTY(EditAnywhere, Category = "Room")
	int weight;

	UPROPERTY(EditAnywhere, Category = "Room")
	float time;

public:
	US47RoomData();

	int GetNbDoor() const { return doors.Num(); }
};
