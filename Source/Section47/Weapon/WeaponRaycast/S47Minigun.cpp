// Fill out your copyright notice in the Description page of Project Settings.

#include "S47Minigun.h"
#include "S47Character.h"
#include "Player/S47PlayerCharacter.h"
#include "Components/S47AttackComponent.h"
#include "Components/S47CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

AS47Minigun::AS47Minigun()
{
	weightWeapon = 30;
}

void AS47Minigun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	BarrelAcceleration(DeltaTime);

	attackSpeed = customFirerate / (float)10.0f;
}

void AS47Minigun::BarrelAcceleration(float _deltaTime)
{
	APlayerController* playerController = Cast<APlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (nullptr == playerController)
		return;
	if (nullptr == Cast<AS47PlayerCharacter>(GetOwner()))
		return;
	if (!Cast<AS47PlayerCharacter>(GetOwner())->IsLocallyControlled())
		return;
	if (Cast<AS47PlayerCharacter>(GetOwner())->GetAttackComponent()->GetCurrentWeapon() != this)
	{
		customFirerate = fireRateChange;
		return;
	}

	if (!playerController->IsInputKeyDown(EKeys::LeftMouseButton) && !playerController->IsInputKeyDown(EKeys::RightMouseButton))
	{
		if (customFirerate < fireRateChange)
			customFirerate += _deltaTime;
		else
			customFirerate = fireRateChange;
	}
	else
	{
		if (customFirerate > 0.01)
		{
			customFirerate -= _deltaTime;
			if (customFirerate < 0.01)
				customFirerate = 0.01;
		}
	}
}


void AS47Minigun::Attack()
{
	if (CurrentAmmo <= 0)
	{
		OnAttackEmpty();
		return;
	}
	Super::Attack();
	OwnerCharacter = Cast<AS47PlayerCharacter>(GetOwner());
	if(OwnerCharacter != nullptr)
	{
		OwnerCharacter->FirstPersonCameraComponent->CameraShake(CAMERASHAKE::Minigun);
	}
}

void AS47Minigun::AttackBis()
{
	Super::AttackBis();
}

void AS47Minigun::StopAttackBis()
{
	Super::StopAttackBis();
}
