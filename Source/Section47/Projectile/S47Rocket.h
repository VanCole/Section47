// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S47Projectile.h"
#include "S47Rocket.generated.h"

/**
 * 
 */
UCLASS()
class SECTION47_API AS47Rocket : public AS47Projectile
{
	GENERATED_BODY()
	

public:
	virtual void OnHitCollision(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
