// Fill out your copyright notice in the Description page of Project Settings.

#include "S47Rocket.h"

void AS47Rocket::OnHitCollision(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Explode(2.0f);

}