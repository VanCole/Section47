// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "S47GameMode.h"
#include "UI/S47HUD.h"
#include "Player/S47PlayerCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Player/S47PlayerState.h"
#include "TimerManager.h"
#include "S47GameState.h"
#include "S47Types.h"
#include "Section47.h"
#include "Kismet/GameplayStatics.h"
#include "Engine.h"
#include "Map/S47MapGenerator.h"
#include "Player/S47PlayerState.h"
#include "../Plugins/FMODStudio/Source/FMODStudio/Public/FMODStudioModule.h"
#include "Enemy/Boss/S47BossDistance.h"

AS47GameMode::AS47GameMode()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;

	(void)IFMODStudioModule::Get();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Characters/Player/PlayerCharacter_BP"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	static ConstructorHelpers::FClassFinder<APawn> SpectatorPawnClassFinder(TEXT("/Game/Characters/S47SpectatorPawn_BP"));
	SpectatorClass = SpectatorPawnClassFinder.Class;

	PlayerStateClass = AS47PlayerState::StaticClass();

	GameStateClass = AS47GameState::StaticClass();

	// use our custom HUD class
	HUDClass = AS47HUD::StaticClass();
}

void AS47GameMode::BeginPlay()
{
	Super::BeginPlay();

	GameState = GetWorld()->GetGameState<AS47GameState>();
	if (GameState != nullptr)
	{
		GameState->OnPreGeneration.AddUniqueDynamic(this, &AS47GameMode::PlayerRespawn);
		GameState->OnPostGeneration.AddUniqueDynamic(this, &AS47GameMode::OnPostGeneration);
	}
}

void AS47GameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<AActor*> AllChar;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AS47BossDistance::StaticClass(), AllChar);

	for (auto Char : AllChar)
	{
		AS47BossDistance* boss = Cast< AS47BossDistance>(Char);
		if (nullptr != boss)
		{
			GameState = GetWorld()->GetGameState<AS47GameState>();
			if (GameState != nullptr)
			{
				GameState->SetBossHealth(boss->GetHealth());
			}
		}
	}
}

void AS47GameMode::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// Add the new player in the lists of players
	Players.AddUnique(NewPlayer);
	PlayersAlive.AddUnique(NewPlayer);

	UE_LOG(LogS47GameMode, Log, TEXT("%s: Player join the game | Num Player: %d (Alive: %d / Dead: %d)"), GET_NETMODE_TEXT(), Players.Num(), PlayersAlive.Num(), PlayersDead.Num());

	// Add a callback in the death of the new player
	AS47Character* Character = Cast<AS47Character>(NewPlayer->GetCharacter());
	if (Character != nullptr)
	{
		Character->onEndDeathAnimation.AddUniqueDynamic(this, &AS47GameMode::PlayerDead);
	}

	// Update values in the GameState
	GameState = GetWorld()->GetGameState<AS47GameState>();
	if (GameState != nullptr)
	{
		GameState->NumPlayer = Players.Num();
		GameState->NumPlayerAlive = PlayersAlive.Num();
		//GameState->OnPostGeneration.AddUniqueDynamic(this, &AS47GameMode::TeleportAllPlayers);
	}
}

void AS47GameMode::Logout(AController * Exiting)
{
	Super::Logout(Exiting);
	UE_LOG(LogS47GameMode, Log, TEXT("%s: Player quit the game | Num Player: %d (Alive: %d / Dead: %d)"), GET_NETMODE_TEXT(), Players.Num(), PlayersAlive.Num(), PlayersDead.Num());

	// Check if it's a player controller
	APlayerController* ExitingPlayer = Cast<APlayerController>(Exiting);
	if (nullptr != ExitingPlayer)
	{
		// Remove the player in the lists of players
		Players.Remove(ExitingPlayer);
		PlayersAlive.Remove(ExitingPlayer);
		PlayersDead.Remove(ExitingPlayer);

		// Update GameState values
		GameState = GetWorld()->GetGameState<AS47GameState>();
		if (GameState != nullptr)
		{
			GameState->NumPlayer = Players.Num();
			GameState->NumPlayerAlive = PlayersAlive.Num();
			GameState->NumPlayerDead = PlayersDead.Num();
		}
	}

	CheckGameOver();
}

