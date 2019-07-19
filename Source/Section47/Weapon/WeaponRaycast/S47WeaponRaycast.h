// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../S47Weapon.h"
#include "AI/S47AICharacter.h"
#include "S47WeaponRaycast.generated.h"

/**
 * 
 */
UCLASS()
class SECTION47_API AS47WeaponRaycast : public AS47Weapon
{
	GENERATED_BODY()
	

public:
	AS47WeaponRaycast();

	//Methods
public:

	virtual void Tick(float DeltaTime);
	virtual void BeginPlay();

	virtual void Attack();

	virtual void StopAttack();
	/*UFUNCTION(Server,Reliable,WithValidation)
	void ServerAttackWeaponRayCastRPC(FVector2D _ViewportSize, FVector2D _ViewportCenter, FVector _StartLocation, FVector _RayDirection);*/

	void AttackRayCast();

	virtual void AttackBis();
	virtual void StopAttackBis();

	virtual void Reload();

	//virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void ShootRaycast(FVector StartLocation, FVector RayDirection);

	//UFUNCTION(Server, Reliable, WithValidation)
	//void GetClientViewPortRPC(FVector2D _center, FVector2D _size);
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerGetDamage(AS47AICharacter* _ResultHit, float _Damage);

	//Attributes
protected:

	UPROPERTY(EditAnywhere, Category = S47Attack)
		TSubclassOf<class AS47Impact> ImpactClass;

	
public :

	UPROPERTY(Category = S47WeaponRaycast, EditAnywhere)
		float precision;

	float rifleDecalage;

};
