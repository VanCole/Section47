// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "S47GameState.h"
#include "Player/S47PlayerCharacter.h"
//#include "S47"
#include "S47GameMode.generated.h"


UCLASS(minimalapi)
class AS47GameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AS47GameMode();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void SetSeed(uint32 seed);
	uint32 RandomSeed();

	void EndLevel();

	int32 NumPlayer() { return Players.Num(); }
	int32 NumPlayerAlive() { return PlayersAlive.Num(); }
	int32 NumPlayerDead() { return PlayersDead.Num(); }

	UFUNCTION(BlueprintCallable)
	void GenerateMap();

	//UFUNCTION(BlueprintCallable)
	//void Respawn(APlayerController* _controller);

	UFUNCTION(BlueprintCallable)
	void PlayerRespawn(); // player respawn

private:
	AS47GameState* GameState;
	float TimeStartLevel;

	virtual void PostLogin(APlayerController * NewPlayer) override; // player join the game
	virtual void Logout(AController * Exiting) override; // player quit the game

	UFUNCTION()
	void OnPostGeneration();

	UFUNCTION()
	void PlayerDead(AController* Controller, AActor* Killer);

	void CheckGameOver();

	// Track players life
	TArray<APlayerController*> Players;
	TArray<APlayerController*> PlayersAlive;
	TArray<APlayerController*> PlayersDead;

	/*UFUNCTION()
	void TeleportAllPlayers();*/
};



