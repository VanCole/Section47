// Fill out your copyright notice in the Description page of Project Settings.

#include "S47TriggerDoor.h"
#include "Components/BoxComponent.h"
#include "S47Character.h"
#include "S47Types.h"
#include "S47Room.h"
#include "S47RoomLevel.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Enemy/S47Enemy.h"

AS47TriggerDoor::AS47TriggerDoor()
{
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));

	if (RootComponent != nullptr)
	{
		BoxComponent->SetupAttachment(RootComponent);
	}
}

void AS47TriggerDoor::BeginPlay()
{
	Super::BeginPlay();

	if (nullptr != BoxComponent)
	{
		BoxComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &AS47TriggerDoor::OnTriggerEnter);
		BoxComponent->OnComponentEndOverlap.AddUniqueDynamic(this, &AS47TriggerDoor::OnTriggerExit);
	}
}

void AS47TriggerDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!isOpen && !locked && CharacterList.Num() > 0)
	{
		OnOpenDoor();
		isOpen = true;
	}
	else if (isOpen && (locked || CharacterList.Num() <= 0))
	{
		OnCloseDoor();
		isOpen = false;
	}
}

void AS47TriggerDoor::SetRoomsAlwaysVisible(bool _visible)
{
	if (nullptr != m_pRoomA && nullptr != m_pRoomA->GetLevelScript())
	{
		m_pRoomA->GetLevelScript()->AlwaysVisible = _visible;
	}
	if (nullptr != m_pRoomB && nullptr != m_pRoomB->GetLevelScript())
	{
		m_pRoomB->GetLevelScript()->AlwaysVisible = _visible;
	}
}

void AS47TriggerDoor::OnTriggerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	AS47Character* OtherCharacter = Cast<AS47Character>(OtherActor);
	UCapsuleComponent* OtherCapsule = Cast<UCapsuleComponent>(OtherComp);


	if (OtherCharacter != nullptr && OtherCapsule != nullptr && OtherCapsule == OtherCharacter->GetCapsuleComponent() && !CharacterList.Contains(OtherCharacter))
	{
		CharacterList.Add(OtherCharacter);

		AS47Enemy* OtherCharacter = Cast<AS47Enemy>(OtherActor);
		if (OtherCharacter != nullptr && OtherCharacter->GetCharacterMovement()->CanEverCrouch())
		{
			OtherCharacter->nbDoorEnter++;
			OtherCharacter->CrouchBP();
		}

		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, *FString::Printf(TEXT("[%s] Character Enter %s"), GET_NETMODE_TEXT(), *GetNameSafe(OtherCharacter)));
	}

}

void AS47TriggerDoor::OnTriggerExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AS47Character* OtherCharacter = Cast<AS47Character>(OtherActor);
	UCapsuleComponent* OtherCapsule = Cast<UCapsuleComponent>(OtherComp);

	if (OtherCharacter != nullptr && OtherCapsule != nullptr && OtherCapsule == OtherCharacter->GetCapsuleComponent() && CharacterList.Contains(OtherCharacter))
	{
		CharacterList.Remove(OtherCharacter);

		AS47Enemy* OtherCharacter = Cast<AS47Enemy>(OtherActor);
		if (OtherCharacter != nullptr && OtherCharacter->GetCharacterMovement()->CanEverCrouch())
		{
			OtherCharacter->nbDoorEnter--;
			if (OtherCharacter->nbDoorEnter <= 0)
			{
				OtherCharacter->UnCrouchBP();
			}
		}

		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Orange, *FString::Printf(TEXT("[%s] Character Exit %s"), GET_NETMODE_TEXT(), *GetNameSafe(OtherCharacter)));
	}
}
