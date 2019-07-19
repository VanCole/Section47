// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Map/S47Door.h"
#include "S47TriggerDoor.generated.h"

/**
 * 
 */
UCLASS()
class SECTION47_API AS47TriggerDoor : public AS47Door
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, Category="Door Trigger")
	class UBoxComponent* BoxComponent;

	TArray<class AS47Character*> CharacterList;

	bool isOpen = false;

public:
	AS47TriggerDoor();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnTriggerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void OnTriggerExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintImplementableEvent, Category="Door")
	void OnOpenDoor();

	UFUNCTION(BlueprintImplementableEvent, Category = "Door")
	void OnCloseDoor();

	UFUNCTION(BlueprintCallable, Category = "Door")
	void SetRoomsAlwaysVisible(bool Visible);
};
