// Fill out your copyright notice in the Description page of Project Settings.

#include "S47Rifle.h"
#include "UnrealNetwork.h"
#include "Player/S47PlayerCharacter.h"
#include "Components/S47AttackComponent.h"


AS47Rifle::AS47Rifle()
{
	timeSinceLastAttackBis = 0;
	isAttackingBis = false;
	weightWeapon = 50;
}

void AS47Rifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (timeSinceLastAttackBis > 0.f)
	{
		timeSinceLastAttackBis -= DeltaTime;
	}
	else if (isAttackingBis && currentAmmoBis > 0)
	{
		rifleDecalage = 8.0f;
		ConsumeAmmoBis();
		Super::Attack();
		OnAttackBis();
		timeSinceLastAttackBis = attackSpeed;		
	}
	else if (isAttackingBis && currentAmmoBis <= 0)
	{
		timeSinceLastAttackBis = attackSpeed;
		OnAttackEmpty();
	}
}

void AS47Rifle::BeginPlay()
{
	Super::BeginPlay();
	currentAmmoBis = TotalAmmo;
}

void AS47Rifle::Attack()
{
	if (CurrentAmmo > 0)
	{
		rifleDecalage = -8.0f;
		ConsumeAmmo();
		Super::Attack();
		OnAttack();
	}
	else
		OnAttackEmpty();
}

void AS47Rifle::AttackBis()
{
	//Super::AttackBis();	
	isAttackingBis = true;
}

void AS47Rifle::StopAttackBis()
{
	//Super::StopAttackBis();
	isAttackingBis = false;
}

void AS47Rifle::Reload()
{
	float trueTotalAmmo = TotalAmmo;
	AS47PlayerCharacter* PlayerOwnerCharacter = ((AS47PlayerCharacter*)OwnerCharacter);
	AS47PlayerState* PlayerState = PlayerOwnerCharacter->GetPlayerState<AS47PlayerState>();
	if (PlayerState != nullptr && PlayerState->levelAmmo > 0)
	{
		trueTotalAmmo *= PlayerState->levelAmmo * 1.1f;
	}
	CurrentAmmo = trueTotalAmmo;
	currentAmmoBis = trueTotalAmmo;
}

void AS47Rifle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AS47Rifle, currentAmmoBis);
}

void AS47Rifle::ConsumeAmmoBis_Implementation()
{
	currentAmmoBis--;
}

bool AS47Rifle::ConsumeAmmoBis_Validate()
{
	return true;
}
