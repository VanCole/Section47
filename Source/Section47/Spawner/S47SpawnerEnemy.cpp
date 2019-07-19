// Fill out your copyright notice in the Description page of Project Settings.

#include "S47SpawnerEnemy.h"
#include "Components/BoxComponent.h"
#include "S47GameState.h"
#include "UnrealNetwork.h"
#include "S47GameInstance.h"

uint32 AS47SpawnerEnemy::count = 0;

// Sets default values
AS47SpawnerEnemy::AS47SpawnerEnemy()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	RootComponent = SceneComponent;

	BoxSpawningZone = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxSpawningZone"));

	if (nullptr != SceneComponent && nullptr != BoxSpawningZone)
		BoxSpawningZone->SetupAttachment(SceneComponent);

	TriggerZone = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerZone"));

	if (nullptr != SceneComponent && nullptr != TriggerZone)
		TriggerZone->SetupAttachment(SceneComponent);

	nbAlreadySpawn = 0;
	minSpawn = 1;
	maxSpawn = 1;
	minSpawnPerTick = 1;
	maxSpawnPerTick = 1;
	findPlayer = false;
	triggerSpawn = false;
	hasBeenActivated = false;
}

// Called when the game starts or when spawned
void AS47SpawnerEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (Role < ROLE_Authority)
	{
		return;
	}

	ID = count;
	count++;
	if (GetWorld()->GetGameState<AS47GameState>() != nullptr)
	{
		random.Initialize(GetWorld()->GetGameState<AS47GameState>()->Seed + ID);
		GetWorld()->GetGameState<AS47GameState>()->OnBossEnd.AddUniqueDynamic(this, &AS47SpawnerEnemy::EndSpawner);
	}

	randomNumberToSpawn = random.RandRange(minSpawn, maxSpawn);
	percentMobDeath = 0;



	//if (!triggerSpawn && !dependOfSpawner)
	//{
	//	/*SpawnEnnemyRPC();

	//	GetWorld()->GetTimerManager().SetTimer(timerRespawn, this, &AS47SpawnerEnemy::SpawnEnnemyRPC, valueTimerRespawn, true);
	//	hasBeenActivated = true;*/

	//	if (firstDelay > 0)
	//	{
	//		FTimerHandle handle;
	//		GetWorld()->GetTimerManager().SetTimer(handle, this, &AS47SpawnerEnemy::StartSpawner, firstDelay, false);
	//	}
	//	else
	//	{
	//		StartSpawner();
	//	}
	//	hasBeenActivated = true;
	//}
}
// Called every frame
void AS47SpawnerEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (timeToInit > 0)
	{
		timeToInit -= DeltaTime;
		return;
	}
	if (!dependOfSpawner)
	{
		if (triggerSpawn)
		{
			if (hasBeenActivated)
			{
				//SpawnEnnemyRPC();

				//GetWorld()->GetTimerManager().SetTimer(timerRespawn, this, &AS47SpawnerEnemy::SpawnEnnemyRPC, valueTimerRespawn, true);
				if (firstDelay > 0)
				{
					FTimerHandle handle;
					GetWorld()->GetTimerManager().SetTimer(handle, this, &AS47SpawnerEnemy::StartSpawner, firstDelay, false);
				}
				else
				{
					StartSpawner();
				}
				triggerSpawn = false;
			}
		}
		else
		{
			if (!hasBeenActivated)
			{
				//SpawnEnnemyRPC();

				//GetWorld()->GetTimerManager().SetTimer(timerRespawn, this, &AS47SpawnerEnemy::SpawnEnnemyRPC, valueTimerRespawn, true);
				if (firstDelay > 0)
				{
					FTimerHandle handle;
					GetWorld()->GetTimerManager().SetTimer(handle, this, &AS47SpawnerEnemy::StartSpawner, firstDelay, false);
				}
				else
				{
					StartSpawner();
				}
				hasBeenActivated = true;
			}
		}
	}
	if (nbAlreadySpawn >= randomNumberToSpawn)
	{
		percentMobDeath = 100.0f * ((float)(randomNumberToSpawn - livingEnemies.Num()) / (float)randomNumberToSpawn);
		GetWorld()->GetTimerManager().ClearTimer(timerRespawn);

		for (int i = livingEnemies.Num() - 1; i >= 0; i--)
		{
			if (nullptr == livingEnemies[i])
			{
				livingEnemies.RemoveAt(i);
			}
			else if (nullptr != livingEnemies[i])
			{
				if (!livingEnemies[i]->m_isAlive)
				{
					livingEnemies.RemoveAt(i);
				}
			}
		}
		if (livingEnemies.Num() <= 0)
		{
			percentMobDeath = 100;
			allEnemiesDead = true;
		}
	}
}

