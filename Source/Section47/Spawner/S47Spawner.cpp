// Fill out your copyright notice in the Description page of Project Settings.

#include "S47Spawner.h"
#include "S47Character.h"
#include "AI/S47AICharacter.h"
#include "Spawner/S47SpawnerWeapon.h"
#include "S47GameState.h"
#include "Components/StaticMeshComponent.h"
#include "S47Types.h"
#include "UnrealNetwork.h"

uint32 AS47Spawner::count = 0;

// Sets default values
AS47Spawner::AS47Spawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	RootComponent = SceneComponent;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));

	if (nullptr != MeshComponent && nullptr != SceneComponent)
		MeshComponent->SetupAttachment(SceneComponent);

	rotationSpeed = 50.0f;

	isEmpty = true;
	isRespawnable = true;
	isRotating = true;

	nbAlreadySpawn = 0;
	minSpawn = 1;
	maxSpawn = 1;
}

// Called when the game starts or when spawned
void AS47Spawner::BeginPlay()
{
	Super::BeginPlay();

	

	ID = count;
	count++;

	random.Initialize(GetWorld()->GetGameState<AS47GameState>()->Seed + ID);

	randomNumberToSpawn = random.RandRange(minSpawn, maxSpawn);

	Spawn();
}

void AS47Spawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (CurrentActor != nullptr)
	{
		CurrentActor->Destroy();
	}
}

// Called every frame
void AS47Spawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (isRotating)
	{
		RotateOnSpawner(CurrentActor, DeltaTime);
	}

	if (isEmpty)
	{
		if (isRespawnable)
		{
			if (nbAlreadySpawn >= randomNumberToSpawn)
			{
				GetWorld()->GetTimerManager().ClearTimer(timerRespawn);
			}
		}
		else
		{
			GetWorld()->GetTimerManager().ClearTimer(timerRespawn);
		}
	}
}

void AS47Spawner::RotateOnSpawner(AActor* actorToRotate, float DeltaTime)
{
	FRotator rotation = FRotator(0.0f, rotationSpeed, 0.0f);

	if (nullptr != CurrentActor)
	{
		CurrentActor->AddActorLocalRotation(rotation * DeltaTime);
	}
}

void AS47Spawner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AS47Spawner, indexSpawner);
	DOREPLIFETIME(AS47Spawner, CurrentActor);
	DOREPLIFETIME(AS47Spawner, MeshComponent);
	//DOREPLIFETIME(AS47Spawner, rotationSpeed);

	
}

int32 AS47Spawner::Lottery()
{
	int32 sum = 0;

	TArray<int32> chances;

	for (int i = 0; i < SpawnerClasses.Num(); ++i)
	{
		AActor* defaultActor = SpawnerClasses[i]->GetDefaultObject<AActor>();
		IS47SpawnableInterface* newActor = Cast<IS47SpawnableInterface>(defaultActor);

		if (nullptr != newActor)
		{
			sum += newActor->GetWeight();
			chances.Add(sum);
		}
	}

	int32 tmpResult = random.RandRange(0, sum);

	for (int i = 0; i < SpawnerClasses.Num(); ++i)
	{
		if (tmpResult < chances[i])
		{
			indexSpawner = i;
			return indexSpawner;
		}
	}

	return 0;
}

void AS47Spawner::Spawn_Implementation()
{
	TSubclassOf<AActor> classToSpawn;

	int randomLottery = Lottery();
	classToSpawn = SpawnerClasses[randomLottery];

	FVector SpawnLocation = GetActorLocation() + FVector::UpVector * 100;

	CurrentActor = GetWorld()->SpawnActor<AActor>(classToSpawn, SpawnLocation, FRotator::ZeroRotator);
	isEmpty = false;

	nbAlreadySpawn++;
}

bool AS47Spawner::Spawn_Validate()
{
	return true;
}

AActor* AS47Spawner::GetSpawnedActor()
{
	return CurrentActor;
}