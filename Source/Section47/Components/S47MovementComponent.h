// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Player/S47PlayerCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "S47MovementComponent.generated.h"
/**
 *
 */
UCLASS()
class SECTION47_API US47MovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
		//Methods

public:

	UPROPERTY()
		class AS47PlayerCharacter* character;

	void SetCharacterOwner(AS47PlayerCharacter* _character);

	US47MovementComponent(const FObjectInitializer& ObjectInitializer);

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;


	///@brief Event triggered at the end of a movement update
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector & OldLocation, const FVector & OldVelocity) override;

	//DASH 
	///@brief Triggers the dodge action.
	void DoDash();
	void DashBehavior();

	UFUNCTION(Unreliable, Server, WithValidation)
		void ServerSetMoveDirection(const FVector& MoveDir);

	UPROPERTY(EditAnywhere, Category = "Dash")
		float dashStrength;
	UPROPERTY(EditAnywhere, Category = "Dash")
		float groundDashStrengthMultiplier;
	UPROPERTY(EditAnywhere, Category = "Dash")
		float dashFuelConsumption;
	UPROPERTY(EditAnywhere, Category = "Dash")
		float dashCooldown;
		float dashCooldownTimer;
	FVector MoveDirection;
	uint8 bWantsToDash : 1;

	

	//HOVERING
	UPROPERTY(EditAnywhere, Category = "Hover")
		float hoverSlowFall;

	virtual FVector NewFallVelocity(const FVector& InitialVelocity, const FVector& Gravity, float DeltaTime) const;

	///@brief Activate or deactivate Hovering.
	void SetHover(bool bHovering);

	///@brief Flag for activating Hovering.
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	uint8 bWantsToHover : 1;


	//SUPERJUMP
	UFUNCTION(Unreliable, Server, WithValidation)
		void ServerSetWantsToSuperJump(bool wantToJump);

	void SetAllowedToSuperJump(bool bCanSuperJump);

	void SuperJumpBehavior();
	uint8 bWantsToSuperJump : 1;
	uint8 bAllowedToSuperJump : 1;

	UPROPERTY(EditAnywhere, Category = "SuperJump")
		float superJumpForce;

	UPROPERTY(EditAnywhere, Category = "SuperJump")
		float superJumpTimeWindow;
		float superJumpTimeWindowTimer;


public:
	////============================================================================================
	////Replication    TEST BY FABIEN
	////============================================================================================

	friend class FSavedMove_CustomMovement;

	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;

};


//============================================================================================
	//Replication  CLASS   TEST BY FABIEN
//============================================================================================

class FSavedMove_CustomMovement : public FSavedMove_Character
{
public:

	FVector SavedMoveDirection;
	uint8 bSavedWantsToDash : 1;
	float savedDashCooldownTimer;
	float savedSuperJumpTimeWindowTimer;

	uint8 bSavedWantsToHover : 1;

	typedef FSavedMove_Character Super;

	///@brief Resets all saved variables.
	virtual void Clear() override;

	///@brief Store input commands in the compressed flags.
	virtual uint8 GetCompressedFlags() const override;

	///@brief This is used to check whether or not two moves can be combined into one.
	///Basically you just check to make sure that the saved variables are the same.
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const override;

	///@brief Sets up the move before sending it to the server. 
	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData) override;
	
	///@brief This is used to copy state from the saved move to the character movement component.
	///This is ONLY used for predictive corrections, the actual data must be sent through RPC.
	virtual void PrepMoveFor(class ACharacter* Character) override;
};

class FNetworkPredictionData_Client_CustomMovement : public FNetworkPredictionData_Client_Character
{
public:
	FNetworkPredictionData_Client_CustomMovement(const UCharacterMovementComponent& ClientMovement);

	typedef FNetworkPredictionData_Client_Character Super;

	///@brief Allocates a new copy of our custom saved move
	virtual FSavedMovePtr AllocateNewMove() override;
};
