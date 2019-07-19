// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "S47AIController.generated.h"

/**
 * 
 */
UCLASS()
class SECTION47_API AS47AIController : public AAIController
{
	GENERATED_BODY()
public:
	AS47AIController(const FObjectInitializer& ObjectInitializer);
	
	virtual void BeginPlay() override;

	//TeamAgent
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

};
