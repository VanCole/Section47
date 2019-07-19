// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../S47Enemy.h"

#include "S47EnemyRange.generated.h"

/**
 *
 */
UCLASS()
class SECTION47_API AS47EnemyRange : public AS47Enemy
{
	GENERATED_BODY()

public:
	AS47EnemyRange();

	virtual void BeginPlay();

	//Methods
public:

	void AttackRange();

	virtual void Attack() override;
	virtual TArray<class AS47Projectile*> SpawnProjectile();

	//Attributes
protected:

	UPROPERTY(EditAnywhere, Category = S47EnemyRange)
		TSubclassOf<class AS47Projectile> ProjectileClass;
};
