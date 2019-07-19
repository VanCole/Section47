// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "S47GameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSimpleEvent);
/**
 * 
 */
UCLASS()
class SECTION47_API AS47GameState : public AGameStateBase
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintAssignable, Category = "S47")
	FSimpleEvent OnEndLevel;
	UPROPERTY(BlueprintAssignable, Category = "S47")
	FSimpleEvent OnGameOver;

	UPROPERTY(BlueprintAssignable, Category = "S47")
	FSimpleEvent OnSetSeed;
	UPROPERTY(BlueprintAssignable, Category = "S47")
	FSimpleEvent OnPreGeneration;
	UPROPERTY(BlueprintAssignable, Category = "S47")
	FSimpleEvent OnPostGeneration;

	// Boss or Survival rooms events
	UPROPERTY(BlueprintAssignable, Category = "S47")
	FSimpleEvent OnBossUnlock;
	UPROPERTY(BlueprintAssignable, Category = "S47")
	FSimpleEvent OnBossStart;
	UPROPERTY(BlueprintAssignable, Category = "S47")
	FSimpleEvent OnBossEnd;
	UPROPERTY(BlueprintAssignable, Category = "S47")
	FSimpleEvent OnEndStartBoss;
	

	

	UPROPERTY(ReplicatedUsing = OnRep_NumPlayer, BlueprintReadWrite)
	int32 NumPlayer;

	UPROPERTY(ReplicatedUsing = OnRep_NumPlayer, BlueprintReadWrite)
	int32 NumPlayerAlive;

	UPROPERTY(ReplicatedUsing = OnRep_NumPlayer)
	int32 NumPlayerDead;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Seed;

	bool GameOver;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BattleIntensity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NormalIntensity;

	/** Total time estimated to clear the level (based on individual reference time in rooms) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float TimeReference;

	/** Total time taken by players to reach the end room */
	UPROPERTY(ReplicatedUsing = OnRep_TimeLevel, VisibleAnywhere, BlueprintReadOnly)
	float TimeLevel;

	/** Total score for the current run */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int TotalScore;

	UPROPERTY(BlueprintReadWrite, Replicated)
	float bossHealth;

public:
	
	UFUNCTION()
	void SetBossHealth(float _bossHealth);
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void SetBossHealthRPC(float _bossHealth);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, WithValidation)
	void InvokeEndLevel();
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, WithValidation)
	void InvokeGameOver();

	// Boss lifecycle
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, WithValidation)
	void InvokeBossUnlock();// unlock boss room when all players are in SAS
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, WithValidation)
	void InvokeBossStart(); // before the boss start animation
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, WithValidation)
	void InvokeEndStartBoss(); // after the boss start animation
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, WithValidation)
	void InvokeBossEnd(); // after the boss death animation
	

	UFUNCTION(NetMulticast, Reliable, WithValidation, BlueprintCallable)
	void TeleporteActorTo(APawn* _pawn, FVector _position, FRotator _rotation);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void SetSeed(uint32 NewSeed);

	UFUNCTION(BlueprintCallable)
	bool AllPlayerReady();

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void RespawnPlayer(APlayerController* playerController);

private:
	UFUNCTION()
	void OnRep_NumPlayer();
	UFUNCTION()
	void OnRep_TimeLevel();
};
