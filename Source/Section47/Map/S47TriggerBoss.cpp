// Fill out your copyright notice in the Description page of Project Settings.

#include "S47TriggerBoss.h"
#include "Engine/World.h"
#include "Components/BoxComponent.h"
#include "S47GameState.h"
#include "S47GameMode.h"
#include "TimerManager.h"
#include "Player/S47PlayerCharacter.h"
#include "S47Types.h"
#include "S47RoomLevel.h"

// Sets default values for this component's properties
AS47TriggerBoss::AS47TriggerBoss()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));

	startedBoss = false;
	if (RootComponent != nullptr)
	{
		BoxComponent->SetupAttachment(RootComponent);
	}
}


// Called when the game starts
void AS47TriggerBoss::BeginPlay()
{
	Super::BeginPlay();

	if (nullptr != GetWorld())
	{
		// Only valid on Server
		AS47GameMode* GameMode = GetWorld()->GetAuthGameMode<AS47GameMode>();
		if (nullptr != GameMode && nullptr != BoxComponent)
		{
			BoxComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &AS47TriggerBoss::OnTriggerEnter);
			BoxComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &AS47TriggerBoss::OnTriggerExit);
		}

		AS47GameState* GameState = GetWorld()->GetGameState<AS47GameState>();
		if (nullptr != GameState)
		{
			GameState->OnBossStart.AddUniqueDynamic(this, &AS47TriggerBoss::LockBossRoom);
		}
	}
}


// Called every frame
void AS47TriggerBoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

void AS47TriggerBoss::OnTriggerEnter(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!startedBoss)
	{
		AS47PlayerCharacter* Player = Cast<AS47PlayerCharacter>(OtherActor);
		if (nullptr != Player && nullptr != GetWorld() && OtherComp == Cast<UPrimitiveComponent>(Player->GetCapsuleComponent()))
		{
			AS47GameState* GameState = GetWorld()->GetGameState<AS47GameState>();
			if (!PlayersInTrigger.Contains(Player) && nullptr != GameState)
			{
				PlayersInTrigger.AddUnique(Player);

				GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, *FString::Printf(TEXT("Player enter the BossRoom (%d/%d)"), PlayersInTrigger.Num(), GameState->NumPlayerAlive));

				if (PlayersInTrigger.Num() >= GameState->NumPlayerAlive)
				{
					GameState->InvokeBossStart();
				}
			}
		}
	}
}

void AS47TriggerBoss::LockBossRoom()
{
	SetLocked(true);
	startedBoss = true;

}

void AS47TriggerBoss::OnTriggerExit(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	AS47PlayerCharacter* Player = Cast<AS47PlayerCharacter>(OtherActor);
	if (nullptr != Player && nullptr != GetWorld() && OtherComp == Cast<UPrimitiveComponent>(Player->GetCapsuleComponent()))
	{
		AS47GameState* GameState = GetWorld()->GetGameState<AS47GameState>();
		if (PlayersInTrigger.Contains(Player) && nullptr != GameState)
		{
			PlayersInTrigger.Remove(Player);

			GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, *FString::Printf(TEXT("Player exit the BossRoom (%d/%d)"), PlayersInTrigger.Num(), GameState->NumPlayerAlive));
		}
	}
}

