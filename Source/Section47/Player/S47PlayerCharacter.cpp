// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "Player/S47PlayerCharacter.h"
#include "Projectile/S47Projectile.h"
#include "Projectile/S47Impact.h"
#include "Components/S47CameraComponent.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "S47GameInstance.h"
#include "Components/S47MovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/S47AttackComponent.h"
#include "Components/S47BoostComponent.h"
#include "UnrealNetwork.h"
#include "Section47.h"
#include "S47GameState.h"
#include "Map/S47LevelStart.h"
#include "Perception/AISense_Hearing.h"

#include "Engine.h"


DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AS47PlayerCharacter

AS47PlayerCharacter::AS47PlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<US47MovementComponent>(ACharacter::CharacterMovementComponentName))
{
	bAllowTickBeforeBeginPlay = 0;

	AttackComponent = CreateDefaultSubobject<US47AttackComponent>(TEXT("AttackComponent"));
	if (AttackComponent)
	{
		AttackComponent->SetIsReplicated(true);
	}

	BoostComponent = CreateDefaultSubobject<US47BoostComponent>(TEXT("BoostComponent"));
	if (BoostComponent)
	{
		BoostComponent->SetIsReplicated(true);
	}

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<US47CameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(false);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	TArray<AActor*> AllController;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AController::StaticClass(), AllController);

	for (auto Controller : AllController)
	{
		APlayerController* PlayerController = Cast<APlayerController>(Controller);
		if (nullptr != PlayerController)
		{
			if (nullptr == PlayerController->GetPawn())
			{
				PlayerController->Possess(this);
			}
		}
	}
}

void AS47PlayerCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	
	hasDoubleDamageActive = false;

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	//ICI//FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	//GetPawn()->OnRep_PlayerState;

	//AS47PlayerState* PlayerS = Cast<AS47PlayerState>(this->PlayerState);
	//AGameMode::PlayerStateClass

	//AS47PlayerState* playerState = Cast<AS47PlayerState>(GetController()->GetPawn()->GetPlayerState());
	//playerState->GetPlayerLife();


	moveComponent = Cast<US47MovementComponent>(GetMovementComponent());

	if (nullptr != moveComponent)
	{
		moveComponent->SetCharacterOwner(this);
	}

	isInGameMenu = false;

	playerController = Cast<APlayerController>(GetController());

	AS47GameState* gameState = Cast<AS47GameState>(GetWorld()->GetGameState());
	if (nullptr != gameState)
	{
		gameState->OnEndLevel.AddUniqueDynamic(this, &AS47PlayerCharacter::EnableEndGame);
	}

	//GetAttackComponent()->OwnerCharacter = this;

	else
	{
		UE_LOG(LogS47HUD, Error, TEXT("gameState is nullptr"));
	}



	AS47GameState* GameState = GetWorld()->GetGameState<AS47GameState>();
	if (nullptr != GameState)
	{
		GameState->OnPostGeneration.AddUniqueDynamic(this, &AS47PlayerCharacter::TeleportToSpawn);
	}

	onEndDeathAnimation.AddUniqueDynamic(this, &AS47PlayerCharacter::EnterSpectatorMode);
}


//////////////////////////////////////////////////////////////////////////
// Input

