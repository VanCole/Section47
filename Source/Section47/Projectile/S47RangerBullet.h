// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile/S47Projectile.h"
#include "S47RangerBullet.generated.h"

/**
 * 
 */
UCLASS()
class SECTION47_API AS47RangerBullet : public AS47Projectile
{
	GENERATED_BODY()
	
public:
	virtual void OnHitCollision(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
