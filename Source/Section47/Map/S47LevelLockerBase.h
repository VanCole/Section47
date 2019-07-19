// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "S47Types.h"
#include "S47LevelLockerBase.generated.h"

UCLASS()
class SECTION47_API AS47LevelLockerBase : public AActor
{
	GENERATED_BODY()
	
public:	
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnPostGeneration() {}

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="On Post Generation"))
	void OnPostGeneration_BP();

	UFUNCTION()
	virtual void OnBossEnd() {}

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Boss End"))
	void OnBossEnd_BP();

	void SetLocked(bool _locked, bool self = true, ERoomType roomType = ERoomType::NbRoomType);

private:
	class AS47RoomLevel* GetRoomLevel();

};
