// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S47PickUp.h"
#include "S47Armor_PickUp.generated.h"

/**
 * 
 */
UCLASS()
class SECTION47_API AS47Armor_PickUp : public AS47PickUp
{
	GENERATED_BODY()

	AS47Armor_PickUp();
	
public:

	void Effect(class AS47PlayerCharacter* target);
};