void AS47PlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AS47PlayerCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AS47PlayerCharacter::StopJumping);

	// Bind fire event

	// Bind fire event
	//PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AS47PlayerCharacter::OnFire);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AS47PlayerCharacter::Action);
	PlayerInputComponent->BindAction("Attack", IE_Released, this, &AS47PlayerCharacter::StopAction);

	PlayerInputComponent->BindAction("AttackBis", IE_Pressed, this, &AS47PlayerCharacter::ActionBis);
	PlayerInputComponent->BindAction("AttackBis", IE_Released, this, &AS47PlayerCharacter::StopActionBis);

	PlayerInputComponent->BindAction("FrontalLight", IE_Pressed, this, &AS47PlayerCharacter::OnFrontalLightBP);

	PlayerInputComponent->BindAxis("ChangeWeapon", AttackComponent, &US47AttackComponent::ChangeWeapon);

	PlayerInputComponent->BindAction("Kick", IE_Pressed, AttackComponent, &US47AttackComponent::Kick);

	PlayerInputComponent->BindAction("ChooseWeapon1", IE_Pressed, AttackComponent, &US47AttackComponent::ChooseWeapon<ES47WeaponIndex::Weapon_1>);
	PlayerInputComponent->BindAction("ChooseWeapon2", IE_Pressed, AttackComponent, &US47AttackComponent::ChooseWeapon<ES47WeaponIndex::Weapon_2>);
	PlayerInputComponent->BindAction("ChooseWeapon3", IE_Pressed, AttackComponent, &US47AttackComponent::ChooseWeapon<ES47WeaponIndex::Weapon_3>);


	PlayerInputComponent->BindAction("ThrowWeapon", IE_Pressed, this, &AS47PlayerCharacter::ThrowWeaponAction);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AS47PlayerCharacter::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AS47PlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AS47PlayerCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices thFat provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AS47PlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AS47PlayerCharacter::LookUpAtRate);


	//CUSTOM Movement behavior
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &AS47PlayerCharacter::Dash);

	PlayerInputComponent->BindAction("Hover", IE_Pressed, this, &AS47PlayerCharacter::StartHovering);
	PlayerInputComponent->BindAction("Hover", IE_Released, this, &AS47PlayerCharacter::StopHovering);

	//Pause Menu
	PlayerInputComponent->BindAction("PauseMenu", IE_Pressed, this, &AS47PlayerCharacter::ShowHideInGameMenu);
}





void AS47PlayerCharacter::Jump()
{
	Super::Jump();
	US47MovementComponent* MoveComp = Cast<US47MovementComponent>(GetCharacterMovement());
	if (MoveComp)
	{
		if (HasAuthority())
		{
			OnJumpStart_ToClient();
		}
		else
		{
			OnJumpStart_ToServ();
			OnJumpStart_BP();
		}


		if (MoveComp->MovementMode == MOVE_Walking)
		{
			UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1, this, 1000);
			MoveComp->SetAllowedToSuperJump(true);
		}
	}
}


void AS47PlayerCharacter::StopJumping()
{
	Super::StopJumping();

}

void AS47PlayerCharacter::StartHovering()
{
	US47MovementComponent* MoveComp = Cast<US47MovementComponent>(GetCharacterMovement());
	if (MoveComp)
	{
		if (MoveComp->Velocity.Z < 0.0f && GetCurrentFuel() > 0.0f && MoveComp->MovementMode == MOVE_Falling)
		{
			if (!MoveComp->bWantsToHover)
			{
				StartHoveringBP();
			}

			//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, ("Start hover"));
			UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1, this, 1000);
			MoveComp->SetHover(true);			
		}
	}
}

void AS47PlayerCharacter::StopHovering()
{
	US47MovementComponent* MoveComp = Cast<US47MovementComponent>(GetCharacterMovement());
	if (MoveComp)
	{
		if (MoveComp->bWantsToHover)
		{
			StopHoveringBP();

			MoveComp->SetHover(false);
		}
	}
}


void AS47PlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (nullptr != moveComponent)
	{
		//Start hovering when player keep hover input pressed
		if (nullptr != playerController)
		{
			AS47PlayerState* state = GetPlayerState<AS47PlayerState>();

			if (nullptr != state)
			{
				//Hover when key is being pressed
				if (playerController->GetInputKeyTimeDown(EKeys::SpaceBar) >= 0.01)
				{
					StartHovering();
				}
			}
		}
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("obstacle Height: %d"), obstacleHeight));
	}
	

	UpdateFuel(DeltaTime);

//#if WITH_EDITOR
	if (nullptr != playerController)
	{
		// Debug key to refill life
		if (playerController->WasInputKeyJustPressed(EKeys::L))
		{
			RefillLife();
		}
		// Debug key to kill player
		else if (playerController->WasInputKeyJustPressed(EKeys::K))
		{
			Kill();
		}
		// Debug key to show FPS
		else if (playerController->WasInputKeyJustPressed(EKeys::F))
		{
			UKismetSystemLibrary::ExecuteConsoleCommand(GetWorld(), "stat FPS");
		}
	}
//#endif
}

float AS47PlayerCharacter::GetPlayerLifePercentage()
{
	AS47PlayerState* state = GetPlayerState<AS47PlayerState>();

	if (nullptr != state)
	{
		return state->CurrentHealth / state->MaxHealth;
	}

	return 0.0f;
}

