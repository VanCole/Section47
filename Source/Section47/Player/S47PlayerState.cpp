// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/S47PlayerState.h"
#include "S47Types.h"
#include "Net/UnrealNetwork.h"
#include "S47Character.h"
#include "S47GameState.h"
#include "Engine/World.h"

const float AS47PlayerState::StartHealth = 100.0f;
const float AS47PlayerState::StartArmor = 100.0f;
const float AS47PlayerState::StartFuel = 300.0f;

AS47PlayerState::AS47PlayerState()
	: Super()
{
	NetUpdateFrequency = 30.0f;
	MinNetUpdateFrequency = 30.0f;

	MaxHealth = StartHealth;
	CurrentHealth = MaxHealth;
	previousHealth = CurrentHealth;

	MaxArmor = StartArmor;
	CurrentArmor = MaxArmor;

	MaxFuel = StartFuel;
	CurrentFuel = MaxFuel;

	isDead = false;
}

void AS47PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AS47PlayerState, DetectedEnemy);
	DOREPLIFETIME(AS47PlayerState, CurrentHealth);
	DOREPLIFETIME(AS47PlayerState, CurrentArmor);
	DOREPLIFETIME(AS47PlayerState, CurrentFuel);
	DOREPLIFETIME(AS47PlayerState, MaxHealth);
	DOREPLIFETIME(AS47PlayerState, MaxArmor);
	DOREPLIFETIME(AS47PlayerState, MaxFuel);
	DOREPLIFETIME(AS47PlayerState, levelLife);
	DOREPLIFETIME(AS47PlayerState, levelDamage);
	DOREPLIFETIME(AS47PlayerState, levelAmmo);
	DOREPLIFETIME(AS47PlayerState, levelArmor);
	DOREPLIFETIME(AS47PlayerState, levelFuel);
	DOREPLIFETIME(AS47PlayerState, levelTime);
	DOREPLIFETIME(AS47PlayerState, killMonsters);
	DOREPLIFETIME(AS47PlayerState, damageDealed);
	DOREPLIFETIME(AS47PlayerState, damageTaken);
	DOREPLIFETIME(AS47PlayerState, levelPoint);
	DOREPLIFETIME(AS47PlayerState, totalLevelPoint);
	DOREPLIFETIME(AS47PlayerState, isReady);
	DOREPLIFETIME(AS47PlayerState, isDead);
	DOREPLIFETIME(AS47PlayerState, nbWeaponTaken);
	DOREPLIFETIME(AS47PlayerState, nbWeaponThrown);
	DOREPLIFETIME(AS47PlayerState, PlayerNickname);
}

void AS47PlayerState::BeginPlay()
{
	AS47GameState* GameState = GetWorld()->GetGameState<AS47GameState>();
	if (nullptr != GameState)
	{
		GameState->OnPreGeneration.AddUniqueDynamic(this, &AS47PlayerState::ResetPlayerGameStats);
	}

	previousLevelLife = 0;
	previousLevelDamage = 0;
	previousLevelAmmo = 0;
	previousLevelArmor = 0;
	previousLevelFuel = 0;
}

void AS47PlayerState::SetPlayerName(const FString & S)
{
	Super::SetPlayerName(S);
	PlayerNickname = S;
}

void AS47PlayerState::SetPlayerNameServer_Implementation(const FString & S)
{
	SetPlayerName(S);
	PlayerNickname = S;
}

bool AS47PlayerState::SetPlayerNameServer_Validate(const FString & S)
{
	return true;
}

void AS47PlayerState::OnRep_CurrentHealth()
{
	float delta = CurrentHealth - previousHealth;

	if (delta < 0.0f)
	{
		AS47Character* character = Cast<AS47Character>(GetPawn());
		if (nullptr != character)
		{
			character->onGetDamageEvent.Broadcast(delta, CurrentHealth);
		}
	}

	previousHealth = CurrentHealth;

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, *FString::Printf(TEXT("%s: Life Replicated %f"), GET_NETMODE_TEXT(), CurrentHealth));
}

void AS47PlayerState::OnRep_LevelLife()
{
	CurrentHealth += levelLife * 10.0f;
	MaxHealth += levelLife * 10.0f;
}

