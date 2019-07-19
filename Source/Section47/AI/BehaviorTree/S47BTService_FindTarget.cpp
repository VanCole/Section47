// Fill out your copyright notice in the Description page of Project Settings.

#include "S47BTService_FindTarget.h"

#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"

#include "Enemy/S47Enemy.h"
#include "AI/S47AIController.h"
#include "Enemy/S47AIPerceptionComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "S47GameState.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void US47BTService_FindTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AS47AIController* MyController = Cast<AS47AIController>(OwnerComp.GetAIOwner());

	AS47Enemy* MyAIChar = Cast<AS47Enemy>(MyController->GetPawn());

	AS47PlayerCharacter* MyPlayerChar = MyAIChar->m_player;
	float DistanceBetweenChar = -2.f;


	if (nullptr != MyController)
	{
		US47AIPerceptionComponent* PercComp = Cast<US47AIPerceptionComponent>(MyController->GetPerceptionComponent());

		if (nullptr != PercComp)
		{
			TArray<AActor*> ActorPerceived;
			PercComp->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), ActorPerceived);
			AS47PlayerCharacter* PlayerCharPerceive = nullptr;
			for (auto Actor : ActorPerceived)
			{
				if (Cast<AS47PlayerCharacter>(Actor) != nullptr)
				{
					PlayerCharPerceive = Cast<AS47PlayerCharacter>(Actor);
				}
			}
			ActorPerceived.Empty();
			PercComp->GetCurrentlyPerceivedActors(UAISense_Hearing::StaticClass(), ActorPerceived);

			for (auto Actor : ActorPerceived)
			{
				if (Cast<AS47PlayerCharacter>(Actor) != nullptr)
				{
					PlayerCharPerceive = Cast<AS47PlayerCharacter>(Actor);
				}
				else
				{
					MyAIChar->timeForgotenPlayer = MyAIChar->timeBeforeForgotenPlayer - 0.2f;
					MyAIChar->m_nearDistance = Actor->GetActorLocation();
				}
			}

			if (PlayerCharPerceive != nullptr)
			{
				MyPlayerChar = PlayerCharPerceive;
				MyAIChar->timeForgotenPlayer = 0;
			}
		}
	}

	//Oubli player when too far for too long
	if (nullptr != MyAIChar->m_player)
	{
		if (MyAIChar->timeForgotenPlayer > MyAIChar->timeBeforeForgotenPlayer)
		{
			//REMOVE
			MyPlayerChar = nullptr;

			if (nullptr != MyAIChar->m_player->GetPlayerState<AS47PlayerState>())
			{
				MyAIChar->m_player->GetPlayerState<AS47PlayerState>()->DetectedEnemy.Remove(MyAIChar);
			}

			MyAIChar->m_player = nullptr;
			MyAIChar->m_findPlayer = false;

		}
	}


	if (nullptr != MyAIChar)
	{
		if (MyAIChar->m_findPlayer)
		{

			if (MyAIChar->m_player != nullptr && MyAIChar->m_player->GetController() == nullptr)
			{
				//REMOVE
				if (nullptr != MyAIChar->m_player->GetPlayerState<AS47PlayerState>())
				{
					MyAIChar->m_player->GetPlayerState<AS47PlayerState>()->DetectedEnemy.Remove(MyAIChar);
				}
				MyAIChar->m_player = nullptr;
				MyAIChar->m_findPlayer = false;
			}
			else
			{
				float distanceMin = 100000.0f;
				float tempDistance = distanceMin;

				for (APlayerState* player : GetWorld()->GetGameState<AS47GameState>()->PlayerArray)
				{
					if (nullptr != player->GetPawn())
					{
						tempDistance = FVector::Dist(player->GetPawn()->GetActorLocation(), MyAIChar->GetActorLocation());
						if (tempDistance < distanceMin)
						{
							distanceMin = tempDistance;
							MyPlayerChar = Cast<AS47PlayerCharacter>(player->GetPawn());
						}
					}
				}
			}
		}

		if (nullptr != MyPlayerChar)
		{
			MyAIChar->GetCharacterMovement()->MaxWalkSpeed = MyAIChar->speedMax;
			DistanceBetweenChar = MyAIChar->GetDistanceTo(MyPlayerChar);
			if (MyAIChar->m_player == nullptr)
			{
				//ADD			
				UAISense_Hearing::ReportNoiseEvent(GetWorld(), MyAIChar->GetActorLocation(), 1, MyAIChar, 3000);
			}

			MyAIChar->m_player = MyPlayerChar;
			if (nullptr != MyAIChar->m_player->GetPlayerState<AS47PlayerState>() && MyAIChar->m_isAlive)
			{
				MyAIChar->m_player->GetPlayerState<AS47PlayerState>()->DetectedEnemy.AddUnique(MyAIChar);
			}
		}
		else
		{
			if (MyAIChar->alertedByEnvironnement)
			{
				DistanceBetweenChar = FVector::Dist(MyAIChar->m_nearDistance, MyAIChar->GetActorLocation());
				MyAIChar->GetCharacterMovement()->MaxWalkSpeed = MyAIChar->speedMax;
				if (DistanceBetweenChar < 200)
				{
					MyAIChar->alertedByEnvironnement = false;

				}
			}
			else
			{
				MyAIChar->GetCharacterMovement()->MaxWalkSpeed = MyAIChar->speedPatrouille;
			}
		}
	}

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	BBComp->SetValueAsBool(TEXT("CanAttackTarget"), false);

	if (nullptr != BBComp)
	{

		if (nullptr != MyAIChar)
		{
			if (DistanceBetweenChar > 0 && DistanceBetweenChar < MyAIChar->m_rangeAttack)
			{
				if (MyAIChar->CanAttack())
				{
					BBComp->SetValueAsBool(TEXT("CanAttackTarget"), true);
					BBComp->SetValueAsBool(TEXT("isAttacking"), true);
				}
			}

			if (MyAIChar->m_findPlayer == true || (DistanceBetweenChar > -1.f && DistanceBetweenChar < MyAIChar->m_rangeVisionForward) && nullptr != MyPlayerChar) //modifier cette valeur par la vision de l'enemy
			{

				BBComp->SetValueAsBool(TEXT("HasTarget"), true);
				BBComp->SetValueAsObject(TEXT("Target"), MyPlayerChar);
				//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("%s"), *GetNameSafe(MyPlayerChar)));
				MyAIChar->m_findPlayer = true;

			}
			else
			{
				BBComp->SetValueAsVector(TEXT("NearDistance"), MyAIChar->m_nearDistance);
				BBComp->SetValueAsBool(TEXT("HasTarget"), false);
				BBComp->SetValueAsObject(TEXT("Target"), nullptr);
			}

			if (!BBComp->GetValueAsBool(TEXT("IsInit")))
			{
				MyAIChar->m_nearDistance = MyAIChar->GetActorLocation();
				MyAIChar->m_nearDistance.X += FMath::RandRange(-500, 500);
				MyAIChar->m_nearDistance.Y += FMath::RandRange(-500, 500);
				BBComp->SetValueAsBool(TEXT("IsInit"), true);
			}
		}
	}
}