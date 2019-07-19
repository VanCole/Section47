// Fill out your copyright notice in the Description page of Project Settings.

#include "S47TriggerEndLevel.h"
#include "Engine/World.h"
#include "S47GameMode.h"
#include "TimerManager.h"
#include "Player/S47PlayerCharacter.h"
#include "S47Types.h"

// Sets default values for this component's properties
AS47TriggerEndLevel::AS47TriggerEndLevel()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryActorTick.bCanEverTick = true;

	NumPlayerInTrigger = 0;
	NumPlayer = -1;
	TimeBeforeEnd = 1.0f;
	GameMode = nullptr;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	if (Box)
	{
		Box->SetIsReplicated(true);
	}
}


// Called when the game starts
void AS47TriggerEndLevel::BeginPlay()
{
	Super::BeginPlay();

	if (Box != nullptr && GetWorld() != nullptr)
	{
		// Only valid on Server
		GameMode = GetWorld()->GetAuthGameMode<AS47GameMode>();
		if (GameMode != nullptr)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, *FString::Printf(TEXT("%s: GameMode Found !"), GET_NETMODE_TEXT()));
			
			Box->OnComponentBeginOverlap.AddUniqueDynamic(this, &AS47TriggerEndLevel::OnTriggerEnter);
			Box->OnComponentEndOverlap.AddUniqueDynamic(this, &AS47TriggerEndLevel::OnTriggerExit);
		}
		else
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, *FString::Printf(TEXT("%s: No GameMode..."), GET_NETMODE_TEXT()));
		}
	}
}


void AS47TriggerEndLevel::OnTriggerEnter(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	AS47PlayerCharacter* Player = Cast<AS47PlayerCharacter>(OtherActor);
	if (nullptr != Player && nullptr != GetWorld())
	{
		PlayersInTrigger.AddUnique(Player);

		if (NumPlayerInTrigger != PlayersInTrigger.Num())
		{
			NumPlayerInTrigger = PlayersInTrigger.Num();
			NumPlayer = GameMode->NumPlayerAlive();
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, *FString::Printf(TEXT("%s: Nb player: %d/%d"), GET_NETMODE_TEXT(), NumPlayerInTrigger, NumPlayer));
			if (NumPlayerInTrigger >= NumPlayer && !GetWorld()->GetTimerManager().IsTimerActive(Timer))
			{
				GetWorld()->GetTimerManager().SetTimer(Timer, this, &AS47TriggerEndLevel::EndLevel, TimeBeforeEnd);
			}
		}
	}
}

void AS47TriggerEndLevel::OnTriggerExit(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	AS47PlayerCharacter* Player = Cast<AS47PlayerCharacter>(OtherActor);
	if (nullptr != Player && nullptr != GetWorld())
	{
		PlayersInTrigger.Remove(Player);

		if (NumPlayerInTrigger != PlayersInTrigger.Num())
		{
			NumPlayerInTrigger = PlayersInTrigger.Num();
			GetWorld()->GetTimerManager().ClearTimer(Timer);
			NumPlayer = GameMode->NumPlayerAlive();
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, *FString::Printf(TEXT("%s: Nb player: %d/%d"), GET_NETMODE_TEXT(), NumPlayerInTrigger, NumPlayer));
		}
	}
}

void AS47TriggerEndLevel::EndLevel()
{
	AS47GameMode* GameMode = GetWorld()->GetAuthGameMode<AS47GameMode>();
	if (nullptr != GameMode)
	{
		GameMode->EndLevel();
		SetLocked(true);
	}
}

