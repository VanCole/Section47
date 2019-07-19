// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/S47AIController.h"
#include "Enemy/S47AIPerceptionComponent.h"
#include "S47Character.h"

AS47AIController::AS47AIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PerceptionComponent = CreateDefaultSubobject<US47AIPerceptionComponent>("S47PerceptionComp");
}

void AS47AIController::BeginPlay()
{
	Super::BeginPlay();
}

ETeamAttitude::Type AS47AIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	return Super::GetTeamAttitudeTowards(Other);
}

