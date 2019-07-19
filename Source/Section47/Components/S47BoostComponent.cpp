// Fill out your copyright notice in the Description page of Project Settings.

#include "S47BoostComponent.h"
#include "Player/S47PlayerCharacter.h"
#include "Spawner/S47SpawnerBoost.h"
#include "Boost/S47Boost.h"
#include "Boost/S47DoubleDamage.h"
#include "Boost/S47SpeedUp.h"
#include "Boost/S47FullArmor.h"
#include "Boost/S47Invincibility.h"
#include "TimerManager.h"
#include "S47GameState.h"
#include "Boost/S47BoostStruct.h"

// Sets default values for this component's properties
US47BoostComponent::US47BoostComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void US47BoostComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ROLE_Authority)
	{
		OwnerCharacter = Cast<AS47PlayerCharacter>(GetOwner());

		if (nullptr != OwnerCharacter)
		{
			OwnerCharacter->OnActorBeginOverlap.AddUniqueDynamic(this, &US47BoostComponent::OnSpawnerBeginOverlap);
		}
	}
}


// Called every frame
void US47BoostComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void US47BoostComponent::OnSpawnerBeginOverlap(AActor * _OverlappedActor, AActor * _OtherActor)
{
	if (nullptr != _OtherActor)
	{
		if (_OtherActor->IsA<AS47SpawnerBoost>())
		{
			AS47SpawnerBoost* MySpawner = Cast<AS47SpawnerBoost>(_OtherActor);

			if (nullptr != _OverlappedActor && _OverlappedActor->IsA<AS47Character>())
			{
				AS47Character* MyCharacter = Cast<AS47Character>(_OverlappedActor);

				if (MyCharacter->IsValidActorForCollision(_OverlappedActor))
				{
					if (nullptr != MySpawner)
					{
						AS47Boost* newBoost = Cast<AS47Boost>(MySpawner->CurrentActor);

						if (nullptr != newBoost)
						{
							AddBoost(newBoost->boostStruct);
							MySpawner->TakeBoost();
						}
					}
				}
			}
		}
	}
}

void US47BoostComponent::AddBoost(US47BoostStruct* newBoost)
{
	US47BoostStruct* refBoost = nullptr;

	for (int32 i = 0; i < listBoost.Num() && refBoost == nullptr; ++i)
	{
		if (*listBoost[i] == *newBoost)
		{
			refBoost = listBoost[i];
		}
	}

	// if newBoost is already in listboost
	if (refBoost != nullptr)
	{
		delete newBoost;

		FTimerDelegate TimerDel;

		GetWorld()->GetTimerManager().ClearTimer(refBoost->timerBoost);

		TimerDel.BindUFunction(this, FName(TEXT("RemoveBoost")), refBoost, refBoost->timerBoost);
		GetWorld()->GetTimerManager().SetTimer(refBoost->timerBoost, TimerDel, refBoost->duration, false);
	}
	// if not newBoost in listboost then add it
	else
	{
		FTimerDelegate TimerDel;

		TimerDel.BindUFunction(this, FName(TEXT("RemoveBoost")), newBoost, newBoost->timerBoost);
		listBoost.Add(newBoost);
		GetWorld()->GetTimerManager().SetTimer(newBoost->timerBoost, TimerDel, newBoost->duration, false);

		newBoost->Effect(OwnerCharacter);
	}
	OnBoostTaken();
}

void US47BoostComponent::RemoveBoost(US47BoostStruct* newBoost)
{
	newBoost->StopEffect(OwnerCharacter);

	delete newBoost;

	listBoost.Remove(newBoost);
}


void US47BoostComponent::OnBoostTaken_Implementation()
{
	OnBoostTaken_BP.Broadcast();
}
