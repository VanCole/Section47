// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "S47Types.h"
#include "S47CameraComponent.generated.h"

/**
 * 
 */
UCLASS()
class SECTION47_API US47CameraComponent : public UCameraComponent
{
	GENERATED_BODY()

	

public :
	
	
	void CameraShake(CAMERASHAKE _index);

protected:

	UPROPERTY(EditAnywhere, Category = S47CameraShake)
	TArray<TSubclassOf<class UCameraShake>> cameraShake;

};
