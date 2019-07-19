// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S47WeaponProjectile.h"
#include "GenericTeamAgentInterface.h"
#include "S47GrenadeLauncher.generated.h"

/**
 * 
 */
UCLASS()
class SECTION47_API AS47GrenadeLauncher : public AS47WeaponProjectile, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

	AS47GrenadeLauncher();

public:
	virtual void Tick(float DeltaTime);

	virtual void AttackBis();
	virtual void StopAttackBis();

	void Explode();

	//UPROPERTY(Replicated)
	bool isExploding = false;

	UFUNCTION(BlueprintImplementableEvent)
	void OnCharge1();
	UFUNCTION(BlueprintImplementableEvent)
	void OnCharge2();
	UFUNCTION(BlueprintImplementableEvent)
	void OnCharge3();

	UFUNCTION(BlueprintImplementableEvent)
	void OnDischarge();

	UFUNCTION(BlueprintImplementableEvent)
	void OnThrow();

	UFUNCTION(BlueprintImplementableEvent)
	void OnExplodingBP();

	virtual FGenericTeamId GetGenericTeamId() const override { return S47TeamID; }
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;

private:

	UPROPERTY(Category = OMFExplosion, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AOETrigger;

	bool isCharging = false;
	const float timeToCharge = 1.5f;
	float chargeTimer = timeToCharge;

	const float timeToExplode = 2.0f;
	float explodeTimer = timeToExplode;

protected:
	FGenericTeamId S47TeamID = 0;

	UFUNCTION(NetMulticast, Reliable)
	void OnExplodingBP_ToClient();

	UFUNCTION(NetMulticast, Unreliable)
	void AddImpulse_ToClient(UPrimitiveComponent* _Enemy, FVector _Impulse, FVector _Location);

};
