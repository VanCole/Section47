// Fill out your copyright notice in the Description page of Project Settings.

#include "S47RoomData.h"
#include "CoreUObject.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "S47RoomLevel.h"
#include "S47Types.h"
#include "Section47.h"

US47RoomData::US47RoomData()
	: Super()
{
	doors.Add(FS47DoorDef());
	weight = 1;
	size = FIntVector(1, 1, 1);
}