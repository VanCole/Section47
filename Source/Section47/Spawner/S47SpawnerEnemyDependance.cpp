// Fill out your copyright notice in the Description page of Project Settings.

#include "S47SpawnerEnemyDependance.h"
#include "S47GameState.h"
#include "Spawner/S47SpawnerEnemy.h"

// Sets default values
AS47SpawnerEnemyDependance::AS47SpawnerEnemyDependance()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	actifSpawner = 0;
}

// Called when the game starts or when spawned
void AS47SpawnerEnemyDependance::BeginPlay()
{
	Super::BeginPlay();

	for (FS47DependanceSpawner ds : dependanceSpawners)
	{
		ds.spawnerEnemy->dependOfSpawner = true;
	}
	if (!isEndRoom)
	{
		dependanceSpawners[actifSpawner].spawnerEnemy->dependOfSpawner = false;
	}
	else
	{
		AS47GameState* GameState = GetWorld()->GetGameState<AS47GameState>();
		if (nullptr != GameState)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Yellow, TEXT("LockerBase BeginPlay"));
			GameState->OnEndStartBoss.AddUniqueDynamic(this, &AS47SpawnerEnemyDependance::OnBossStart);
		}
	}

	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Yellow, TEXT("SpawnerDependance BeginPlay"));
}

void AS47SpawnerEnemyDependance::OnPostGeneration()
{
	if (isEndRoom)
	{
		SetLocked(true, true, ERoomType::Exit);
	}
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Yellow, TEXT("Lock ExitRoom and BossRoom"));
}

void AS47SpawnerEnemyDependance::OnBossEnd()
{
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Yellow, TEXT("Unlock ExitRoom and BossRoom"));
	SetLocked(false, true, ERoomType::Exit);
}

void AS47SpawnerEnemyDependance::OnBossStart()
{
	dependanceSpawners[actifSpawner].spawnerEnemy->dependOfSpawner = false;
}


// Called every frame
void AS47SpawnerEnemyDependance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetNetMode() == ENetMode::NM_Client)
		return;
	if (actifSpawner < dependanceSpawners.Num())
	{
		if (dependanceSpawners[actifSpawner].spawnerEnemy == nullptr
			|| dependanceSpawners[actifSpawner].spawnerEnemy->dependOfSpawner)
		{
			return;
		}
		bool allEnemiesFromAllSpawnerDeath = true;
		switch (dependanceSpawners[actifSpawner].dependanceMode)
		{
		case ES47DEPENDANCEMODE::NBSPAWN:
			if (!canOpenDoor)
			{
				if (dependanceSpawners[actifSpawner].spawnerEnemy->nbAlreadySpawn >= dependanceSpawners[actifSpawner].spawnerEnemy->randomNumberToSpawn)
				{
					actifSpawner++;
					if (actifSpawner < dependanceSpawners.Num())
					{
						dependanceSpawners[actifSpawner].spawnerEnemy->dependOfSpawner = false;

					}
					else
					{
					}
				}
			}
			break;
		case ES47DEPENDANCEMODE::TIME:
			timer += DeltaTime;
			if (timer > dependanceSpawners[actifSpawner].timeBetweenEachSpawner)
			{
				actifSpawner++;
				if (actifSpawner < dependanceSpawners.Num())
				{
					dependanceSpawners[actifSpawner].spawnerEnemy->dependOfSpawner = false;

				}
				else
				{
				}
				timer = 0;
			}
			break;
		case ES47DEPENDANCEMODE::ALLMOBDEATHFROMSPAWNER:
			if (dependanceSpawners[actifSpawner].percentFromMobDeath <= dependanceSpawners[actifSpawner].spawnerEnemy->percentMobDeath+1)
			{
				actifSpawner++;
				if (actifSpawner < dependanceSpawners.Num())
				{
					dependanceSpawners[actifSpawner].spawnerEnemy->dependOfSpawner = false;

				}
				else
				{
				}
			}
			break;
		case ES47DEPENDANCEMODE::ALLMOBDEATHFROMACTIVE:

			for (int i = 0; i <= actifSpawner; i++)
			{
				if (!(dependanceSpawners[actifSpawner].percentFromMobDeath <= dependanceSpawners[actifSpawner].spawnerEnemy->percentMobDeath +1))
				{
					allEnemiesFromAllSpawnerDeath = false;
				}

			}
			if (allEnemiesFromAllSpawnerDeath)
			{
				actifSpawner++;
				if (actifSpawner < dependanceSpawners.Num())
				{

			
					NextWaveWarning_BP();
					dependanceSpawners[actifSpawner].spawnerEnemy->dependOfSpawner = false;
				}

				else
				{
					//GetWorld()->GetGameState<AS47GameState>()->InvokeBossEnd();
				}

			}

			break;
		case ES47DEPENDANCEMODE::ALLMOBDEATHFROMDEPENDANCE:

			for (int i = 0; i < dependanceSpawners.Num(); i++)
			{
				if (!(dependanceSpawners[actifSpawner].percentFromMobDeath <= dependanceSpawners[actifSpawner].spawnerEnemy->percentMobDeath +1))
				{
					allEnemiesFromAllSpawnerDeath = false;
				}

			}
			if (allEnemiesFromAllSpawnerDeath)
			{
				actifSpawner++;
				if (actifSpawner < dependanceSpawners.Num())
				{
					dependanceSpawners[actifSpawner].spawnerEnemy->dependOfSpawner = false;
				}

				else
				{
					GetWorld()->GetGameState<AS47GameState>()->InvokeBossEnd();
				}
			}

			break;
		default:
			break;
		}
	}
}




