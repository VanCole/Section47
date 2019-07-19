// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "S47SpawnableInterface.h"
#include "S47Spawner.generated.h"

UCLASS()
class SECTION47_API AS47Spawner : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AS47Spawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void RotateOnSpawner(AActor* actorToRotate, float DeltaTime);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	UFUNCTION(BlueprintCallable)
		AActor* GetSpawnedActor();

	template<typename T>
	TSubclassOf<T> GetActorFromSpawner()
	{
		TSubclassOf<AActor> NewActor = nullptr;

		NewActor = SpawnerClasses[indexSpawner];

		if (NewActor->IsChildOf(T::StaticClass()))
		{
			TSubclassOf<T> ActorClass = *NewActor;
			return ActorClass;
		}

		return nullptr;
	}

	template<typename T>
	TSubclassOf<T> TakeActor()
	{
		if (CurrentActor != nullptr)
		{
			CurrentActor->Destroy();
		}

		isEmpty = true;
		CurrentActor = nullptr;

		GetWorld()->GetTimerManager().SetTimer(timerRespawn, this, &AS47Spawner::Spawn, valueTimerRespawn, false);

		return GetActorFromSpawner<T>();
	}


	int32 Lottery();

	UFUNCTION(Server, Reliable, WithValidation)
		void Spawn();

public:

	UFUNCTION(BlueprintImplementableEvent)
		void TakeActorBP();

	UPROPERTY(Replicated,VisibleAnywhere, Category = S47Spawner)
		class AActor* CurrentActor;

	UPROPERTY(Category = S47Spawner, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* SceneComponent;

	UPROPERTY(Replicated,VisibleAnywhere, Category = S47Spawner, META = (MustImplement = "S47SpawnableInterface"))
		class UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, Category = S47Spawner, META = (MustImplement= "S47SpawnableInterface"))
		TArray< TSubclassOf<AActor>> SpawnerClasses;

	UPROPERTY(EditAnywhere, Category = S47Spawner)
		float rotationSpeed;

	UPROPERTY(Replicated)
		int32 indexSpawner;

	bool isEmpty;

	UPROPERTY(EditAnywhere, Category = S47Spawner)
		bool isRespawnable;

	FTimerHandle timerRespawn;

	UPROPERTY(EditAnywhere, Category = S47Spawner)
		float valueTimerRespawn;

	UPROPERTY(EditAnywhere, Category = S47Spawner)
		bool isRotating;

	UPROPERTY(VisibleAnywhere, Category = S47Spawner)
		uint32 ID;

	static uint32 count;

	FRandomStream random;

	UPROPERTY(EditAnywhere, Category = S47Spawner)
		uint32 minSpawn;

	UPROPERTY(EditAnywhere, Category = S47Spawner)
		uint32 maxSpawn;

	UPROPERTY(VisibleAnywhere, Category = S47Spawner)
		uint32 randomNumberToSpawn;

	UPROPERTY(VisibleAnywhere, Category = S47Spawner)
		uint32 nbAlreadySpawn;
};