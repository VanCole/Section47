// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S47WeaponRaycast.h"
#include "S47Rifle.generated.h"

/**
 * 
 */
UCLASS()
class SECTION47_API AS47Rifle : public AS47WeaponRaycast
{
	GENERATED_BODY()
	
		AS47Rifle();

public:

	virtual void Tick(float DeltaTime);
	virtual void BeginPlay();

	virtual void Attack();

	virtual void AttackBis();
	virtual void StopAttackBis();

	virtual void Reload();

	UFUNCTION(Server, Reliable, WithValidation)
	void ConsumeAmmoBis();


	UPROPERTY(Replicated, Category = S47Rifle, EditAnywhere)
		int currentAmmoBis;

private:
		float timeSinceLastAttackBis;
		bool isAttackingBis;
	
};
