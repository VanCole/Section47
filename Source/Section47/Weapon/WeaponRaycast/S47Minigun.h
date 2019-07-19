// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S47WeaponRaycast.h"
#include "S47Minigun.generated.h"

/**
 * 
 */
UCLASS()
class SECTION47_API AS47Minigun : public AS47WeaponRaycast
{
	GENERATED_BODY()

	AS47Minigun();
	
protected:
	UPROPERTY(Category = S47Animation, VisibleAnywhere, BlueprintReadOnly)
	float fireRateChange = 2.0f;
	UPROPERTY(Category = S47Animation, EditAnywhere, BlueprintReadWrite)
	float customFirerate = fireRateChange;
	float timer = 0;

	void BarrelAcceleration(float _deltaTime);

public:
	virtual void Tick(float DeltaTime) override;

	virtual void Attack();

	virtual void AttackBis();

	virtual void StopAttackBis();

};
