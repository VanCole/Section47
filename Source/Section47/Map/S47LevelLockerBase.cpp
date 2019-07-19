// Fill out your copyright notice in the Description page of Project Settings.

#include "S47LevelLockerBase.h"
#include "Map/S47RoomLevel.h"
#include "Map/S47Room.h"
#include "Map/S47RoomData.h"
#include "S47GameState.h"
#include "Engine/World.h"

void AS47LevelLockerBase::BeginPlay()
{
	Super::BeginPlay();
	AS47GameState* GameState = GetWorld()->GetGameState<AS47GameState>();
	if (nullptr != GameState)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Yellow, TEXT("LockerBase BeginPlay"));
		GameState->OnPostGeneration.AddUniqueDynamic(this, &AS47LevelLockerBase::OnPostGeneration);
		GameState->OnPostGeneration.AddUniqueDynamic(this, &AS47LevelLockerBase::OnPostGeneration_BP);

		GameState->OnBossEnd.AddUniqueDynamic(this, &AS47LevelLockerBase::OnBossEnd);
		GameState->OnBossEnd.AddUniqueDynamic(this, &AS47LevelLockerBase::OnBossEnd_BP);
	}
}

void AS47LevelLockerBase::SetLocked(bool _locked, bool self, ERoomType roomType)
{
	AS47RoomLevel* script = GetRoomLevel();
	if (nullptr != script)
	{
		if (self)
		{
			script->IsLocked = _locked;
		}

		US47Room* room = script->Room;
		if (nullptr != room)
		{
			for (int i = 0; i < room->GetConnectionCount(); i++)
			{
				if (nullptr != room->GetConnection(i) && nullptr != room->GetConnection(i)->GetLevelScript())
				{
					if (roomType == room->GetConnection(i)->prefab->type)
					{
						room->GetConnection(i)->GetLevelScript()->IsLocked = _locked;
					}
				}
			}
		}
	}
}

AS47RoomLevel * AS47LevelLockerBase::GetRoomLevel()
{
	return Cast<AS47RoomLevel>(GetLevel()->GetLevelScriptActor());
}

