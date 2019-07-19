// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "S47Types.h"
#include "Map/S47LevelLockerBase.h"
#include "S47SpawnerEnemyDependance.generated.h"

UCLASS()
class SECTION47_API AS47SpawnerEnemyDependance : public AS47LevelLockerBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AS47SpawnerEnemyDependance();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnPostGeneration() override;
	virtual void OnBossEnd() override;

	UFUNCTION()
	void OnBossStart();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = S47DependanceSpawner)
	TArray<FS47DependanceSpawner> dependanceSpawners;

	UPROPERTY(EditAnywhere, Category = S47DependanceSpawner)
		bool isEndRoom = true;

	UFUNCTION(BlueprintImplementableEvent)
		void NextWaveWarning_BP();


	int actifSpawner = 0;
	bool canOpenDoor = false;
	float timer = 0;
};