FText AS47PlayerCharacter::GetPlayerLifeIntText()
{
	int32 HP = FMath::RoundHalfFromZero(GetPlayerLifePercentage() * 100);
	FString HPS = FString::FromInt(HP);
	FString playerLifeHUD = HPS + FString(TEXT("%"));
	FText HPText = FText::FromString(playerLifeHUD);
	return HPText;
}

float AS47PlayerCharacter::GetPlayerArmorPercentage()
{
	AS47PlayerState* state = GetPlayerState<AS47PlayerState>();

	if (nullptr != state)
	{
		return state->CurrentArmor / state->MaxArmor;
	}

	return 0.0f;
}

FText AS47PlayerCharacter::GetPlayerArmorIntText()
{
	int32 HP = FMath::RoundHalfFromZero(GetPlayerArmorPercentage() * 100);
	FString HPS = FString::FromInt(HP);
	FString playerArmorHUD = HPS + FString(TEXT("%"));
	FText HPText = FText::FromString(playerArmorHUD);
	return HPText;
}

//FUEL METHODS ==========================================================================
float AS47PlayerCharacter::GetPlayerFuelPercentage()
{
	return playerFuelPercentage;
}

FText AS47PlayerCharacter::GetPlayerFuelIntText()
{
	int32 HP = FMath::RoundHalfFromZero(GetPlayerFuelPercentage() * 100);
	FString HPS = FString::FromInt(HP);
	FString playerFuelHUD = HPS + FString(TEXT("%"));
	FText HPText = FText::FromString(playerFuelHUD);
	return HPText;
}


void AS47PlayerCharacter::UpdateFuel(float DeltaTime)
{
	AS47PlayerState* state = GetPlayerState<AS47PlayerState>();

	//Decrease or increase fuel amount depending on multiplier value
	playerFuelTick += DeltaTime;

	if (nullptr != state)
	{
		if (playerFuelTick >= playerFuelTickMax)
		{
			state->CurrentFuel += playerFuelPerTick * playerFuelMultiplier;

			UpdateFuelParam(state->CurrentFuel);
			playerFuelTick = 0.0f;
		}

		//Lock fuel at max amount when tank full
		if (state->CurrentFuel >= state->MaxFuel)
		{
			state->CurrentFuel = state->MaxFuel;
			SetFuelMultiplier(0.0f);
		}
	}
	else
	{
		UE_LOG(LogS47Character, Error, TEXT("state is nullptr"));
	}
}

void AS47PlayerCharacter::UpdateFuelParam(float fuelChange)
{
	AS47PlayerState* state = GetPlayerState<AS47PlayerState>();

	if (nullptr != state)
	{
		SetCurrentFuel(FMath::Clamp(fuelChange, 0.0f, GetMaxFuel()));
		playerFuelPercentage = GetCurrentFuel() / GetMaxFuel();
	}
	else
	{
		UE_LOG(LogS47Character, Error, TEXT("state is nullptr"));
	}
}

void AS47PlayerCharacter::SetFuelMultiplier(float multiplierValue)
{
	playerFuelMultiplier = multiplierValue;
	GetWorld()->GetTimerManager().ClearTimer(FuelTimerHandle);
}

//Set fuel multiplier after set delay
void AS47PlayerCharacter::DelegateSetFuelMultiplier(float multiplierValue, float delay)
{
	FTimerDelegate FuelDel;
	FuelDel.BindUFunction(this, FName("SetFuelMultiplier"), multiplierValue); //set fuel multiplier to X
	GetWorld()->GetTimerManager().SetTimer(FuelTimerHandle, FuelDel, delay, false);   //Start refuelling after Y second
}


void AS47PlayerCharacter::Dash()
{
	US47MovementComponent* MoveComp = Cast<US47MovementComponent>(GetCharacterMovement());
	if (nullptr != MoveComp)
	{
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1, this, 1000);
		MoveComp->DoDash();
	}
}

void AS47PlayerCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AS47PlayerCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		//OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AS47PlayerCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

void AS47PlayerCharacter::ShowHideInGameMenu()
{
	isInGameMenu = !isInGameMenu;

	onShowInGameMenu.Broadcast();
}

void AS47PlayerCharacter::EnableEndGame()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("This is an on screen message!"));
	//moveComponent->Velocity = FVector(0.0f, 0.0f, 0.0f);
	moveComponent->StopMovementImmediately();
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

