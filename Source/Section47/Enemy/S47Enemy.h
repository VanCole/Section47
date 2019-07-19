// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/S47PlayerCharacter.h"
#include "Spawner/S47SpawnableInterface.h"
#include "AI/S47AICharacter.h"
#include "AI/S47AIController.h"
#include "S47Enemy.generated.h"

/**
 *
 */

UCLASS(Config = Game)
class SECTION47_API AS47Enemy : public AS47AICharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float m_rangeVisionForward; //Range to see

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float m_rangeVisionBack; //Range to see

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float m_rangeAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		int m_lifeMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float m_speed; //speed Max

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		int32 m_dmg; //Dommage can deal

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float timeBeetweenAttack; //AttackSpeed

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float offsetAnimAttack; //AttackSpeed

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		bool isRanged; //Range or not

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float speedPatrouille;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float speedMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float timeBeforeForgotenPlayer = 5;

	/*UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		class UStaticMeshComponent* Mesh1P;*/

protected:
	float m_timeKicked = 0; // actual time kicked if 0 can move
	float m_timeStun = 0; // actual time stuned if 0 can move
	const static int m_distanceEnemyMax = 100; //Max distance can live
	
	//TArray<AS47PlayerCharacter*> m_players; //Faire une requete au serveur pour récupérer les joueurs
	FColor m_color; //Actual color
	float reloadTime;
	bool IsAttacking = false;
	float TimeSinceLastAttack = 0;
	virtual void BeginPlay() override;
	virtual void EnemyAttack() {};
	//virtual void NearPlayer();
	AS47AIController* MyController;



public:
	AS47PlayerCharacter* m_player; //Near player
	bool m_isKicked = false;
	bool m_isStun = false;
	float m_duringStun = 0;
	bool m_findPlayer = false;
	bool m_isDead = false;
	float m_life;
	float m_timeHit = 0;
	FVector m_nearDistance; //distance from player

	bool alwaysFindPlayer = false;

	bool alertedByEnvironnement = false;

	float timeForgotenPlayer = 5;

	int nbDoorEnter = 0;

	UPROPERTY(Category = S47Ragdoll, Replicated, VisibleAnywhere, BlueprintReadOnly)
	bool m_isAlive = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool canPlaySound = true;
private:
	bool m_hit = false;

	void EnemyFeedBack();

	void FindNearPlayer();

public:

	UFUNCTION(BlueprintImplementableEvent)
		void OnAttackBP();

	UFUNCTION(BlueprintCallable)
		void DoAttackBP();

	UFUNCTION(BlueprintImplementableEvent)
		void EndAttackBP();

	UFUNCTION(BlueprintImplementableEvent)
		void CrouchBP();

	UFUNCTION(BlueprintImplementableEvent)
		void UnCrouchBP();

	UFUNCTION(BlueprintCallable)
	void EndAttack();

	//UFUNCTION(BlueprintImplementableEvent)
		void OnStun();

	void InitAttack();

	virtual void Attack();

	virtual void Tick(float DeltaTime) override;

	virtual bool CanAttack();

	UFUNCTION(BlueprintCallable)
	void Kill();

	UFUNCTION(BlueprintCallable)
	void ForcedKill();

};
