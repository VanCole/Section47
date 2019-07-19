
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy/S47Boss.h"
#include "AI/S47AIController.h"
#include "Player/S47PlayerCharacter.h"
#include "S47BossDistance.generated.h"

/**
 *
 */
UCLASS()
class SECTION47_API AS47BossDistance : public AS47Boss
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float m_rangeBaseAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		int m_lifeMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float m_speed; //speed Max

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		int32 m_dmgBaseAttack; //Dommage can deal

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float timeBeetweenBaseAttack; //AttackSpeed

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float timeBeetweenPhase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float ProjectileDmg;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float JetAcideDmg;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		int JetAcidePhasePoids;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		int multiProjectilePhasePoids;

	/*UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class UStaticMeshComponent* Mesh1P;*/

protected:


	bool IsAttacking = false;
	float TimeSinceLastAttack = 0;

	virtual void BeginPlay() override;

	AS47AIController* MyController;

	float TimeSinceLastPhase = 0;

	float timeInPhase = 0;

	FQuat originQuat;
	FQuat destinationQuat;

	//MultipleProjectilePhase
	bool projectile[10] = { false };

	UPROPERTY(EditAnywhere, Category = S47EnemyRange)
		TSubclassOf<class AS47Projectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = S47EnemyRange, BlueprintReadWrite)
		TSubclassOf<class AS47Projectile> ProjectileClassBP;

	void SpawnProjectile(FVector _InitLocation, FVector _Direction);

	UFUNCTION(BlueprintCallable)
	void SpawnProjectileBP(FVector _InitLocation, FVector _Direction);

	//JetAcidePhase
	FQuat tempDestinationQuat;

	bool spawnJet = false;

	UFUNCTION(BlueprintImplementableEvent)
		void OnJetBP();

	UFUNCTION(BlueprintImplementableEvent)
		void OnJetEndBP();

	UFUNCTION(BlueprintImplementableEvent)
		void OnAnimationBP();

	TArray<USceneComponent*> JetCollidersRight;
	TArray<USceneComponent*> JetCollidersLeft;

	bool colliderRight = false; // direction to enable collider, left to right enable right .. right to left enable left

	float tickDmg;
	bool start = false;

public:
	AS47PlayerCharacter* m_player; //Near player

	bool isInPhase = false;
	bool isAlive = true;
	bool init = false;
	bool multiProjectilePhase = false;
	bool jetAcidePhase = false;
	// 0 Rien /1 Phase Multiprojectile /2 Phase DoubleLaser

	UFUNCTION(BlueprintImplementableEvent)
		void NewPhaseBP();

	UFUNCTION()
	void AnimationEnd(AController* _controller, AActor* _killer);

	UFUNCTION()
	void StartBoss();

	UFUNCTION(BlueprintImplementableEvent)
	void StartBossBP();

	UFUNCTION(BlueprintCallable)
	void EndStartBoss();
	UFUNCTION(BlueprintImplementableEvent)
	void EndStartBossBP();

	//int SetPhase();

	//Right From Boss Forward, Negative Yaw rotation to center
	USceneComponent* Bras1;
	//Left From Boss Forward, Positive Yaw rotation to center
	USceneComponent* Bras2;

	const USkeletalMeshSocket* fireRight;
	const USkeletalMeshSocket* fireLeft;

	UFUNCTION(BlueprintCallable)
		void OnFireRight();

	UFUNCTION(BlueprintCallable)
		void OnFireLeft();


	//void InitArmMovement(int _angleMovementMax, int _angleMovementMin = 0, bool _setRandomAngle = false, bool _focusPlayer = false, int _startExtremite = -1);
	//void MultiProjectilePhase(float DeltaTime);

	//void JetAcidePhase(float DeltaTime);

	virtual void Tick(float DeltaTime) override;

};
