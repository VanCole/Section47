// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S47Types.h"
#include "Components/ActorComponent.h"
#include "S47BoostComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBoostComponentEvent);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SECTION47_API US47BoostComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	US47BoostComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
		void OnSpawnerBeginOverlap(AActor* _OverlappedActor, AActor* _OtherActor);

	void AddBoost(US47BoostStruct* newBoost);

	UFUNCTION()
	void RemoveBoost(US47BoostStruct* newBoost);

public:
	
	UFUNCTION(NetMulticast, Reliable)
	void OnBoostTaken();

	UPROPERTY(BlueprintAssignable)
	FBoostComponentEvent OnBoostTaken_BP;

	class AS47PlayerCharacter* OwnerCharacter;

	UPROPERTY(VisibleAnywhere, Category = S47BoostComponent)
		TArray<US47BoostStruct*> listBoost;
};
