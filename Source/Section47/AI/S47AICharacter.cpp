// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/S47AICharacter.h"
#include "Player/S47PlayerCharacter.h"


void AS47AICharacter::BeginPlay()
{
	Super::BeginPlay();
	onDeathEvent.AddUniqueDynamic(this, &AS47AICharacter::OnDeath);
}

void AS47AICharacter::SetHealth(float _health)
{
	float delta = _health - currentHealth;
	currentHealth = _health;


	onHealthChangeEvent.Broadcast(delta, _health);

	if (delta > 0)
	{
		onHealEvent.Broadcast(delta, _health);
	}
	else if (delta < 0)
	{
		onGetDamageEvent.Broadcast(delta, _health);
	}
}

void AS47AICharacter::GetDamage(float _amount, AActor * Dealer)
{
	Super::GetDamage(_amount, Dealer);

	// Apply Damages
	if (CanBeDamaged())
	{
		SetHealth(GetHealth() - _amount);
	}

	// If dealer is a player, then add damages to its stats
	AS47PlayerCharacter* Player = Cast<AS47PlayerCharacter>(Dealer);
	if (nullptr != Player)
	{
		AS47PlayerState* PlayerState = Cast<AS47PlayerState>(Player->GetPlayerState());
		if (nullptr != PlayerState)
		{
			PlayerState->damageDealed += _amount;
		}
	}
}

void AS47AICharacter::SaveLastShot(FVector _force_Saved, FVector _poshit_Saved, FName _boneTouched_Saved)
{
	force_Saved = _force_Saved;
	poshit_Saved = _poshit_Saved;
	boneTouched_Saved = _boneTouched_Saved;
}

void AS47AICharacter::SaveLastShot_ToClient_Implementation(FVector _force_Saved, FVector _poshit_Saved, FName _boneTouched_Saved)
{
	SaveLastShot(_force_Saved, _poshit_Saved, _boneTouched_Saved);
}

void AS47AICharacter::OnDeath(AController * _controller, AActor * killer)
{
	AS47PlayerCharacter* player = Cast<AS47PlayerCharacter>(killer);
	if (nullptr != player)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan, *FString::Printf(TEXT("Enemy killed by %s"), *GetNameSafe(player)));
		AS47PlayerState* state = player->GetPlayerState<AS47PlayerState>();
		if (nullptr != GetWorld()->GetAuthGameMode() && nullptr != state)
		{
			state->killMonsters++;
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, *FString::Printf(TEXT("Nb Enemy killed %d"), state->killMonsters));
		}
	}
}
