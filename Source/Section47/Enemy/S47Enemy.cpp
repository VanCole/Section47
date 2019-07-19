// Fill out your copyright notice in the Description page of Project Settings.

#include "S47Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AI/S47AIController.h"
#include "S47AIPerceptionComponent.h"
#include "S47Types.h"
#include "Ranger/S47EnemyRange.h"
#include "Perception/AISenseConfig_Sight.h"
#include "S47GameMode.h"
#include "S47GameState.h"
#include "Player/S47PlayerState.h"


void AS47Enemy::BeginPlay()
{
	Super::BeginPlay();
	currentHealth = m_lifeMax;
	MyController = Cast<AS47AIController>(GetController());
	//GetCharacterMovement()->bWantsToCrouch = true;

	////Faire une requete au serveur pour récupérer les joueurs dans m_players
	//ACharacter* player = UGameplayStatics::GetPlayerCharacter(nullptr, 0);
	//if (nullptr != player)
	//{
	//	m_player = Cast<AS47PlayerCharacter>(player);
	//	if (nullptr == m_player)
	//	{
	//		UE_LOG(LogTemp, Error, TEXT("Player is Vanished"));
	//	}
	//}

	AS47GameState* GameState = GetWorld()->GetGameState<AS47GameState>();
	if (nullptr != GameState)
	{
		GameState->OnBossEnd.AddUniqueDynamic(this, &AS47Enemy::ForcedKill);
		GameState->OnBossUnlock.AddUniqueDynamic(this, &AS47Enemy::ForcedKill);
		GameState->OnGameOver.AddUniqueDynamic(this, &AS47Enemy::ForcedKill);

	}
}

//Faire de la perception
//void AS47Enemy::NearPlayer()
//{
//	FVector m_position = GetCharacterMovement()->GetActorLocation();
//	FVector playerPosition = m_player->GetCharacterMovement()->GetActorLocation();
//	if (FVector::Dist(m_position, playerPosition) < m_rangeVisionForward)
//	{
//		if (FVector::Dist(m_position, playerPosition) < m_rangeVisionBack)
//		{
//			
//			/*FHitResult hit;
//			FVector offsetVisionEnemy = m_position;
//			FVector offsetVisionPlayer = playerPosition;
//			offsetVisionEnemy.Y += 0.75f;
//			offsetVisionPlayer.Y += 0.75f;
//			ActorLineTraceSingle(hit, m_position, offsetVisionPlayer - offsetVisionEnemy,Ecc)
//		*/}
//	}
//}

void AS47Enemy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AS47Enemy, m_isAlive);

}

void AS47Enemy::EnemyFeedBack()
{
	m_timeHit -= UGameplayStatics::GetWorldDeltaSeconds(nullptr);
	if (m_timeHit > 0)
	{
		m_hit = true;
		//color Red Feed Back
	}
	else if (m_hit)
	{
		m_hit = false;
	}
}

void AS47Enemy::FindNearPlayer()
{
	float minDistance = 10000;

	//for (AS47PlayerCharacter* player : m_players)
	{
		FVector m_position = GetCharacterMovement()->GetActorLocation();
		//FVector playerPosition = player->GetCharacterMovement()->GetActorLocation();
		FVector playerPosition = m_player->GetCharacterMovement()->GetActorLocation();
		float temp = FVector::Dist(m_position, playerPosition);
		if (temp < minDistance)
		{
			minDistance = temp;
			//m_player = player;
		}
	}
}

void AS47Enemy::Kill()
{
	InvokeDeath();
	m_isAlive = false;
	if (nullptr != m_player)
	{
		if (m_player->GetPlayerState<AS47PlayerState>() != nullptr)
		{
			if (m_player->GetPlayerState<AS47PlayerState>()->DetectedEnemy.Contains(this))
			{
				m_player->GetPlayerState<AS47PlayerState>()->DetectedEnemy.Remove(this);
			}
		}
	}

}

