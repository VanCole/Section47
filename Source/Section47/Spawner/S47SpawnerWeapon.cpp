// Fill out your copyright notice in the Description page of Project Settings.

#include "S47SpawnerWeapon.h"
#include "Engine.h"

AS47SpawnerWeapon::AS47SpawnerWeapon()
{

}

void AS47SpawnerWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AS47SpawnerWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

TSubclassOf<AS47Weapon> AS47SpawnerWeapon::TakeWeapon()
{
	//TakeWeaponBP();
	return TakeActor<AS47Weapon>();
}
