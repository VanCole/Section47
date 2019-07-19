// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UnrealNetwork.h"
#include "S47DamageableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable, META = (CannotImplementInterfaceInBlueprint))
class US47DamageableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SECTION47_API IS47DamageableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable)
	virtual float GetMaxHealth() { return -1.0f;  }

	UFUNCTION(BlueprintCallable)
	virtual float GetHealth() { return -1.0f; }

	UFUNCTION(BlueprintCallable)
	virtual void SetHealth(float _health) {}

	UFUNCTION(BlueprintCallable)
	virtual bool CanBeDamaged() { return true; }

	UFUNCTION(BlueprintCallable)
	virtual void GetDamage(float _amount, class AActor* Dealer)
	{
		if (CanBeDamaged())
		{
			SetHealth(GetHealth() - _amount);
		}
	}
	
};