void AS47PlayerCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1, this, 1000);
		isRunningForward = Value;
		AddMovementInput(GetActorForwardVector(), Value);
	}
	else
	{
		isRunningForward = 0.0f;
		AddMovementInput(GetActorForwardVector(), 0);
	}
}

void AS47PlayerCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1, this, 1000);
		isRunningRight = Value;
		AddMovementInput(GetActorRightVector(), Value);
	}
	else
	{
		isRunningRight = 0.0f;
		AddMovementInput(GetActorRightVector(), 0);
	}
}

void AS47PlayerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information

	if (nullptr != GetPlayerState())
	{
		AS47PlayerState* playerState = Cast<AS47PlayerState>(GetPlayerState());

		if (nullptr != playerState)
		{
			AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds() * (playerState->mouseSensibility * 2));
		}
	}
}

void AS47PlayerCharacter::LookUpAtRate(float Rate)
{
	if (nullptr != GetPlayerState())
	{
		AS47PlayerState* playerState = Cast<AS47PlayerState>(GetPlayerState());

		if (nullptr != playerState)
		{
			AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds() * (playerState->mouseSensibility * 2));
		}
	}
}

bool AS47PlayerCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AS47PlayerCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &AS47PlayerCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AS47PlayerCharacter::TouchUpdate);
		return true;
	}

	return false;
}

float AS47PlayerCharacter::GetMaxHealth()
{
	AS47PlayerState* state = GetPlayerState<AS47PlayerState>();

	if (nullptr != state)
	{
		return state->MaxHealth;
	}

	return -1.0f;
}

float AS47PlayerCharacter::GetHealth()
{
	AS47PlayerState* state = GetPlayerState<AS47PlayerState>();

	return state != nullptr ? state->CurrentHealth : -1.0f;
}

void AS47PlayerCharacter::SetHealth(float _health)
{
	AS47PlayerState* state = GetPlayerState<AS47PlayerState>();

	if (state != nullptr && state->CurrentHealth > 0)
	{
		float delta = _health - state->CurrentHealth;
		state->CurrentHealth = FMath::Clamp(_health, 0.0f, GetMaxHealth());

		//UpdatePlayerLife();

		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Orange, *FString::Printf(TEXT("%s: %s Health set to %f"), GET_NETMODE_TEXT(), *GetName(), state->CurrentHealth));
		if (!state->IsAlive())
		{
			if (nullptr != GetController<APlayerController>())
			{
				GetController<APlayerController>()->SetInputMode(FInputModeUIOnly());
			}

			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Player Dead!"));
			MoveRight(0);
			MoveForward(0);
			//GetCapsuleComponent()->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
			//GetController<APlayerController>()->InputMotion(FVector::ZeroVector, FVector::ZeroVector, FVector::ZeroVector, FVector::ZeroVector);

			InvokeDeath();
		}

		onHealthChangeEvent.Broadcast(delta, _health);
		
		if (delta > 0)
		{
			onHealEvent.Broadcast(delta, _health);
		}
		else if (delta < 0)
		{
			onGetDamageEvent.Broadcast(delta, _health);
		}
	}
	else
	{
		UE_LOG(LogS47Character, Error, TEXT("state is nullptr"));
	}
}

void AS47PlayerCharacter::Action()
{
	ServerAttack();
}

void AS47PlayerCharacter::ServerAttack_Implementation()
{
	if (nullptr != AttackComponent)
	{
		AttackComponent->Attack();
	}
}

bool AS47PlayerCharacter::ServerAttack_Validate()
{
	return true;
}

void AS47PlayerCharacter::StopAction()
{
	ServerStopAction();
}

void AS47PlayerCharacter::ServerStopAction_Implementation()
{
	if (nullptr != AttackComponent)
	{
		AttackComponent->StopAttack();
	}
}

bool AS47PlayerCharacter::ServerStopAction_Validate()
{
	return true;
}

void AS47PlayerCharacter::ThrowWeaponAction()
{
	ServerThrowWeaponAction();
}

void AS47PlayerCharacter::ServerThrowWeaponAction_Implementation()
{
	if (nullptr != AttackComponent)
	{
		AttackComponent->ThrowCurrentWeapon();
	}
}

bool AS47PlayerCharacter::ServerThrowWeaponAction_Validate()
{
	return true;
}

void AS47PlayerCharacter::ActionBis()
{
	AttackComponent->AttackBis();
}

