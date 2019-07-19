// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S47Projectile.h"
#include "S47Grenade.generated.h"

/**
 * 
 */
UCLASS()
class SECTION47_API AS47Grenade : public AS47Projectile
{
	GENERATED_BODY()


	int32 nbBounces = 0;

public:

	UFUNCTION(BlueprintImplementableEvent)
	void OnBounceBP();

	virtual void OnHitCollision(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
