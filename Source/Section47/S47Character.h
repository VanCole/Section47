// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "S47DamageableInterface.h"
#include "S47Character.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FS47DeathEvent, AController*, Controller, AActor*, Killer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FS47HealthEvent, float, Amount, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FS47RespawnEvent, APlayerController*, Player);



UCLASS()
class SECTION47_API AS47Character : public ACharacter, public IGenericTeamAgentInterface, public IS47DamageableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AS47Character();
	AS47Character(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/*UFUNCTION(BlueprintCallable, Category = S47Character)
	void Attack();*/

	/* First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class US47CameraComponent* FirstPersonCameraComponent;

public:
	virtual FGenericTeamId GetGenericTeamId() const override { return S47TeamID; }
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID)override;

	//virtual void GetDamage(float _amountDamage);

	virtual bool IsValidActorForCollision(AActor* _OtherActor);

	//virtual void Heal(float _amount);

	UPROPERTY(BlueprintAssignable, Category = "Character")
	FS47DeathEvent onDeathEvent; // Death event
	UPROPERTY(BlueprintAssignable, Category = "Character")
	FS47HealthEvent onHealEvent;
	UPROPERTY(BlueprintAssignable, Category = "Character")
	FS47HealthEvent onGetDamageEvent;
	UPROPERTY(BlueprintAssignable, Category = "Character")
	FS47HealthEvent onHealthChangeEvent;
	UPROPERTY(BlueprintAssignable, Category = "Character")
	FS47RespawnEvent onRespawnEvent;
	/*UPROPERTY(BlueprintAssignable, Category = "Character")
	FS47SimpleEvent onStartDeathAnimation;*/
	UPROPERTY(BlueprintAssignable, Category = "Character")
	FS47DeathEvent onEndDeathAnimation;

protected:
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void InvokeDeath();

	UPROPERTY(Category = TeamID, EditAnywhere, BlueprintReadWrite)
	FGenericTeamId S47TeamID;

public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnHealthChange(float Amount, float NewHealth);

	UFUNCTION(BlueprintImplementableEvent)
	void OnGetDamage(float Amount, float NewHealth);

	UFUNCTION(BlueprintImplementableEvent)
	void OnHeal(float Amount, float NewHealth);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnHitBP(FVector _posHit, bool byLazer = false, FLinearColor ColorFX = FLinearColor::Black);

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="On Death"))
	void OnDeathBP(const AController* _controller);
	

	// Death animation
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void DeathAnimationEnd();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "End Death Animation"))
	void OnEndDeathAnimationBP(const AController* _controller);


	// Interface IS47Damageable
	UFUNCTION(BlueprintCallable)
	bool CanBeDamaged();

	UFUNCTION(BlueprintCallable)
	virtual float GetMaxHealth() { return -1.0f; }

	UFUNCTION(BlueprintCallable)
	virtual float GetHealth() { return -1.0f; }

	UFUNCTION(BlueprintCallable)
	virtual void SetHealth(float _health) {}

	UFUNCTION(BlueprintCallable)
	virtual void GetDamage(float _amount, AActor* Dealer);

	UFUNCTION(NetMulticast, Reliable)
	void SetLastDamageDealer(AActor* Dealer);

public:

	bool canBeDamaged;

	AActor* LastDamageDealer;

	bool isAlreadyDead;
	bool hasAlreadyEndDeath;
};
