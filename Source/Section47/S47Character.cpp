// Fill out your copyright notice in the Description page of Project Settings.

#include "S47Character.h"
#include "AI/S47AICharacter.h"
#include "Player/S47PlayerState.h"
#include "S47Types.h"
// Sets default values
AS47Character::AS47Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	//maxHealth = 100.0f;
	//currentHealth = maxHealth;
	LastDamageDealer = nullptr;
}

AS47Character::AS47Character(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

}

//void AS47Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME(AS47Character, LastDamageDealer);
//}

// Called when the game starts or when spawned
void AS47Character::BeginPlay()
{
	Super::BeginPlay();
	

	onGetDamageEvent.AddUniqueDynamic(this, &AS47Character::OnGetDamage);
	onHealthChangeEvent.AddUniqueDynamic(this, &AS47Character::OnHealthChange);
	onHealEvent.AddUniqueDynamic(this, &AS47Character::OnHeal);

	canBeDamaged = true;
}

// Called every frame
void AS47Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AS47Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AS47Character::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (S47TeamID != NewTeamID)
	{
		S47TeamID = NewTeamID;
		S47TeamID.SetAttitudeSolver(&S47TeamAttitudeSolver);
	}
}

//void AS47Character::GetDamage(float _amountDamage)
//{
//	currentHealth -= _amountDamage;
//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("CurrentHealth: %f"), currentHealth));
//}

bool AS47Character::IsValidActorForCollision(AActor * _OtherActor)
{
	if (nullptr != _OtherActor && _OtherActor->IsA<AS47Character>() && !_OtherActor->IsA<AS47AICharacter>())
		return true;

	return false;
}

//void AS47Character::Heal(float _amount)
//{
//	AS47PlayerState* State = GetPlayerState<AS47PlayerState>();
//	if (State != nullptr)
//	{
//		State->CurrentHealth += _amount;
//		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("%s: CurrentHealth: %f"), GET_NETMODE_TEXT(), currentHealth));
//	}
//}


//void AS47Character::SetHealth_Implementation(float _health)
//{
//	AS47PlayerState* State = GetPlayerState<AS47PlayerState>();
//	if (State != nullptr)
//	{
//		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Orange, *FString::Printf(TEXT("%s: %s Health set to %f"), *GetName(), GET_NETMODE_TEXT(), State->CurrentHealth));
//		State->CurrentHealth = _health;
//		if (!State->IsAlive())
//		{
//			InvokeDeath();
//		}
//	}
//}
//
//bool AS47Character::SetHealth_Validate(float _health)
//{
//	return true;
//}

void AS47Character::InvokeDeath_Implementation()
{
	if (!isAlreadyDead)
	{
		isAlreadyDead = true;
		//GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, *FString::Printf(TEXT("%s: %s Player dead !"), GET_NETMODE_TEXT(), *GetName()));
		onDeathEvent.Broadcast(GetController(), LastDamageDealer);
		OnDeathBP(GetController());
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Yellow, *FString::Printf(TEXT("Character %s killed by %s"), *GetNameSafe(this), *GetNameSafe(LastDamageDealer)));
	}
}

bool AS47Character::InvokeDeath_Validate()
{
	return true;
}

bool AS47Character::CanBeDamaged()
{
	return canBeDamaged;
}

void AS47Character::GetDamage(float _amount, AActor * Dealer)
{
	SetLastDamageDealer(Dealer);
}

void AS47Character::SetLastDamageDealer_Implementation(AActor * Dealer)
{
	LastDamageDealer = Dealer;


	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, *FString::Printf(TEXT("Received Damage from %s"), *GetNameSafe(Dealer)));
}

//void AS47Character::DeathAnimationStart_Implementation()
//{
//	onStartDeathAnimation.Broadcast();
//	OnStartDeathAnimationBP();
//}

void AS47Character::DeathAnimationEnd_Implementation()
{
	if (!hasAlreadyEndDeath)
	{
		hasAlreadyEndDeath = true;
		onEndDeathAnimation.Broadcast(GetController(), LastDamageDealer);
		OnEndDeathAnimationBP(GetController());
	}
}