void AS47Enemy::ForcedKill()
{
	LastDamageDealer = nullptr;
	canPlaySound = false;
	Kill();

}

void AS47Enemy::DoAttackBP()
{
	/*if (isRanged)
	{
		AS47EnemyRange* MyAIChar = Cast<AS47EnemyRange>(MyController->GetPawn());
		if (MyAIChar != nullptr)
		{
			MyAIChar->AttackRange();
		}
	}
	else
	{
		Attack();
	}*/
	if (HasAuthority())
	{
		Attack();
	}
}

void AS47Enemy::EndAttack()
{
	EndAttackBP();
}

void AS47Enemy::OnStun()
{
	m_timeStun = 1.5f;
}

void AS47Enemy::InitAttack()
{
	IsAttacking = true;

}

void AS47Enemy::Attack()
{
	OnAttackBP();
	if (nullptr != m_player)
	{
		if (FVector::Dist(m_player->GetActorLocation(), GetActorLocation()) < m_rangeAttack)
		{
			m_player->GetDamage(m_dmg, this);
		}
	}
}



void AS47Enemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!m_isAlive)
	{
		return;
	}
	if (m_timeStun > 0.f)
	{
		m_isStun = true;
		m_timeStun -= DeltaTime;
		if (m_timeStun <= 0.f)
		{
			m_isStun = false;
		}
	}


	if (GetHealth() <= 0 && m_isAlive)
	{
		Kill();
	}

	if (!alwaysFindPlayer && !alertedByEnvironnement && timeForgotenPlayer <= timeBeforeForgotenPlayer)
	{
		timeForgotenPlayer += DeltaTime;
	}

	//FindNearPlayer();
	//if (!m_findPlayer)
	//{
	//	//NearPlayer();
	//	//faire la perception ici
	//}
	//EnemyAttack();
	//EnemyFeedBack();

//#if WITH_EDITOR
	// Debug key to manually kill all enemies
	if (GetWorld()->GetFirstPlayerController()->WasInputKeyJustPressed(EKeys::End))
	{
		AS47GameMode* GameMode = GetWorld()->GetAuthGameMode<AS47GameMode>();
		if (GameMode != nullptr)
		{
			ForcedKill();
		}
	}
//#endif
}

bool AS47Enemy::CanAttack()
{
	FVector EndRayCast = GetActorLocation() + (m_player->GetActorLocation() - GetActorLocation());
	FHitResult ResultHit;

	if (nullptr != GetWorld())
	{
		GetWorld()->LineTraceSingleByChannel(ResultHit, GetActorLocation(), EndRayCast, ECollisionChannel::ECC_GameTraceChannel5 /*IAProjectile*/);

		//DrawDebugLine(GetWorld(), StartLocation, EndRayCast, FColor::Red, false, 5.f);

		//if (ResultHit.bBlockingHit && nullptr != ResultHit.GetActor() && ResultHit.GetActor()->ActorHasTag(TEXT("TrapGround")))
		if (ResultHit.bBlockingHit)
		{
			if (Cast<AS47PlayerCharacter>(ResultHit.GetActor()))
			{
				return true;
			}
		}
	}
	return false;
}

//AS47Enemy::AS47Enemy()
//{
//	//récupérer・color
//	m_life = m_lifeMax;
//	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("You Die: %f"), m_life));
//}

void AS47AICharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	S47TeamID.SetAttitudeSolver(&S47TeamAttitudeSolver);

	AS47AIController* CurrentController = Cast<AS47AIController>(NewController);

	UAIPerceptionSystem* AIPerceptionSys = UAIPerceptionSystem::GetCurrent(GetWorld());

	if (nullptr != CurrentController && nullptr != CurrentController->PerceptionComponent && AIPerceptionSys != nullptr)
	{
		CurrentController->SetGenericTeamId(S47TeamID);
		AIPerceptionSys->UpdateListener(*CurrentController->PerceptionComponent);

	}
}


