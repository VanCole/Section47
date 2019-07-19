// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S47Spawner.h"
#include "S47SpawnerWeapon.generated.h"

/**
 *
 */
UCLASS()
class SECTION47_API AS47SpawnerWeapon : public AS47Spawner
{
	GENERATED_BODY()

public:

	AS47SpawnerWeapon();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	/*UFUNCTION(BlueprintImplementableEvent)
		void TakeWeaponBP();*/

	TSubclassOf<class AS47Weapon> TakeWeapon();

};