// Fill out your copyright notice in the Description page of Project Settings.

#include "S47Door.h"
#include "S47Room.h"
#include "S47RoomLevel.h"

// Sets default values
AS47Door::AS47Door()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
}

// Called when the game starts or when spawned
void AS47Door::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AS47Door::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	locked = !AlwaysUnlocked && 
		  ((m_pRoomA == nullptr || (m_pRoomA->GetLevelScript() != nullptr && m_pRoomA->GetLevelScript()->IsLocked))
		|| (m_pRoomB == nullptr || (m_pRoomB->GetLevelScript() != nullptr && m_pRoomB->GetLevelScript()->IsLocked)));

	SetActorHiddenInGame( !AlwaysVisible &&
			(m_pRoomA == nullptr || (m_pRoomA->GetLevelScript() != nullptr && m_pRoomA->GetLevelScript()->IsHidden)) 
		&&	(m_pRoomB == nullptr || (m_pRoomB->GetLevelScript() != nullptr && m_pRoomB->GetLevelScript()->IsHidden)));

	if (locked != prevLocked)
	{
		if (locked)
		{
			OnDoorLock();
			OnDoorLock_BP();
		}
		else
		{
			OnDoorUnlock();
			OnDoorUnlock_BP();
		}
	}

	prevLocked = locked;
}

void AS47Door::OnDoorLock()
{

	//red light
}

void AS47Door::OnDoorUnlock()
{
	//green + sound
}

void AS47Door::SetConnectingRooms(US47Room * _roomA, US47Room * _roomB)
{
	m_pRoomA = _roomA;
	m_pRoomB = _roomB;
}

