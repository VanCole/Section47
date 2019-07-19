// Fill out your copyright notice in the Description page of Project Settings.

#include "S47Shotgun.h"
#include "Player/S47PlayerCharacter.h"
#include "Components/S47CameraComponent.h"

AS47Shotgun::AS47Shotgun()
{
	weightWeapon = 50;
}

void AS47Shotgun::Attack()
{
	if (CurrentAmmo <= 0)
	{
		OnAttackEmpty();
		return;
	}

	ConsumeAmmo();
	for (int i = 0; i < 10; i++)
	{
		Super::Attack();
	}
	OnAttack();
	OwnerCharacter = Cast<AS47PlayerCharacter>(GetOwner());
	if (nullptr != OwnerCharacter)
	{
		OwnerCharacter->FirstPersonCameraComponent->CameraShake(CAMERASHAKE::Shotgun);
	}
}

void AS47Shotgun::AttackBis()
{
	SemiAuto = false;
	attackSpeed = 0.25;
}

void AS47Shotgun::StopAttackBis()
{
	SemiAuto = true;
	attackSpeed = 0.5;
}
