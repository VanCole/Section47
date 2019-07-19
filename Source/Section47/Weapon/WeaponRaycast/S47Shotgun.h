// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S47WeaponRaycast.h"
#include "S47Shotgun.generated.h"

/**
 * 
 */
UCLASS()
class SECTION47_API AS47Shotgun : public AS47WeaponRaycast
{
	GENERATED_BODY()

	AS47Shotgun();
	
public:



	virtual void Attack();

	virtual void AttackBis();
	virtual void StopAttackBis();

};
