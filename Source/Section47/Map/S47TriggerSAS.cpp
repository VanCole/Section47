// Fill out your copyright notice in the Description page of Project Settings.

#include "S47TriggerSAS.h"
#include "Engine/World.h"
#include "Components/BoxComponent.h"
#include "S47GameState.h"
#include "S47GameMode.h"
#include "TimerManager.h"
#include "Player/S47PlayerCharacter.h"
#include "S47Types.h"
#include "S47RoomLevel.h"

// Sets default values for this component's properties
AS47TriggerSAS::AS47TriggerSAS()
{
	PrimaryActorTick.bCanEverTick = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));

	if (RootComponent != nullptr)
	{
		BoxComponent->SetupAttachment(RootComponent);
	}
}


// Called when the game starts
void AS47TriggerSAS::BeginPlay()
{
	Super::BeginPlay();

	if (nullptr != GetWorld())
	{
		// Only valid on Server
		AS47GameMode* GameMode = GetWorld()->GetAuthGameMode<AS47GameMode>();
		if (nullptr != GameMode && nullptr != BoxComponent)
		{
			BoxComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &AS47TriggerSAS::OnTriggerEnter);
			BoxComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &AS47TriggerSAS::OnTriggerExit);
		}

		AS47GameState* GameState = GetWorld()->GetGameState<AS47GameState>();
		if (nullptr != GameState)
		{
			GameState->OnBossUnlock.AddUniqueDynamic(this, &AS47TriggerSAS::UnlockBossRoom);
		}
	}
}


// Called every frame
void AS47TriggerSAS::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

void AS47TriggerSAS::OnTriggerEnter(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	AS47PlayerCharacter* Player = Cast<AS47PlayerCharacter>(OtherActor);
	if (nullptr != Player && nullptr != GetWorld() && OtherComp == Cast<UPrimitiveComponent>(Player->GetCapsuleComponent()))
	{
		AS47GameState* GameState = GetWorld()->GetGameState<AS47GameState>();
		if (!PlayersInTrigger.Contains(Player) && nullptr != GameState)
		{
			PlayersInTrigger.AddUnique(Player);

			//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, *FString::Printf(TEXT("Player enter the SAS (%d/%d)"), PlayersInTrigger.Num(), GameState->NumPlayerAlive));

			if (PlayersInTrigger.Num() >= GameState->NumPlayerAlive)
			{
				GameState->InvokeBossUnlock();
			}
		}
	}
}

void AS47TriggerSAS::UnlockBossRoom()
{
	SetLocked(true, false, ERoomType::Normal);
	SetLocked(false, false, ERoomType::Boss);
}

void AS47TriggerSAS::OnTriggerExit(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	AS47PlayerCharacter* Player = Cast<AS47PlayerCharacter>(OtherActor);
	if (nullptr != Player && nullptr != GetWorld() && OtherComp == Cast<UPrimitiveComponent>(Player->GetCapsuleComponent()))
	{
		AS47GameState* GameState = GetWorld()->GetGameState<AS47GameState>();
		if (PlayersInTrigger.Contains(Player) && nullptr != GameState)
		{
			PlayersInTrigger.Remove(Player);

			//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Cyan, *FString::Printf(TEXT("Player exit the SAS (%d/%d)"), PlayersInTrigger.Num(), GameState->NumPlayerAlive));
		}
	}
}

