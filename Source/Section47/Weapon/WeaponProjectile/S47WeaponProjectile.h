// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/S47Weapon.h"
#include "S47WeaponProjectile.generated.h"

/**
 * 
 */
UCLASS()
class SECTION47_API AS47WeaponProjectile : public AS47Weapon
{
	GENERATED_BODY()
	
public:
	AS47WeaponProjectile();

	virtual void Tick(float DeltaTime);
	virtual void BeginPlay();

	//Methods
public:

	virtual void Attack();

	virtual void AttackBis();
	virtual void StopAttackBis();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSpawnProjectile();
	virtual TArray<class AS47Projectile*> SpawnProjectile();

	//Attributes
protected:

	UPROPERTY(EditAnywhere, Category = S47Attack)
	TSubclassOf<class AS47Projectile> ProjectileClass;

	TArray<FTransform> StartingProjectilesPoints;

	UPROPERTY(EditAnywhere, Category = S47ProjectileWeapon)
	FName StartingProjectilePointTag;

	UPROPERTY(EditAnywhere, Category = S47ProjectileWeapon)
	int32 NbProjectiles;
};
