// Fill out your copyright notice in the Description page of Project Settings.

#include "S47TriggerHealingRoom.h"
#include "TimerManager.h"
#include "Player/S47PlayerCharacter.h"
#include "GameFramework/PlayerController.h"
#include "S47DamageableInterface.h"
#include "S47Types.h"

// Sets default values for this component's properties
US47TriggerHealingRoom::US47TriggerHealingRoom()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	HealAmount = 10;
	TickDuration = 0.5f;
}


// Called when the game starts
void US47TriggerHealingRoom::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld()->GetAuthGameMode() != nullptr)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Healing Begin Play"));
		OnComponentBeginOverlap.AddUniqueDynamic(this, &US47TriggerHealingRoom::OnTriggerEnter);
		OnComponentEndOverlap.AddUniqueDynamic(this, &US47TriggerHealingRoom::OnTriggerExit);
		GetWorld()->GetTimerManager().SetTimer(Timer, this, &US47TriggerHealingRoom::HealCharacters, TickDuration, true);
	}

	/*APlayerController* controller = GetWorld()->GetFirstPlayerController();
	if (controller != nullptr)
	{
		LocalCharacter = Cast<AS47PlayerCharacter>(controller->GetCharacter());
		GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Black, *FString::Printf(TEXT("%s: Player controller: %s"), GET_NETMODE_TEXT(), *LocalCharacter->GetName()));

	}*/

}


// Called every frame
void US47TriggerHealingRoom::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void US47TriggerHealingRoom::OnTriggerEnter(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Actor enter healing room"));
	AS47PlayerCharacter* Character = Cast<AS47PlayerCharacter>(OtherActor);
	if (nullptr != GetWorld() && nullptr != Character)// && LocalCharacter == Character)
	{
		
		if (!Characters.Contains(Character))
		{
			Characters.Add(Character);
			//GetWorld()->GetTimerManager().SetTimer(Timers, this, &US47TriggerHealingRoom::HealCharacter, TickDuration, true);
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, *FString::Printf(TEXT("%s: Player Enter: %s"), GET_NETMODE_TEXT(), *GetNameSafe(Character)));
		}


	}
	
}

void US47TriggerHealingRoom::OnTriggerExit(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	AS47PlayerCharacter* Character = Cast<AS47PlayerCharacter>(OtherActor);
	if (nullptr != GetWorld() && nullptr != Character)// && Character == LocalCharacter)
	{
		if (Characters.Contains(Character))
		{
			Characters.Remove(Character);
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, *FString::Printf(TEXT("%s: Player Exit: %s"), GET_NETMODE_TEXT(), *GetNameSafe(Character)));
		}
		//GetWorld()->GetTimerManager().ClearTimer(Timer);
	}
}

void US47TriggerHealingRoom::HealCharacters()
{
	//if (nullptr != LocalCharacter)
	//{
	//	
	//	//IS47DamageableInterface* Damageable = Cast<IS47DamageableInterface>(LocalCharacter);
	//	LocalCharacter->SetHealth(LocalCharacter->GetHealth() + HealAmount);
	//}

	for (AS47PlayerCharacter* Character : Characters)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 0.4f, FColor::Yellow, *FString::Printf(TEXT("%s: Heal %dpv to %s"), GET_NETMODE_TEXT(), HealAmount, *GetNameSafe(Character)));
		Character->SetHealth(Character->GetHealth() + HealAmount);
	}
}

