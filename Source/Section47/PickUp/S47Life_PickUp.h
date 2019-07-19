// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S47PickUp.h"
#include "S47Life_PickUp.generated.h"

/**
 * 
 */
UCLASS()
class SECTION47_API AS47Life_PickUp : public AS47PickUp
{
	GENERATED_BODY()
	
	AS47Life_PickUp();

public:

	void Effect(class AS47PlayerCharacter* target);
};
