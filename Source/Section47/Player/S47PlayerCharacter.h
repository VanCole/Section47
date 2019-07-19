// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "S47Character.h"
#include "Components/BoxComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/S47CameraComponent.h"
#include "Blueprint/WidgetTree.h"
#include "Player/S47PlayerState.h"
#include "S47PlayerCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FS47CharacterEvent);

class UInputComponent;

UCLASS(config = Game)
class AS47PlayerCharacter : public AS47Character
{
	GENERATED_BODY()



		/** Gun mesh: 1st person view (seen only by self) */
		UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USceneComponent* FP_MuzzleLocation;

	/** Gun mesh: VR view (attached to the VR controller directly, no arm, just the actual gun) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USkeletalMeshComponent* VR_Gun;

	/** Location on VR gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class USceneComponent* VR_MuzzleLocation;

	/** Motion controller (right hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UMotionControllerComponent* R_MotionController;

	/** Motion controller (left hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UMotionControllerComponent* L_MotionController;

public:

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
		class USkeletalMeshComponent* Mesh1P;

	AS47PlayerCharacter(const FObjectInitializer& ObjectInitializer);


protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class US47MovementComponent* moveComponent;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditAnywhere, Category = Projectile)
		TSubclassOf<class AS47Projectile> ProjectileClass;

	/** Impact class to spawn */
	UPROPERTY(EditAnywhere, Category = Impact)
		TSubclassOf<class AS47Impact> ImpactClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAnimMontage* FireAnimation;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		uint32 bUsingMotionControllers : 1;


	/** Player Life **/
	UFUNCTION(BlueprintPure, Category = "Player Life")
		float GetPlayerLifePercentage();
	UFUNCTION(BlueprintPure, Category = "Player Life")
		FText GetPlayerLifeIntText();
	/*UFUNCTION(BlueprintCallable, Category = "Player Life")
		void UpdatePlayerLife();*/

		/** Player Armor **/
	UFUNCTION(BlueprintPure, Category = "Armor")
		float GetPlayerArmorPercentage();
	UFUNCTION(BlueprintPure, Category = "Armor")
		FText GetPlayerArmorIntText();
	/*UFUNCTION(BlueprintCallable, Category = "Player Life")
		void UpdatePlayerArmor();*/

		/** Player Fuel **/
	UFUNCTION(BlueprintPure, Category = "Fuel")
		float GetPlayerFuelPercentage();
	UFUNCTION(BlueprintPure, Category = "Fuel")
		FText GetPlayerFuelIntText();

	UFUNCTION()
		void UpdateFuel(float DeltaTime);
	UFUNCTION()
		void UpdateFuelParam(float fuelChange);

	/*UFUNCTION()
		void SetFuelMultiplier(float multiplierValue);*/
		/*UFUNCTION()
			void DelegateSetFuelMultiplier(float multiplierValue, float delay);*/

			/*UFUNCTION()
				void SetFuelMultiplier(float multiplierValue);*/
				/*UFUNCTION()
					void DelegateSetFuelMultiplier(float multiplierValue, float delay);*/


					/** Set Damage State */


				/** Set Damage State */
	UFUNCTION()
		void SetFuelMultiplier(float multiplierValue);
	UFUNCTION()
		void DelegateSetFuelMultiplier(float multiplierValue, float delay);
	FTimerHandle FuelTimerHandle;
	class US47AttackComponent* GetAttackComponent() { return AttackComponent; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintAssignable)
	FS47CharacterEvent onShowInGameMenu;



	float GetCurrentArmor();
	float GetMaxArmor();
	void SetCurrentArmor(float _armor);

	float GetCurrentFuel();
	float GetMaxFuel();
	void SetCurrentFuel(float _fuel);

	UFUNCTION()
	void EnterSpectatorMode(AController* _controller, AActor* Killer);

protected:

	/** Fires a projectile. */
	//void OnFire();

	/** Fires a raycast. */
	//void OnFireRaycast();

	void Dash();
	void Jump() override;
	void StopJumping() override;

	void StartHovering();
	void StopHovering();

