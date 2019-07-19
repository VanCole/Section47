// Fill out your copyright notice in the Description page of Project Settings.

#include "S47Grenade.h"
#include "AI/S47AICharacter.h"
#include "Components/S47ProjectileComponent.h"


void AS47Grenade::OnHitCollision(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	nbBounces++;
	OnBounceBP();
	if (nbBounces >= 3 || OtherActor->IsA<AS47AICharacter>())
		Explode(2.0f);
}
