// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S47WeaponRaycast.h"
#include "S47Lasergun.generated.h"

/**
 * 
 */
UCLASS()
class SECTION47_API AS47Lasergun : public AS47WeaponRaycast
{
	GENERATED_BODY()

	AS47Lasergun();
	
public:

	UFUNCTION(BlueprintImplementableEvent)
	void OnStartConsumingBP();

	UFUNCTION(BlueprintImplementableEvent)
	void OnConsumeBP();

	UFUNCTION(BlueprintImplementableEvent)
	void OnBeamUpdateBP();

	UFUNCTION(BlueprintImplementableEvent)
		void OnAttackBis();
	UFUNCTION(BlueprintImplementableEvent)
		void OnStopAttackBis();

	virtual void Tick(float DeltaTime) override;

	virtual void Attack();

	virtual void AttackBis();

	virtual void StopAttack();

	virtual void StopAttackBis();

	void ZoomTo(float _oldFOV, float _newFOV);

	UFUNCTION(BlueprintCallable)
	void ResetZoom();
	
	float currentFOV = 90;

	UPROPERTY(Category = S47Lasergun, VisibleAnywhere, BlueprintReadWrite)
	float wantedFOV = 90;

	UPROPERTY(Category = S47Weapon, VisibleAnywhere, BlueprintReadWrite)
	int AmmoConsumed = 0;
	UPROPERTY(Category = S47Weapon, VisibleAnywhere, BlueprintReadOnly)
	int MaxAmmoConsumed = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = S47Weapon)
	float timeToCharge = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = S47Weapon)
	float ShakeAmount = 0.7f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = S47Weapon)
	float ShakeDecrease = 1.0f;
	float ShakeDuration = 0.0f;


private:

	float timerCharging = timeToCharge;
	void LaserGunAttack();
	bool CanAttack;


	FVector originPos;
	void ShakeWeapon(float _deltaTime);

};
