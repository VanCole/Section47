// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S47Spawner.h"
#include "S47SpawnerBoost.generated.h"

/**
 * 
 */
UCLASS()
class SECTION47_API AS47SpawnerBoost : public AS47Spawner
{
	GENERATED_BODY()
	
public:

	AS47SpawnerBoost();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	TSubclassOf<class AS47Boost> TakeBoost();

	UFUNCTION(BlueprintImplementableEvent)
		void TakeBoostBP();
};
