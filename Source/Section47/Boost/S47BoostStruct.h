// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S47Types.h"
#include "S47BoostStruct.generated.h"

/**
 * 
 */
UCLASS()
class SECTION47_API US47BoostStruct : public UObject
{
	GENERATED_BODY()

public:
	US47BoostStruct();

	bool operator== (const US47BoostStruct& other) const;

	virtual void Effect(class AS47PlayerCharacter* target);

	virtual void StopEffect(class AS47PlayerCharacter* target);

//Attributes

	ES47BoostEnum boost;

	FTimerHandle timerBoost;

	UPROPERTY(EditAnywhere, Category = S47Boost)
		float duration;
};

UCLASS(BlueprintType)
class US47SpeedUpStruct : public US47BoostStruct
{
	GENERATED_BODY()

public:
	//Functions
	US47SpeedUpStruct();

	void Effect(class AS47PlayerCharacter* target);

	void StopEffect(class AS47PlayerCharacter* target);

	//Attributes
	UPROPERTY(EditAnywhere, Category = S47SpeedUp)
		float speedMultiplicator;
};

UCLASS(BlueprintType)
class US47DoubleDamageStruct : public US47BoostStruct
{
	GENERATED_BODY()

public:
	//Functions
	US47DoubleDamageStruct();

	void Effect(class AS47PlayerCharacter* target);

	void StopEffect(class AS47PlayerCharacter* target);

};

UCLASS(BlueprintType)
class US47FullArmorStruct : public US47BoostStruct
{
	GENERATED_BODY()

public:
	//Functions
	US47FullArmorStruct();

	void Effect(class AS47PlayerCharacter* target);

	void StopEffect(class AS47PlayerCharacter* target);

};

UCLASS(BlueprintType)
class US47InvincibilityStruct : public US47BoostStruct
{
	GENERATED_BODY()

public:
	//Functions
	US47InvincibilityStruct();

	void Effect(class AS47PlayerCharacter* target);

	void StopEffect(class AS47PlayerCharacter* target);

};

UCLASS(BlueprintType)
class US47RefillAmmoStruct : public US47BoostStruct
{
	GENERATED_BODY()

public:
	//Functions
	US47RefillAmmoStruct();

	void Effect(class AS47PlayerCharacter* target);

	void StopEffect(class AS47PlayerCharacter* target);

};