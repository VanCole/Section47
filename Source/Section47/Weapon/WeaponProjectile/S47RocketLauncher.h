// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S47WeaponProjectile.h"
#include "S47RocketLauncher.generated.h"

/**
 * 
 */
UCLASS()
class SECTION47_API AS47RocketLauncher : public AS47WeaponProjectile
{
	GENERATED_BODY()

	AS47RocketLauncher();

public:

	virtual void AttackBis();

};
