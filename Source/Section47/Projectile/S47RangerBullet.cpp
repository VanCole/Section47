// Fill out your copyright notice in the Description page of Project Settings.

#include "S47RangerBullet.h"

void AS47RangerBullet::OnHitCollision(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Explode(2.0f);
}