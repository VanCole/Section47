// Fill out your copyright notice in the Description page of Project Settings.

//FABIEN OUVRE      INFO !!
//REPLICATION BASED ON THIS METHOD https://wiki.unrealengine.com/Authoritative_Networked_Character_Movement

#include "S47MovementComponent.h"
#include "S47Types.h"
#include "Player/S47PlayerCharacter.h"
#include "UnrealMathVectorConstants.h"

#include "UnrealNetwork.h"

#include "Engine.h"
#include "GameFramework/Character.h"

US47MovementComponent::US47MovementComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	dashStrength = 200.0f;
	groundDashStrengthMultiplier = 1.0f;
	dashFuelConsumption = 100.0f;

	hoverSlowFall = 0.3f;

	AirControl = 1.0f;
	dashCooldown = 0.25f;
	dashCooldownTimer = 0.0f;

	superJumpForce = 750.0f;
	superJumpTimeWindow = 0.25f;
	superJumpTimeWindowTimer = 0.0f;
	bAllowedToSuperJump = false;
}


void US47MovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	if (PreviousMovementMode == MOVE_Falling && MovementMode == MOVE_Walking)
	{
		if (nullptr != character)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, TEXT("OnMovementModeChanged Jump Recept"));
			character->OnJumpReception_BP();
		}
	}

	if (nullptr != character)
	{
		character->OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
	}
}


void US47MovementComponent::SetCharacterOwner(AS47PlayerCharacter* _character)
{
	character = _character;
}

//TICk
void US47MovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

	if (!CharacterOwner)
	{
		return;
	}

	//Store movement vector
	if (character->IsLocallyControlled())
	{
		MoveDirection = character->GetController()->GetPawn()->GetLastMovementInputVector();
	}
	//Send movement vector to server

	if (character->Role < ROLE_Authority)
	{
		if (bWantsToDash)
		{
			ServerSetMoveDirection(MoveDirection);
		}
		if (bWantsToSuperJump)
		{
			ServerSetWantsToSuperJump(true);
		}
	}
	
	//Update Dash movement
	if (bWantsToDash && !bAllowedToSuperJump)
	{
		DashBehavior();
	}
	if (bWantsToSuperJump && bAllowedToSuperJump)
	{
		SuperJumpBehavior();
	}

	if (bWantsToHover && character->GetCurrentFuel() <= 0.0f)
	{
		bWantsToHover = false;
		character->StopHoveringBP();
	}

	//UpdateTimer
	if (dashCooldownTimer > 0.0f)
	{
		dashCooldownTimer -= DeltaSeconds;
	}

	if (superJumpTimeWindowTimer > 0.0f)
	{
		superJumpTimeWindowTimer -= DeltaSeconds;
	}
	else
	{
		bAllowedToSuperJump = false;
	}

	//starting jetpack refuel when landing
	if (IsMovingOnGround() && !character->isStartingRefill)
	{
		character->SetFuelMultiplier(0.0f);

		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("REFILL "));

		character->isStartingRefill = true;
		character->DelegateSetFuelMultiplier(1.0f, 1.0f);
	}
}

void US47MovementComponent::SetHover(bool bHovering)
{
	bWantsToHover = bHovering;
	character->isStartingRefill = false;
	if (bWantsToHover)
	{
		character->SetFuelMultiplier(-1.0f);	
	}
	else
	{
		character->DelegateSetFuelMultiplier(0.0f, 0.25f);
	}

}

//Hover Behavior
FVector US47MovementComponent::NewFallVelocity(const FVector& InitialVelocity, const FVector& Gravity, float DeltaTime) const
{
	Super::NewFallVelocity(InitialVelocity, Gravity, DeltaTime);

	FVector Result = InitialVelocity;

	if (DeltaTime > 0.f)
	{
		// Apply gravity.
		//When engaging hover mode
		if (bWantsToHover && character->GetCurrentFuel() > 0.0f)
		{
			Result.Z = 0.0f; //reinit fall velocity
			Result += Gravity * DeltaTime * hoverSlowFall; //apply slowFall multiplier to fall velocity calculation
		}
		else //regular, unchanged fall
		{
			Result = InitialVelocity;

			Result += Gravity * DeltaTime;
		}

		// Don't exceed terminal velocity.
		const float TerminalLimit = FMath::Abs(GetPhysicsVolume()->TerminalVelocity);
		if (Result.SizeSquared() > FMath::Square(TerminalLimit))
		{
			const FVector GravityDir = Gravity.GetSafeNormal();
			if ((Result | GravityDir) > TerminalLimit)
			{
				Result = FVector::PointPlaneProject(Result, FVector::ZeroVector, GravityDir) + GravityDir * TerminalLimit;
			}
		}
	}

	return Result;
}


void US47MovementComponent::SetAllowedToSuperJump(bool bCanSuperJump)
{
	superJumpTimeWindowTimer = superJumpTimeWindow;
	bAllowedToSuperJump = bCanSuperJump;
}

void US47MovementComponent::DashBehavior()
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("DASH"));

	//Update Dash movement
	FVector DodgeVel;
	MoveDirection.Normalize();

	//Dash forward if standing still
	if (MoveDirection.X == 0 && MoveDirection.Y == 0.0f)
	{
		DodgeVel = character->GetActorForwardVector() * dashStrength;
	}
	else //Dash in input direction
	{
		DodgeVel = MoveDirection * dashStrength;
	}

	DodgeVel.Z = 0.0f;

	//reinforce dash on ground to counter ground friction
	if (IsMovingOnGround())
	{
		DodgeVel *= groundDashStrengthMultiplier;
	}

	Launch(DodgeVel);

	bWantsToDash = false;
	dashCooldownTimer = dashCooldown;

}

