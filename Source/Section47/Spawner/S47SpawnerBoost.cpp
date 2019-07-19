// Fill out your copyright notice in the Description page of Project Settings.

#include "S47SpawnerBoost.h"

AS47SpawnerBoost::AS47SpawnerBoost()
{
	isRespawnable = false;
}

void AS47SpawnerBoost::BeginPlay()
{
	Super::BeginPlay();
}

void AS47SpawnerBoost::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

TSubclassOf<class AS47Boost> AS47SpawnerBoost::TakeBoost()
{
	TakeBoostBP();
	return TakeActor<AS47Boost>();
}
