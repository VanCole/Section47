// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "S47Character.h"
#include "Spawner/S47SpawnableInterface.h"
#include "S47Types.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "S47Boost.generated.h"

UCLASS()
class SECTION47_API AS47Boost : public AActor, public IS47SpawnableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AS47Boost();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// functions IS47SpawnableInterface
	int32 GetWeight();

public:

	UPROPERTY(Category = S47Boost, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* SceneComponent;

	UPROPERTY(Replicated, Category = S47Boost, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, Category = S47Boost)
		int32 weightBoost;

	UPROPERTY(Replicated, VisibleAnywhere, Category = S47Boost)
		class US47BoostStruct* boostStruct;

	UPROPERTY(Replicated, EditAnywhere, Category = S47Boost)
		TSubclassOf<class US47BoostStruct> boostClass;
};