void AS47PlayerCharacter::ServerActionBis_Implementation()
{
	if (nullptr != AttackComponent)
	{
		AttackComponent->AttackBis();
	}
}

bool AS47PlayerCharacter::ServerActionBis_Validate()
{
	return true;
}

void AS47PlayerCharacter::StopActionBis()
{
	AttackComponent->StopAttackBis();
}

void AS47PlayerCharacter::ServerStopActionBis_Implementation()
{
	if (nullptr != AttackComponent)
	{
		AttackComponent->StopAttackBis();
	}
}

bool AS47PlayerCharacter::ServerStopActionBis_Validate()
{
	return true;
}


void AS47PlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AS47PlayerCharacter, AttackComponent);
	DOREPLIFETIME(AS47PlayerCharacter, BoostComponent);
	DOREPLIFETIME(AS47PlayerCharacter, moveComponent);
	DOREPLIFETIME(AS47PlayerCharacter, playerFuelMultiplier);
	DOREPLIFETIME(AS47PlayerCharacter, playerFuelTick);
	DOREPLIFETIME(AS47PlayerCharacter, playerFuelTickMax);
	DOREPLIFETIME(AS47PlayerCharacter, playerFuelPerTick);
	DOREPLIFETIME(AS47PlayerCharacter, isRunningForward);
	DOREPLIFETIME(AS47PlayerCharacter, isRunningRight);
}

void AS47PlayerCharacter::GetDamage(float _amount, AActor* Dealer)
{
	Super::GetDamage(_amount, Dealer);

	if (!CanBeDamaged())
	{
		return;
	}

	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, *FString::Printf(TEXT("%f"), GetHealth()));
	float absorbedDmg = _amount * armorAbsorptionPercentage;

	AS47PlayerState* state = GetPlayerState<AS47PlayerState>();

	if (state != nullptr)
	{
		state->damageTaken += _amount;
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, *FString::Printf(TEXT("Damage Taken: %f"), state->damageTaken));
		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, ("receiveddamage"));

		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, *FString::Printf(TEXT("Received Damage from %s"), *GetNameSafe(Dealer)));
		//Armor Damage Mitigation
		if (state->CurrentArmor >= absorbedDmg)
		{
			state->CurrentArmor -= absorbedDmg;
			SetHealth(GetHealth() - (_amount - absorbedDmg));
		}
		else if (state->CurrentArmor < absorbedDmg)
		{
			float realDmg = (_amount - absorbedDmg) + (absorbedDmg - state->CurrentArmor);
			state->CurrentArmor = 0.0f;
			SetHealth(GetHealth() - realDmg);
		}
		else if (state->CurrentArmor == 0.0f)
		{
			SetHealth(GetHealth() - _amount);
		}
		BeingHitBP();
	}
	else
	{
		UE_LOG(LogS47Character, Error, TEXT("state is nullptr"));
	}
}

float AS47PlayerCharacter::GetCurrentArmor()
{
	AS47PlayerState* state = GetPlayerState<AS47PlayerState>();

	if (nullptr != state)
	{
		return state->CurrentArmor;
	}

	return -1.0f;
}

float AS47PlayerCharacter::GetMaxArmor()
{
	AS47PlayerState* state = GetPlayerState<AS47PlayerState>();

	if (nullptr != state)
	{
		return state->MaxArmor;
	}

	return -1.0f;
}


void AS47PlayerCharacter::SetCurrentArmor(float _armor)
{
	AS47PlayerState* state = GetPlayerState<AS47PlayerState>();

	if (nullptr != state)
	{
		state->CurrentArmor = _armor;
	}
	else
	{
		UE_LOG(LogS47Character, Error, TEXT("state is nullptr"));
	}
}

float AS47PlayerCharacter::GetCurrentFuel()
{
	AS47PlayerState* state = GetPlayerState<AS47PlayerState>();

	if (nullptr != state)
	{
		return state->CurrentFuel;
	}

	return -1.0f;
}

float AS47PlayerCharacter::GetMaxFuel()
{
	AS47PlayerState* state = GetPlayerState<AS47PlayerState>();

	if (nullptr != state)
	{
		return state->MaxFuel;
	}

	return -1.0f;
}

void AS47PlayerCharacter::SetCurrentFuel(float _fuel)
{
	AS47PlayerState* state = GetPlayerState<AS47PlayerState>();

	if (nullptr != state)
	{
		state->CurrentFuel = _fuel;
	}
	else
	{
		UE_LOG(LogS47Character, Error, TEXT("state is nullptr"));
	}
}

