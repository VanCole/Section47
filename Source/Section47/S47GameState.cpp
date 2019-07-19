// Fill out your copyright notice in the Description page of Project Settings.

#include "S47GameState.h"
#include "S47Types.h"
#include "Player/S47PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "S47GameInstance.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Player/S47PlayerCharacter.h"
#include "Components/S47AttackComponent.h"
#include "Weapon/S47Weapon.h"
#include "S47GameMode.h"



void AS47GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AS47GameState, NumPlayer);
	DOREPLIFETIME(AS47GameState, NumPlayerAlive);
	DOREPLIFETIME(AS47GameState, NumPlayerDead);
	DOREPLIFETIME(AS47GameState, TimeLevel);
	DOREPLIFETIME(AS47GameState, bossHealth);
}

bool AS47GameState::AllPlayerReady()
{
	int32 nbPlayerReady = 0;

	for (int i = 0; i < PlayerArray.Num(); i++)
	{
		AS47PlayerState* player = Cast<AS47PlayerState>(PlayerArray[i]);
		
		if (nullptr != player)
		{
			if (player->isReady)
			{
				nbPlayerReady++;
			}
		}
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, *FString::Printf(TEXT("%s: player ready: %d, nb player: %d !"), GET_NETMODE_TEXT(), nbPlayerReady, NumPlayer));

	if (nbPlayerReady == NumPlayer)
	{
		return true;
	}

	return false;
}

void AS47GameState::RespawnPlayer_Implementation(APlayerController * playerController)
{
	if (nullptr != playerController)
	{
		if (nullptr != playerController->PlayerState)
		{
			AS47PlayerState* state = Cast<AS47PlayerState>(playerController->PlayerState);

			if (nullptr != state)
			{
				if (state->isDead)
				{
					GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Orange, *FString::Printf(TEXT("%s: Before Name Pawn: %f, %f, %f, %d"), GET_NETMODE_TEXT(), state->CurrentHealth, state->CurrentArmor, state->CurrentFuel, state->bIsSpectator));
					state->bIsSpectator = false;
					state->bOnlySpectator = false;

					if (Role == ROLE_Authority)
					{
						state->ModifyIsDead(false);
						state->ResetStats();
					}
					else
					{
						state->ModifyIsDeadRPC(false);
						state->ResetStatsRPC();
					}

					TArray< AActor*> AllChar;

					UGameplayStatics::GetAllActorsOfClass(GetWorld(), AS47PlayerCharacter::StaticClass(), AllChar);

					for (auto Char : AllChar)
					{
						AS47PlayerCharacter* PlayerChar = Cast< AS47PlayerCharacter>(Char);
						if (nullptr != PlayerChar)
						{
							if (nullptr == PlayerChar->Controller)
							{
								PlayerChar->SetActorLocation(FVector(-100.0f, 0.0f, 100.0f));
								playerController->Possess(PlayerChar);

								if (nullptr != PlayerChar->AttackComponent->WeaponOnPlayer[1])
								{
									PlayerChar->AttackComponent->WeaponOnPlayer[1]->Destroy();
									PlayerChar->AttackComponent->WeaponOnPlayer[1] = nullptr;
								}
								if (nullptr != PlayerChar->AttackComponent->WeaponOnPlayer[2])
								{
									PlayerChar->AttackComponent->WeaponOnPlayer[2]->Destroy();
									PlayerChar->AttackComponent->WeaponOnPlayer[2] = nullptr;
								}

								PlayerChar->AttackComponent->indexWeapon = 0;
								PlayerChar->AttackComponent->ChangeCurrentWeapon();
								PlayerChar->isAlreadyDead = false;
								PlayerChar->hasAlreadyEndDeath = false;
							}
						}
					}
				}
			}
		}
	}
}

bool AS47GameState::RespawnPlayer_Validate(APlayerController* playerController)
{
	return true;
}

void AS47GameState::InvokeEndLevel_Implementation()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, *FString::Printf(TEXT("%s: End Level !"), GET_NETMODE_TEXT()));
	OnEndLevel.Broadcast();
}

bool AS47GameState::InvokeEndLevel_Validate()
{
	return true;
}

void AS47GameState::InvokeGameOver_Implementation()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, *FString::Printf(TEXT("%s: Game Over !"), GET_NETMODE_TEXT()));
	GameOver = true;
	OnGameOver.Broadcast();
}

bool AS47GameState::InvokeGameOver_Validate()
{
	return true;
}


void AS47GameState::InvokeBossUnlock_Implementation()
{
	OnBossUnlock.Broadcast();
}

bool AS47GameState::InvokeBossUnlock_Validate()
{
	return true;
}

void AS47GameState::InvokeBossStart_Implementation()
{
	OnBossStart.Broadcast();
}

bool AS47GameState::InvokeBossStart_Validate()
{
	return true;
}

void AS47GameState::InvokeEndStartBoss_Implementation()
{
	OnEndStartBoss.Broadcast();
}

bool AS47GameState::InvokeEndStartBoss_Validate()
{
	return true;
}

void AS47GameState::InvokeBossEnd_Implementation()
{
	OnBossEnd.Broadcast();
}

bool AS47GameState::InvokeBossEnd_Validate()
{
	return true;
}


void AS47GameState::TeleporteActorTo_Implementation(APawn* _pawn, FVector _position, FRotator _rotation)
{
	if (nullptr != _pawn)
	{
		//_pawn->TeleportTo(_position, _rotation);
		_pawn->SetActorLocationAndRotation(_position, _rotation);
		_pawn->SetActorRotation(_rotation);

		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, *FString::Printf(TEXT("Position // %s: x:%f, y:%f, z: %f !"), GET_NETMODE_TEXT(), _position.X, _position.Y, _position.Z));
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, *FString::Printf(TEXT("Rotation // %s: x:%f, y:%f, z: %f !"), GET_NETMODE_TEXT(), _rotation.Pitch, _rotation.Roll, _rotation.Yaw));
	}
}

bool AS47GameState::TeleporteActorTo_Validate(APawn * _pawn, FVector _position, FRotator _rotation)
{
	return true;
}

void AS47GameState::SetSeed_Implementation(uint32 NewSeed)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, *FString::Printf(TEXT("%s: Seed set with: %d"), GET_NETMODE_TEXT(), NewSeed));
	Seed = NewSeed;
	OnSetSeed.Broadcast();
}

bool AS47GameState::SetSeed_Validate(uint32 NewSeed)
{
	return true;
}

void AS47GameState::OnRep_NumPlayer()
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan, *FString::Printf(TEXT("%s: Num Player: %d (Alive: %d | Dead: %d)"), GET_NETMODE_TEXT(), NumPlayer, NumPlayerAlive, NumPlayerDead));
}

void AS47GameState::OnRep_TimeLevel()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, *FString::Printf(TEXT("Time Level %f!"), TimeLevel));
}

void AS47GameState::SetBossHealth(float _bossHealth)
{
	bossHealth = _bossHealth;
}

void AS47GameState::SetBossHealthRPC_Implementation(float _bossHealth)
{
	SetBossHealth(_bossHealth);
}

bool AS47GameState::SetBossHealthRPC_Validate(float _bossHealth)
{
	return true;
}