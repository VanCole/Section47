// Fill out your copyright notice in the Description page of Project Settings.

#include "S47Life_PickUp.h"
#include "Player/S47PlayerCharacter.h"

AS47Life_PickUp::AS47Life_PickUp()
{
}

void AS47Life_PickUp::Effect(AS47PlayerCharacter * target)
{
	Super::Effect(target);

	float tmpHealth = target->GetHealth() + (target->GetMaxHealth() * value / 100);

	if (tmpHealth > target->GetMaxHealth())
	{
		target->SetHealth(target->GetMaxHealth());
	}
	else
	{
		target->SetHealth(tmpHealth);
	}
}
