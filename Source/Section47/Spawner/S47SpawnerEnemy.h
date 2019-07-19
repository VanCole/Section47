// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy/S47Enemy.h"
#include "S47Types.h"
#include "S47SpawnerEnemy.generated.h"

UCLASS()
class SECTION47_API AS47SpawnerEnemy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AS47SpawnerEnemy();

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	TArray<AS47Enemy*> livingEnemies;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	int32 Lottery();

	UFUNCTION(Server, Reliable, WithValidation)
	void SpawnEnnemyRPC();

	FTransform GetRandomSpawnInABox();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void StartSpawner();

	UFUNCTION()
	void EndSpawner();

public:

	UPROPERTY(Category = S47SpawnerEnemy, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere, Category = S47SpawnerEnemy)
		TArray<FS47SpawnEnemy> EnemyWeightedClasses;

	UPROPERTY(EditAnywhere, Category = S47SpawnerEnemy)
		class UBoxComponent* BoxSpawningZone;

	UPROPERTY(Replicated)
	int32 indexSpawnerEnemy;

	FTimerHandle timerRespawn;

	UPROPERTY(EditAnywhere, Category = S47SpawnerEnemy)
	float valueTimerRespawn;

	UPROPERTY(VisibleAnywhere, Category = S47SpawnerEnemy)
	uint32 ID;

	static uint32 count;

	FRandomStream random;

	UPROPERTY(EditAnywhere, Category = S47SpawnerEnemy)
	uint32 minSpawn;

	UPROPERTY(EditAnywhere, Category = S47SpawnerEnemy)
	uint32 maxSpawn;


	UPROPERTY(EditAnywhere, Category = S47SpawnerEnemy)
	uint32 minSpawnPerTick;

	UPROPERTY(EditAnywhere, Category = S47SpawnerEnemy)
	uint32 maxSpawnPerTick;

	UPROPERTY(EditAnywhere, Category = S47SpawnerEnemy)
	float firstDelay;

	UPROPERTY(VisibleAnywhere, Category = S47SpawnerEnemy)
	uint32 randomNumberToSpawn;

	UPROPERTY(VisibleAnywhere, Category = S47SpawnerEnemy)
	uint32 nbAlreadySpawn;

	UPROPERTY(EditAnywhere, Category = S47SpawnerEnemy)
	bool findPlayer;

	UPROPERTY(EditAnywhere, Category = S47SpawnerEnemy)
	bool triggerSpawn;

	UPROPERTY(EditAnywhere, Category = S47SpawnerEnemy)
		class UBoxComponent* TriggerZone;

	UPROPERTY(VisibleAnywhere, Category = S47SpawnerEnemy)
		bool hasBeenActivated;

	UPROPERTY(EditAnywhere, Category = S47SpawnerEnemy)
	bool alwaysFindPlayer = false;

	bool dependOfSpawner = false;

	bool allEnemiesDead = false;

	float percentMobDeath = 0;

	float timeToInit = 1.0f; //Rustine initialisation après le spawner dependance
};
