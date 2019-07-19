// Fill out your copyright notice in the Description page of Project Settings.

#include "S47BoostStruct.h"
#include "Boost/S47Boost.h"
#include "Player/S47PlayerCharacter.h"
#include "Components/S47AttackComponent.h"
#include "Weapon/S47Weapon.h"
#include "GameFramework/CharacterMovementComponent.h"

bool US47BoostStruct::operator==(const US47BoostStruct& other)const
{
	return other.boost == boost;
}

US47BoostStruct::US47BoostStruct()
{
	boost = Boost_None;
}

void US47BoostStruct::Effect(AS47PlayerCharacter * target)
{
}

void US47BoostStruct::StopEffect(AS47PlayerCharacter * target)
{
}

////////////////////// SPEED UP ////////////////////////
US47SpeedUpStruct::US47SpeedUpStruct()
{
	boost = Boost_SpeedUp;
	speedMultiplicator = 1.75f;
	duration = 60.0f;
}

void US47SpeedUpStruct::Effect(AS47PlayerCharacter * target)
{
	target->GetCharacterMovement()->MaxWalkSpeed *= speedMultiplicator;
}

void US47SpeedUpStruct::StopEffect(AS47PlayerCharacter * target)
{
	ACharacter* MyChar = Cast<ACharacter>(target);

	target->GetCharacterMovement()->MaxWalkSpeed = 600.0f;
}

////////////////////// DOUBLE DAMAGE ////////////////////////
US47DoubleDamageStruct::US47DoubleDamageStruct()
{
	boost = Boost_DoubleDamage;
	duration = 60.0f;
}

void US47DoubleDamageStruct::Effect(AS47PlayerCharacter * target)
{
	target->hasDoubleDamageActive = true;
}

void US47DoubleDamageStruct::StopEffect(AS47PlayerCharacter * target)
{
	target->hasDoubleDamageActive = false;
}

////////////////////// FULL ARMOR ////////////////////////
US47FullArmorStruct::US47FullArmorStruct()
{
	boost = Boost_FullArmor;
	duration = 0.5f;
}

void US47FullArmorStruct::Effect(AS47PlayerCharacter * target)
{
	target->SetCurrentArmor(target->GetMaxArmor());
}

void US47FullArmorStruct::StopEffect(AS47PlayerCharacter * target)
{
}

////////////////////// INVINCIBILITY ////////////////////////
US47InvincibilityStruct::US47InvincibilityStruct()
{
	boost = Boost_Invincibility;
	duration = 20.0f;
}

void US47InvincibilityStruct::Effect(AS47PlayerCharacter * target)
{
	target->canBeDamaged = false;
}

void US47InvincibilityStruct::StopEffect(AS47PlayerCharacter * target)
{
	target->canBeDamaged = true;
}

////////////////////// REFILL AMMO ////////////////////////
US47RefillAmmoStruct::US47RefillAmmoStruct()
{
	boost = Boost_RefillAmmo;
	duration = 0.5f;
}

void US47RefillAmmoStruct::Effect(AS47PlayerCharacter * target)
{
	for (int32 i = 1; i < target->GetAttackComponent()->WeaponOnPlayer.Num(); ++i)
	{
		if (target->GetAttackComponent()->WeaponOnPlayer[i] != nullptr)
		{
			target->GetAttackComponent()->WeaponOnPlayer[i]->Reload();
		}
	}
}

void US47RefillAmmoStruct::StopEffect(AS47PlayerCharacter * target)
{
}