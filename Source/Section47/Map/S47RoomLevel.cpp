// Fill out your copyright notice in the Description page of Project Settings.

#include "S47RoomLevel.h"
#include "CoreUObject.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "S47Types.h"
#include "Section47.h"
#include "S47GameState.h"
#include "S47Room.h"
#include "S47RoomData.h"
#include "Player/S47PlayerCharacter.h"


uint32 AS47RoomLevel::Count = 0;

// Use this for initialization
void AS47RoomLevel::Init(US47Room* _Room)
{
	Id = Count;
	Count++;

	IsInit = false;
	Room = _Room;
	PendingInit = true;
}

AS47RoomLevel::AS47RoomLevel(const FObjectInitializer & ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	IsInit = false;
	PendingInit = false;
	Room = nullptr;
}

void AS47RoomLevel::BeginPlay()
{
	Super::BeginPlay();
}

void AS47RoomLevel::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	for (AActor* Actor : ActorsInLevel)
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
		}
	}
}

// Update is called once per frame
void AS47RoomLevel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsInit)
	{
		if (PendingInit && Room != nullptr)
		{
			//UE_LOG(LogS47Level, Log, TEXT("[%s] Init"), *GetNameSafe(GetLevel()->GetOuter()));
			Transform.SetLocation(FVector(Room->position) * US47Room::unit);
			Transform.SetRotation(FRotator(0.0f, -90.0f * (int8)Room->orientation, 0.0f).Quaternion());

			FIntVector forward = US47Room::GetDirection(Room->orientation);
			FIntVector right = US47Room::GetDirection(US47Room::Add(Room->orientation, ES47Orientation::East));

			// Create triggerBox for occlusion culling
			Center = 0.5f * (US47Room::unit * FVector(Room->position + Room->RoomToWorld(Room->prefab->size) - forward - right));
			HalfExtents = 0.5f * (US47Room::unit * FVector(Room->RoomToWorld(Room->prefab->size) - Room->position));
			HalfExtents = FVector(FMath::Abs(HalfExtents.X), FMath::Abs(HalfExtents.Y), FMath::Abs(HalfExtents.Z));


			// Discover all weapon spawners in this sublevel
			// Get All Actors in the level
			for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
			{
				ULevel *Level = ActorItr->GetLevel();
				if (Level->GetOuter() == GetLevel()->GetOuter())
				{
					//UE_LOG(LogS47Level, Verbose, TEXT("[%s] Found Actor: %s"), *GetNameSafe(GetLevel()->GetOuter()), *ActorItr->GetName());
					ActorsInLevel.Add(*ActorItr);

					AS47SpawnerWeapon* spawner = Cast<AS47SpawnerWeapon>(*ActorItr);
					if (spawner != nullptr)
					{
						WeaponSpawners.Add(spawner);
					}
				}
			}

			if (WeaponSpawners.Num() > 0 && (MinSpawner > 0 || MaxSpawner > 0))
			{
				Random.Initialize(GetWorld()->GetGameState<AS47GameState>()->Seed + 123456 * Id);

				int min = FMath::Min(FMath::Min(MinSpawner, MaxSpawner), WeaponSpawners.Num());
				int max = FMath::Max(FMath::Max(MinSpawner, MaxSpawner), WeaponSpawners.Num());

				int n = (WeaponSpawners.Num() - 1) - Random.RandRange(min, max);
				for (int i = 0; i < n; i++)
				{
					int rand = Random.RandRange(0, WeaponSpawners.Num() - 1);
					ActorsInLevel.Remove(WeaponSpawners[rand]);
					WeaponSpawners[rand]->Destroy();
					WeaponSpawners.RemoveAt(rand);
				}
			}

			PendingInit = false;
			IsInit = true;
		}
	}
	else
	{
		Display();
	}

	if (Data == nullptr)
		return;

	US47RoomData* data = Data.GetDefaultObject();

	FIntVector forward = US47Room::GetDirection(ES47Orientation::North);
	FIntVector right = US47Room::GetDirection(US47Room::Add(ES47Orientation::North, ES47Orientation::East));

	Center = 0.5f * (US47Room::unit * FVector(data->size - forward - right));
	HalfExtents = 0.5f * (US47Room::unit * FVector(data->size));
	HalfExtents = FVector(FMath::Abs(HalfExtents.X), FMath::Abs(HalfExtents.Y), FMath::Abs(HalfExtents.Z));// -FVector(0.5f, 1.0f, 0.5f);

	Center = Transform.TransformPosition(Center);

#if WITH_EDITOR
	// Pivot
	DrawDebugSphere(GetWorld(), Transform.GetLocation(), 100.0f, 4, FColor::Magenta);

	// Room bounds
	DrawDebugBox(GetWorld(), Center, HalfExtents, Transform.GetRotation(), FColor::Red);// , false, 10, 0, 1);

	// Doors
	for (int i = 0; i < data->doors.Num(); i++)
	{
		ES47Orientation ori = data->doors[i].orientation;
		FIntVector rot = US47Room::GetDirection(ori);
		FVector pos = (FVector(data->doors[i].position) * US47Room::unit) + FVector(0, 0, 200) + 0.5f * (FVector(rot) * US47Room::unit);

		pos = Transform.TransformPosition(pos);

		// Arrow
		DrawDebugDirectionalArrow(GetWorld(), pos, pos + Transform.GetRotation() * FVector(rot) * 300, 300, FColor::Blue);

		// Door frame
		FIntVector scale = US47Room::Rotate(FIntVector(20, 320, 200), ori);
		DrawDebugBox(GetWorld(), pos, FVector(scale), Transform.GetRotation(), FColor::Blue);
	}
#endif
}

bool AS47RoomLevel::IsPlayerInside()
{
	bool inside = false;

	FCollisionShape box = FCollisionShape::MakeBox(HalfExtents);

	APawn* player = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawnOrSpectator();
	TArray<FOverlapResult> overlappedActors;
	if (GetWorld()->OverlapMultiByObjectType(
		overlappedActors,
		Center,
		Transform.GetRotation(),
		FCollisionObjectQueryParams::AllDynamicObjects,
		box))
	{
		for (FOverlapResult result : overlappedActors)
		{
			if (player == result.GetActor())
			{
				inside = true;
			}
		}
	}
	return inside;
}

void AS47RoomLevel::Display()
{
	if (!IsPendingKill() && Room != nullptr && US47Room::occlusionCulling)
	{
		PlayerInside = IsPlayerInside();
		IsHidden = !PlayerInside;
		for (int i = 0; i < Room->GetConnectionCount(); i++)
		{
			if (Room->GetConnection(i) != nullptr
				&& IsValid(Room->GetConnection(i)->GetLevelScript())
				&& Room->GetConnection(i)->GetLevelScript()->PlayerInside)
			{
				IsHidden = false;
			}
		}

		// force IsHidden to false if AlwaysVisible is true
		IsHidden &= !AlwaysVisible;

		for (AActor* Actor : ActorsInLevel)
		{
			if (IsValid(Actor))
			{
				Actor->SetActorHiddenInGame(IsHidden);
			}
		}
	}
}