void AS47GameMode::OnPostGeneration()
{
	TimeStartLevel = GetWorld()->GetTimeSeconds();
}
void AS47GameMode::PlayerRespawn()
{
	//GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Orange, (TEXT("EnterRespawn")));

	for (APlayerController* playerController : PlayersDead)
	{
		// Switch the player from alive list to dead list
		PlayersAlive.AddUnique(playerController);

		if (PlayersDead.Contains(playerController))
		{
			PlayersDead.Remove(playerController);
		}

		UE_LOG(LogS47GameMode, Log, TEXT("%s: Player is dead | Num Player: %d (Alive: %d / Dead: %d)"), GET_NETMODE_TEXT(), Players.Num(), PlayersAlive.Num(), PlayersDead.Num());

		// Update GameState values
		GameState = GetWorld()->GetGameState<AS47GameState>();
		if (GameState != nullptr)
		{
			GameState->NumPlayerAlive = PlayersAlive.Num();
			GameState->NumPlayerDead = PlayersDead.Num();

			GameState->RespawnPlayer(playerController);
		}
	}
}

void AS47GameMode::PlayerDead(AController* Controller, AActor* Killer)
{
	// Switch the player from alive list to dead list
	APlayerController* Player = Cast<APlayerController>(Controller);
	if (nullptr != Player)
	{
		PlayersAlive.Remove(Player);
		PlayersDead.AddUnique(Player);

		UE_LOG(LogS47GameMode, Log, TEXT("%s: Player is dead | Num Player: %d (Alive: %d / Dead: %d)"), GET_NETMODE_TEXT(), Players.Num(), PlayersAlive.Num(), PlayersDead.Num());

		// Update GameState values
		GameState = GetWorld()->GetGameState<AS47GameState>();
		if (GameState != nullptr)
		{
			GameState->NumPlayerAlive = PlayersAlive.Num();
			GameState->NumPlayerDead = PlayersDead.Num();
		}

		CheckGameOver();
	}
}

void AS47GameMode::CheckGameOver()
{
	if (nullptr != GameState)
	{
		if (!GameState->GameOver && Players.Num() > 0 && PlayersDead.Num() >= Players.Num())
		{
			UE_LOG(LogS47GameMode, Log, TEXT("%s: Game Over!"), GET_NETMODE_TEXT());
			GameState->InvokeGameOver();
		}
	}
	else
	{
		UE_LOG(LogS47GameMode, Error, TEXT("%s: No GameState to trigger GameOver"), GET_NETMODE_TEXT());
	}
}

void AS47GameMode::SetSeed(uint32 seed)
{
	UE_LOG(LogS47GameMode, Log, TEXT("%s: Seed= %d"), GET_NETMODE_TEXT(), seed);
	GameState->SetSeed(seed);
}

uint32 AS47GameMode::RandomSeed()
{
	FRandomStream rand;
	rand.GenerateNewSeed();
	SetSeed(rand.GetCurrentSeed());
	return rand.GetCurrentSeed();
}

void AS47GameMode::EndLevel()
{
	if(nullptr != GameState)
	{
		GameState->TimeLevel = GetWorld()->GetTimeSeconds() - TimeStartLevel;
		UE_LOG(LogS47GameMode, Log, TEXT("%s: End Level in %f!"), GET_NETMODE_TEXT(), GameState->TimeLevel);
		GameState->InvokeEndLevel();
	}
	else
	{
		UE_LOG(LogS47GameMode, Error, TEXT("%s: No GameState to trigger EndLevel"), GET_NETMODE_TEXT());
	}
}

void AS47GameMode::GenerateMap()
{
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AS47MapGenerator::StaticClass(), actors);
	for (AActor* actor : actors)
	{
		AS47MapGenerator* map = Cast<AS47MapGenerator>(actor);
		if(map != nullptr)
		{
			map->Generate();
		}
	}
}

/*void AS47GameMode::Respawn(APlayerController* _controller)
{
}*/

//void AS47GameMode::TeleportAllPlayers()
//{
//	int i = 0;
//	for (APlayerController* Controller : Players)
//	{
//		AS47Character* Character = Cast<AS47Character>(Controller->GetCharacter());
//		if (Character != nullptr)
//		{
//			Character->TeleportTo(FVector(0, i * 200, 100), FRotator::ZeroRotator);
//			if (nullptr != Character->GetController())
//			{
//				Character->GetController()->SetControlRotation(FRotator::ZeroRotator);
//			}
//			i++;
//		}
//	}
//}