	/** Resets HMD orientation and position in VR. */
	void OnResetVR();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false; Location = FVector::ZeroVector; }
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	//void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;

	UFUNCTION(BlueprintCallable)
	void ShowHideInGameMenu();

	UFUNCTION()
		void EnableEndGame();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/*
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:
	UFUNCTION(BlueprintImplementableEvent)
		void OnFrontalLightBP();

	UFUNCTION(BlueprintImplementableEvent)
		void OnKick();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FrontalLight")
		float FrontalLight_Intensity;

	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE UCameraComponent* GetFirstPersonCameraComponent() const { return Cast<UCameraComponent>(FirstPersonCameraComponent); }

	// Life
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats/Player Life")
		float playerLife;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats/Player Life")
		float playerLifeMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats/Player Life")
		float playerLifePercentage;

	// Armor

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats/Player Armor")
		float playerArmor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats/Player Armor")
		float playerArmorMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats/Player Armor")
		float armorAbsorptionPercentage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats/Player Armor")
		float playerArmorPercentageText;

	//FUEL MANAGEMENT
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Player Stats/Player Fuel")
		float playerFuelMultiplier; //-1: decrease fuel / 1 : increase / 0: no modif
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Player Stats/Player Fuel")
		float playerFuelTick;	//Delay between each fuel update
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Player Stats/Player Fuel")
		float playerFuelTickMax;	//Base value for playerFuelTick
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Player Stats/Player Fuel")
		float playerFuelPerTick;  //fuel gained each tick

	UPROPERTY()
		float playerFuelPercentage;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
		float isRunningForward;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
		float isRunningRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats/Player Fuel")
		float isStartingRefill; //delay before fuel start refilling

		//FTimerHandle FuelTimerHandle;

		//bool bCanUseFuel;

	bool isInGameMenu;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class APlayerController* playerController;

	UPROPERTY(Replicated, Category = S47Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class US47AttackComponent* AttackComponent;

	UPROPERTY(Replicated, Category = S47Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class US47BoostComponent* BoostComponent;



	UFUNCTION(BlueprintImplementableEvent)
		void OnJumpStart_BP();
	UFUNCTION(BlueprintImplementableEvent)
		void OnJumpReception_BP();


	UFUNCTION(Server, Unreliable, WithValidation)
		void OnJumpStart_ToServ();
	UFUNCTION(NetMulticast, Unreliable)
		void OnJumpStart_ToClient();


	//BLUEPRINT CALL SOUND
	UFUNCTION(BlueprintImplementableEvent)
		void DoDashBP();
	UFUNCTION(BlueprintImplementableEvent)
		void IsHoveringBP();
	UFUNCTION(BlueprintImplementableEvent)
		void StartHoveringBP();
	UFUNCTION(BlueprintImplementableEvent)
		void StopHoveringBP();
	UFUNCTION(BlueprintImplementableEvent)
		void BeingHitBP();
	UFUNCTION(BlueprintImplementableEvent)
		void ChangeWeaponBP();
	UFUNCTION(BlueprintImplementableEvent)
		void OnAttack();

	// Network
	void Action();
	UFUNCTION(server, WithValidation, reliable)
		void ServerAttack();
	void ServerAttack_Implementation();
	bool ServerAttack_Validate();

	void StopAction();
	UFUNCTION(server, WithValidation, reliable)
		void ServerStopAction();
	void ServerStopAction_Implementation();
	bool ServerStopAction_Validate();

	void ThrowWeaponAction();
	UFUNCTION(server, WithValidation, reliable)
		void ServerThrowWeaponAction();
	void ServerThrowWeaponAction_Implementation();
	bool ServerThrowWeaponAction_Validate();

	void ActionBis();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerActionBis();
	void ServerActionBis_Implementation();
	bool ServerActionBis_Validate();

	void StopActionBis();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerStopActionBis();
	void ServerStopActionBis_Implementation();
	bool ServerStopActionBis_Validate();

	UPROPERTY(/*Replicated, */VisibleAnywhere, Category = S47Character)
		bool hasDoubleDamageActive;

public: // Interface IS47Damageable
	class AS47PlayerState* PlayerState;
	virtual float GetMaxHealth() override;
	virtual float GetHealth() override;
	virtual void SetHealth(float _health) override;
	virtual	void GetDamage(float _amount, AActor* Dealer) override;

	UFUNCTION()
	void TeleportToSpawn();

	UFUNCTION(BlueprintCallable)
	void SetMouseSensitivity(float _value);

protected:
	// debug function
	UFUNCTION(Server, Reliable, WithValidation)
	void Kill();

	UFUNCTION(Server, Reliable, WithValidation)
	void RefillLife();
};