void AS47PlayerCharacter::EnterSpectatorMode(AController* Controller, AActor* Killer)
{
	AS47PlayerState* state = GetPlayerState<AS47PlayerState>();
	APlayerController* playerController = Cast<APlayerController>(Controller);
	
	if (nullptr != state && nullptr != playerController && playerController->Role == ROLE_Authority)
	{
		if (!state->IsAlive() && state->isDead == false)
		{
			AS47GameState* gameState = Cast<AS47GameState>(GetWorld()->GetGameState());
			if (nullptr != gameState)
			{
				gameState->TeleporteActorTo(playerController->GetPawn(), FVector(-285.0f, 4800.0f, -20000.0f), FRotator::ZeroRotator);
			}

			playerController->SetControlRotation(FRotator::ZeroRotator);

			state->bIsSpectator = true;
			state->bOnlySpectator = true;
			playerController->ClientGotoState(NAME_Spectating);
			playerController->ChangeState(NAME_Spectating);

			if (Role == ROLE_Authority)
			{
				state->ModifyIsDead(true);
			}
			else
			{
				state->ModifyIsDeadRPC(true);
			}
		}
	}
}

void AS47PlayerCharacter::TeleportToSpawn()
{
	//GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Yellow, *FString::Printf(TEXT("Teleport Player Function : %s"), *GetNameSafe(this)));
	AS47GameState* GameState = GetWorld()->GetGameState<AS47GameState>();

	if (nullptr != GameState)
	{
		int ID = -1;
		for (int i = 0; i < GameState->PlayerArray.Num(); i++)
		{
			if (GameState->PlayerArray[i] == GetPlayerState())
			{
				ID = i;
				break;
			}
		}

		//GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Yellow, *FString::Printf(TEXT("Player ID : %d"), ID));
		if (nullptr != GetController())// && GetController()->HasAuthority())
		{
			//GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Yellow, TEXT("Player Controller Found"));
			AS47LevelStart* LevelStart = AS47LevelStart::GetFirstLevelStart(GetWorld());
			if (nullptr != LevelStart)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Green, *FString::Printf(TEXT("LevelStart found : %s"), *GetNameSafe(LevelStart)));
				TeleportTo(LevelStart->GetActorLocation() + FVector(0, 200 * ID, 0), FRotator::ZeroRotator);
				GetController()->SetControlRotation(LevelStart->GetActorRotation());
			}
			else
			{
				//GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Red, TEXT("No LevelStart..."));
				UE_LOG(LogS47Character, Error, TEXT("LevelStart is NULLPTR"));
			}
		}
		else
		{
			//GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Red, TEXT("No Player Controller..."));
			UE_LOG(LogS47Character, Error, TEXT("No Controller on the Player"));
		}

		if (GetHealth() / GetMaxHealth() < 0.25f)
		{
			SetHealth(GetMaxHealth() * 0.25f);
		}
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Red, TEXT("No GameState found..."));
		UE_LOG(LogS47Character, Error, TEXT("GameState is NULLPTR"));
	}
}

void AS47PlayerCharacter::SetMouseSensitivity(float _value)
{
	if (nullptr != playerController && nullptr != playerController->PlayerInput)
	{
		playerController->PlayerInput->SetMouseSensitivity(_value);
	}
}

void AS47PlayerCharacter::OnJumpStart_ToServ_Implementation()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Serv Jump Start"));

	OnJumpStart_BP();
}

bool AS47PlayerCharacter::OnJumpStart_ToServ_Validate()
{
	return true;
}

void AS47PlayerCharacter::OnJumpStart_ToClient_Implementation()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Client Jump Start"));

	OnJumpStart_BP();
}

void AS47PlayerCharacter::Kill_Implementation()
{
	SetHealth(0);
	SetCurrentArmor(0);
}

bool AS47PlayerCharacter::Kill_Validate()
{
	return true;
#if WITH_EDITOR
#else
	return false;
#endif
}

void AS47PlayerCharacter::RefillLife_Implementation()
{
	SetHealth(GetMaxHealth());
	SetCurrentArmor(GetMaxArmor());
}

bool AS47PlayerCharacter::RefillLife_Validate()
{
	return true;
#if WITH_EDITOR
#else
	return false;
#endif
}