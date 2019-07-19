// Fill out your copyright notice in the Description page of Project Settings.

#include "S47Armor_PickUp.h"
#include "Player/S47PlayerCharacter.h"

AS47Armor_PickUp::AS47Armor_PickUp()
{
}

void AS47Armor_PickUp::Effect(AS47PlayerCharacter * target)
{
	Super::Effect(target);

	float tmpArmor = target->GetCurrentArmor() + (target->GetMaxArmor() * value / 100);

	if (tmpArmor > target->GetMaxArmor())
	{
		target->SetCurrentArmor(target->GetMaxArmor());
	}
	else
	{
		target->SetCurrentArmor(tmpArmor);
	}
}
