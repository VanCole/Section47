// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "S47Door.generated.h"

class US47Room;

UCLASS()
class SECTION47_API AS47Door : public AActor
{
	GENERATED_BODY()
	
public:	
	AS47Door();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

protected:
	UFUNCTION()
	virtual void OnDoorLock();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Locked"))
	void OnDoorLock_BP();

	UFUNCTION()
	virtual void OnDoorUnlock();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Unlocked"))
	void OnDoorUnlock_BP();

protected:
	bool locked = false;
	US47Room* m_pRoomA;
	US47Room* m_pRoomB;

	UPROPERTY(EditAnywhere, Category = "Door")
	bool AlwaysVisible = false;

	UPROPERTY(EditAnywhere, Category = "Door")
	bool AlwaysUnlocked = false;

private:
	bool prevLocked = false;

public:
	void SetConnectingRooms(US47Room* _roomA, US47Room* _roomB);

	bool GetLocked() { return locked; }
};
