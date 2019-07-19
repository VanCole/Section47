// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "S47Character.h"
#include "S47AICharacter.generated.h"

/**
 * 
 */
UCLASS()
class SECTION47_API AS47AICharacter : public AS47Character
{
	GENERATED_BODY()
	
public :
	virtual void PossessedBy(AController* NewController) override;

	virtual void BeginPlay() override;


public: // Interface IS47Damageable
	virtual float GetMaxHealth() override { return maxHealth; }
	virtual float GetHealth() override { return currentHealth; }
	virtual void SetHealth(float _health) override;
	virtual void GetDamage(float _amount, AActor* Dealer) override;

	UFUNCTION(BlueprintCallable)
	void SaveLastShot(FVector _force_Saved, FVector _poshit_Saved, FName _boneTouched_Saved);	
	UFUNCTION(NetMulticast, Unreliable)
	void SaveLastShot_ToClient(FVector _force_Saved, FVector _poshit_Saved, FName _boneTouched_Saved);


protected :
	float maxHealth;
	UPROPERTY(Category = AAA, VisibleAnywhere)
	float currentHealth;

	//to save to last shot taken
	UPROPERTY(Category = S47Ragdoll, VisibleAnywhere, BlueprintReadOnly)
	UPrimitiveComponent* component_Saved;
	UPROPERTY(Category = S47Ragdoll, VisibleAnywhere, BlueprintReadOnly)
	FVector force_Saved;
	UPROPERTY(Category = S47Ragdoll, VisibleAnywhere, BlueprintReadOnly)
	FVector poshit_Saved;
	UPROPERTY(Category = S47Ragdoll, VisibleAnywhere, BlueprintReadOnly)
	FName boneTouched_Saved;

	UFUNCTION()
	void OnDeath(AController* _controller, AActor* killer);
};
