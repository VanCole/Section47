// Fill out your copyright notice in the Description page of Project Settings.

#include "S47TriggerDeath.h"
#include "S47GameMode.h"
#include "Player/S47PlayerCharacter.h"
#include "S47Types.h"

// Sets default values for this component's properties
US47TriggerDeath::US47TriggerDeath()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void US47TriggerDeath::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld() != nullptr)
	{
		// Only valid on Server
		AS47GameMode* GameMode = GetWorld()->GetAuthGameMode<AS47GameMode>();
		if (GameMode != nullptr)
		{
			OnComponentBeginOverlap.AddUniqueDynamic(this, &US47TriggerDeath::OnTriggerEnter);
		}
	}
}

void US47TriggerDeath::OnTriggerEnter(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	AS47PlayerCharacter* Player = Cast<AS47PlayerCharacter>(OtherActor);
	if (nullptr != Player)
	{
		Player->GetDamage(10000.0f, GetOwner());
	}
}
