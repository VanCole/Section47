// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "UnrealNetwork.h"
#include "S47PlayerState.generated.h"

/**
 * 
 */
UCLASS()
class SECTION47_API AS47PlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	static const float StartHealth;
	static const float StartArmor;
	static const float StartFuel;

public:
	AS47PlayerState();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	virtual void SetPlayerName(const FString& S) override;
	UFUNCTION(BlueprintCallable, Server, Reliable, WithValidation)
	void SetPlayerNameServer(const FString& S);

	UFUNCTION()
	void OnRep_CurrentHealth();
	UFUNCTION()
	void OnRep_LevelLife();

	bool IsAlive() { return CurrentHealth > 0; }

	UFUNCTION(BlueprintCallable)
	void ModifyIsReady(bool ready);
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void ModifyIsReadyRPC(bool ready);

	UFUNCTION(BlueprintCallable)
	void ModifyIsDead(bool isDead);
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void ModifyIsDeadRPC(bool isDead);

	/*UFUNCTION(BlueprintCallable)
	void ModifyPlayerName(FString playerName);
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void ModifyPlayerNameRPC(FString playerName);*/

	UFUNCTION(BlueprintCallable)
	void ResetStats();
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void ResetStatsRPC();

	UFUNCTION(BlueprintCallable)
	void ApplicateModificationLevelPoint();
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable)
	void ApplicateModificationLevelPointServer();

	UFUNCTION(BlueprintCallable)
	void ResetPlayerGameStats();

public :

	// Player Stats
	UPROPERTY(Replicated, BlueprintReadWrite)
	TArray<class AS47Enemy*> DetectedEnemy;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, BlueprintReadWrite)
	float CurrentHealth;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Life")
	float MaxHealth;

	UPROPERTY(Replicated, BlueprintReadWrite)
	float CurrentArmor;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Armor")
	float MaxArmor;

	UPROPERTY(Replicated, BlueprintReadWrite)
	float CurrentFuel;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Fuel")
	float MaxFuel;

	// Level Point
	UPROPERTY(Replicated/*Using = OnRep_LevelLife*/, EditAnywhere, BlueprintReadWrite, Category = "Point")
	int levelLife;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Point")
	int levelDamage;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Point")
	int levelAmmo;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Point")
	int levelArmor;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Point")
	int levelFuel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Point")
	int remainingPoint;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Point")
	float levelTime;
	UPROPERTY(ReplicatedUsing = OnRep_EnemyKilled, EditAnywhere, BlueprintReadWrite, Category = "Point")
	int killMonsters;
	UPROPERTY(ReplicatedUsing = OnRep_DamageDealed, EditAnywhere, BlueprintReadWrite, Category = "Point")
	float damageDealed;
	UPROPERTY(ReplicatedUsing = OnRep_DamageTaken, EditAnywhere, BlueprintReadWrite, Category = "Point")
	float damageTaken;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Point")
	int levelPoint;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Point")
	int totalLevelPoint;
	UPROPERTY(ReplicatedUsing = OnRep_NbWeaponTaken, EditAnywhere, BlueprintReadWrite, Category = "Point")
	int nbWeaponTaken;
	UPROPERTY(ReplicatedUsing = OnRep_NbWeaponThrown, EditAnywhere, BlueprintReadWrite, Category = "Point")
	int nbWeaponThrown;

	// Player is ready
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Ready")
	bool isReady;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Dead")
	bool isDead;

	float previousHealth;
	int previousLevelLife;
	int previousLevelDamage;
	int previousLevelAmmo;
	int previousLevelArmor;
	int previousLevelFuel;

	UPROPERTY(BlueprintReadWrite)
	float mouseSensibility;

	UPROPERTY(BlueprintReadWrite, Replicated)
	FString PlayerNickname;

	UFUNCTION()
	void OnRep_DamageDealed();
	UFUNCTION()
	void OnRep_DamageTaken();
	UFUNCTION()
	void OnRep_EnemyKilled();
	UFUNCTION()
	void OnRep_NbWeaponTaken();
	UFUNCTION()
	void OnRep_NbWeaponThrown();

	UFUNCTION(BlueprintCallable)
	void CalculPoints();
};
