// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "S47GameMode.h"
#include "S47LevelLockerBase.h"
#include "S47TriggerEndLevel.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SECTION47_API AS47TriggerEndLevel : public AS47LevelLockerBase
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	AS47TriggerEndLevel();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere)
	int32 NumPlayer;

	UPROPERTY(VisibleAnywhere)
	int32 NumPlayerInTrigger;

	UPROPERTY(VisibleAnywhere)
	TArray<class AS47PlayerCharacter*> PlayersInTrigger;

	UPROPERTY(EditAnywhere)
	float TimeBeforeEnd;

	FTimerHandle Timer;

	UPROPERTY()
	AS47GameMode* GameMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* Box;

private:

	UFUNCTION()
	void OnTriggerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnTriggerExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	UFUNCTION()
	void EndLevel();
};