void US47MovementComponent::SuperJumpBehavior()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("SUPER JUMP"));
	Launch(character->GetActorUpVector() * superJumpForce);
	bWantsToSuperJump = false;
	bAllowedToSuperJump = false;
	dashCooldownTimer = dashCooldown;
}


void US47MovementComponent::DoDash()
{
	if (dashCooldownTimer <= 0.0f && dashFuelConsumption <= character->GetCurrentFuel())
	{
		character->SetCurrentFuel(character->GetCurrentFuel() - dashFuelConsumption);
		character->SetFuelMultiplier(0.0f);

		character->isStartingRefill = false;

		if (bAllowedToSuperJump)
		{
			bWantsToSuperJump = true;
		}
		else
		{
			bWantsToDash = true;
		}
		character->DoDashBP();
	}
}

//Set input flags on character from saved inputs
void US47MovementComponent::UpdateFromCompressedFlags(uint8 Flags)//Client only
{
	Super::UpdateFromCompressedFlags(Flags);

	bWantsToHover = (Flags&FSavedMove_Character::FLAG_Custom_0) != 0;
}

class FNetworkPredictionData_Client* US47MovementComponent::GetPredictionData_Client() const
{
	if (nullptr != character)
	{
		if (character->Role < ROLE_Authority)
		{
			if (!ClientPredictionData)
			{
				US47MovementComponent* MutableThis = const_cast<US47MovementComponent*>(this);

				MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_CustomMovement(*this);
				MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
				MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
			}
		}
	}
	return ClientPredictionData;
}

void FSavedMove_CustomMovement::Clear()
{
	Super::Clear();

	//Clear variables back to their default values.
	SavedMoveDirection = FVector::ZeroVector;

	bSavedWantsToHover = false;

	savedDashCooldownTimer = 0.0f;
	savedSuperJumpTimeWindowTimer = 0.0f;
}

uint8 FSavedMove_CustomMovement::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (bSavedWantsToHover)
	{
		Result |= FLAG_Custom_0;
	}

	return Result;
}


bool FSavedMove_CustomMovement::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* Character, float MaxDelta) const
{
	return Super::CanCombineWith(NewMove, Character, MaxDelta);

	//This pretty much just tells the engine if it can optimize by combining saved moves. There doesn't appear to be
	//any problem with leaving it out, but it seems that it's good practice to implement this anyways.
	if (SavedMoveDirection != ((FSavedMove_CustomMovement*)&NewMove)->SavedMoveDirection)
	{
		return false;
	}

	if (bSavedWantsToHover != ((FSavedMove_CustomMovement*)&NewMove)->bSavedWantsToHover)
	{
		return false;
	}

	if (savedDashCooldownTimer != ((FSavedMove_CustomMovement*)&NewMove)->savedDashCooldownTimer)
	{
		return false;
	}
	if (savedSuperJumpTimeWindowTimer != ((FSavedMove_CustomMovement*)&NewMove)->savedSuperJumpTimeWindowTimer)
	{
		return false;
	}
	return Super::CanCombineWith(NewMove, Character, MaxDelta);
}



void FSavedMove_CustomMovement::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, ClientData);

	US47MovementComponent* CharMov = Cast<US47MovementComponent>(Character->GetCharacterMovement());
	if (CharMov)
	{
		//This is literally just the exact opposite of UpdateFromCompressed flags. We're taking the input
		//from the player and storing it in the saved move.
		SavedMoveDirection = CharMov->MoveDirection;
		bSavedWantsToHover = CharMov->bWantsToHover;
		savedDashCooldownTimer = CharMov->dashCooldownTimer;
		savedSuperJumpTimeWindowTimer = CharMov->superJumpTimeWindowTimer;
	}
}

void FSavedMove_CustomMovement::PrepMoveFor(ACharacter * Character)
{
	Super::PrepMoveFor(Character);

	US47MovementComponent* CharMov = Cast<US47MovementComponent>(Character->GetCharacterMovement());
	if (CharMov)
	{
		//This is just the exact opposite of SetMoveFor. It copies the state from the saved move to the movement
		//component before a correction is made to a client.
		CharMov->MoveDirection = SavedMoveDirection;
		CharMov->dashCooldownTimer = savedDashCooldownTimer;
		CharMov->superJumpTimeWindowTimer = savedSuperJumpTimeWindowTimer;

		//Don't update flags here. They're automatically setup before corrections using the compressed flag methods.
	}
}

FNetworkPredictionData_Client_CustomMovement::FNetworkPredictionData_Client_CustomMovement(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{

}

FSavedMovePtr FNetworkPredictionData_Client_CustomMovement::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_CustomMovement());
}



//RPC
bool US47MovementComponent::ServerSetMoveDirection_Validate(const FVector& MoveDir)
{
	return true;
}

void US47MovementComponent::ServerSetMoveDirection_Implementation(const FVector& MoveDir)
{
	bWantsToDash = true;
	MoveDirection = MoveDir;
}

bool US47MovementComponent::ServerSetWantsToSuperJump_Validate(bool wantToJump)
{
	return true;
}

void US47MovementComponent::ServerSetWantsToSuperJump_Implementation(bool wantToJump)
{
	bWantsToSuperJump = wantToJump;
}