int32 AS47SpawnerEnemy::Lottery()
{
	int32 sum = 0;

	TArray<int32> chances;

	for (int i = 0; i < EnemyWeightedClasses.Num(); ++i)
	{
		if (nullptr != EnemyWeightedClasses[i].EnemyClass)
		{
			sum += EnemyWeightedClasses[i].weight;
			chances.Add(sum);
		}
	}

	int32 tmpResult = random.RandRange(0, sum);

	for (int i = 0; i < EnemyWeightedClasses.Num(); ++i)
	{
		if (tmpResult < chances[i])
		{
			indexSpawnerEnemy = i;
			return indexSpawnerEnemy;
		}
	}

	return 0;
}

void AS47SpawnerEnemy::SpawnEnnemyRPC_Implementation()
{
	if (!GetGameInstance<US47GameInstance>()->blockEnemySpawn)
	{
		TSubclassOf<AS47Enemy> classToSpawn;

		int32 nbToSpawn = FMath::Min((int32)(randomNumberToSpawn - nbAlreadySpawn), random.RandRange(minSpawnPerTick, maxSpawnPerTick));

		for (int i = 0; i < nbToSpawn; i++)
		{
			int randomLottery = Lottery();
			classToSpawn = EnemyWeightedClasses[randomLottery].EnemyClass;

			float randomDirectionSpawn = random.RandRange(0.0f, 360.0f);
			FRotator rotation = FRotator(0.0f, randomDirectionSpawn, 0.0f);

			FVector SpawnLocation = GetRandomSpawnInABox().GetLocation();

			//classToSpawn->GetDefaultObject<AS47Enemy>()->m_findPlayer = findPlayer;
			AS47Enemy* enemy = GetWorld()->SpawnActor<AS47Enemy>(classToSpawn, SpawnLocation, rotation);
			if (enemy != nullptr)
			{
				enemy->m_findPlayer = findPlayer;
				enemy->alwaysFindPlayer = alwaysFindPlayer;
				livingEnemies.Add(enemy);
				nbAlreadySpawn++;
			}

		}
	}
}

bool AS47SpawnerEnemy::SpawnEnnemyRPC_Validate()
{
	return true;
}

FTransform AS47SpawnerEnemy::GetRandomSpawnInABox()
{
	FTransform ResultTransform = BoxSpawningZone->GetComponentTransform();

	if (nullptr != BoxSpawningZone)
	{
		FVector TempRelativeLocation(FMath::RandPointInBox(FBox(-BoxSpawningZone->GetScaledBoxExtent(), BoxSpawningZone->GetScaledBoxExtent())));
		TempRelativeLocation.Z = 100.f;
		ResultTransform.SetLocation(TempRelativeLocation + ResultTransform.GetLocation());
	}

	return ResultTransform;
}

void AS47SpawnerEnemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	/*DOREPLIFETIME(AS47SpawnerEnemy, nbAlreadySpawn);
	DOREPLIFETIME(AS47SpawnerEnemy, ID);
	DOREPLIFETIME(AS47SpawnerEnemy, randomNumberToSpawn);
	DOREPLIFETIME(AS47SpawnerEnemy, minSpawn);
	DOREPLIFETIME(AS47SpawnerEnemy, maxSpawn);*/
	DOREPLIFETIME(AS47SpawnerEnemy, indexSpawnerEnemy)
}

void AS47SpawnerEnemy::StartSpawner()
{
	SpawnEnnemyRPC();

	GetWorld()->GetTimerManager().SetTimer(timerRespawn, this, &AS47SpawnerEnemy::SpawnEnnemyRPC, valueTimerRespawn, true);
}

void AS47SpawnerEnemy::EndSpawner()
{
	GetWorld()->GetTimerManager().ClearTimer(timerRespawn);
}