void AS47PlayerState::ModifyIsReady(bool ready)
{
	isReady = ready;
}

void AS47PlayerState::ModifyIsDead(bool _isDead)
{
	isDead = _isDead;
}

/*void AS47PlayerState::ModifyPlayerName(FString playerName)
{
	PlayerNickname = playerName;
}

void AS47PlayerState::ModifyPlayerNameRPC_Implementation(FString playerName)
{
	ModifyPlayerName(playerName);
}

bool AS47PlayerState::ModifyPlayerNameRPC_Validate(FString playerName)
{
	return true;
}*/

void AS47PlayerState::ModifyIsDeadRPC_Implementation(bool _isDead)
{
	ModifyIsDead(_isDead);
}

bool AS47PlayerState::ModifyIsDeadRPC_Validate(bool ready)
{
	return true;
}

void AS47PlayerState::ResetStats()
{
	CurrentHealth = MaxHealth;
	previousHealth = CurrentHealth;

	CurrentArmor = MaxArmor;

	CurrentFuel = MaxFuel;

	isDead = false;
}

void AS47PlayerState::ResetStatsRPC_Implementation()
{
	ResetStats();
}

bool AS47PlayerState::ResetStatsRPC_Validate()
{
	return true;
}

void AS47PlayerState::ApplicateModificationLevelPoint()
{
	if (levelLife != previousLevelLife)
	{
		float deltaLife = CurrentHealth / MaxHealth;
		MaxHealth = StartHealth * (1.0f + 0.1f * levelLife);
		CurrentHealth = MaxHealth * deltaLife;
		previousLevelLife = levelLife;
	}
	if (levelArmor != previousLevelArmor)
	{
		float deltaArmor = CurrentArmor / MaxArmor;
		MaxArmor = StartArmor * (1.0f + 0.1f * levelArmor);
		CurrentArmor = MaxArmor * deltaArmor;
		previousLevelArmor = levelArmor;
	}
	if (levelFuel != previousLevelFuel)
	{
		float deltaFuel = CurrentFuel / MaxFuel;
		MaxFuel = StartFuel * (1.0f + 0.1f * levelFuel);
		CurrentFuel = MaxFuel * deltaFuel;
		previousLevelFuel = levelFuel;
	}
}

void AS47PlayerState::ApplicateModificationLevelPointServer_Implementation()
{
	ApplicateModificationLevelPoint();
}

bool AS47PlayerState::ApplicateModificationLevelPointServer_Validate()
{
	return true;
}

void AS47PlayerState::ResetPlayerGameStats()
{
	if (HasAuthority())
	{
		killMonsters = 0;
		damageDealed = 0;
		damageTaken = 0;
		nbWeaponTaken = 0;
		nbWeaponThrown = 0;
	}
}

void AS47PlayerState::OnRep_DamageDealed()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, *FString::Printf(TEXT("Damage Dealed: %f"), damageDealed));
}

void AS47PlayerState::OnRep_DamageTaken()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, *FString::Printf(TEXT("Damage Taken: %f"), damageTaken));
}

void AS47PlayerState::OnRep_EnemyKilled()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, *FString::Printf(TEXT("Nb Enemy Killed: %d"), killMonsters));
}

void AS47PlayerState::OnRep_NbWeaponTaken()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, *FString::Printf(TEXT("Nb Weapon Taken: %d"), nbWeaponTaken));
}

void AS47PlayerState::OnRep_NbWeaponThrown()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, *FString::Printf(TEXT("Nb Weapon Thrown: %d"), nbWeaponThrown));
}

void AS47PlayerState::CalculPoints()
{
	levelPoint += killMonsters * 10;
	levelPoint += damageDealed;
	levelPoint -= damageTaken;

	if (nullptr != GetWorld())
	{
		AS47GameState* gameState = Cast<AS47GameState>(GetWorld()->GetGameState());

		if (nullptr != gameState)
		{
			levelPoint -= gameState->TimeLevel;
		}
	}

	totalLevelPoint += levelPoint;
}

void AS47PlayerState::ModifyIsReadyRPC_Implementation(bool ready)
{
	ModifyIsReady(ready);
}

bool AS47PlayerState::ModifyIsReadyRPC_Validate(bool ready)
{
	return true;
}