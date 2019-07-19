// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "S47GameMode.h"
#include "Map/S47LevelLockerBase.h"
#include "S47TriggerSAS.generated.h"


UCLASS()
class SECTION47_API AS47TriggerSAS : public AS47LevelLockerBase
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	AS47TriggerSAS();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, Category = "Trigger")
	TArray<class AS47PlayerCharacter*> PlayersInTrigger;

	UPROPERTY(EditAnywhere, Category = "Trigger")
	class UBoxComponent* BoxComponent;

private:
	UFUNCTION()
	void OnTriggerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnTriggerExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void UnlockBossRoom();
};
