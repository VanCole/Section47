// Fill out your copyright notice in the Description page of Project Settings.

#include "S47BTService_BossSetTarget.h"

#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISense_Sight.h"

#include "Enemy/S47Enemy.h"
#include "Enemy/Boss/S47BossDistance.h"
#include "AI/S47AIController.h"
#include "Enemy/S47AIPerceptionComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "S47GameState.h"

void US47BTService_BossSetTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AS47AIController* MyController = Cast<AS47AIController>(OwnerComp.GetAIOwner());

	AS47BossDistance* MyAIChar = Cast<AS47BossDistance>(MyController->GetPawn());

	AS47PlayerCharacter* MyPlayerChar = nullptr;// Cast<AS47PlayerCharacter>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn());

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

	if (!MyAIChar->init)
	{
		MyAIChar->m_player = MyPlayerChar;
	}

	float DistanceBetweenChar = 6000;

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();

	if (nullptr != MyPlayerChar && nullptr != MyAIChar && !MyAIChar->init)
	{
		DistanceBetweenChar = MyAIChar->GetDistanceTo(MyPlayerChar);
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("%f"), DistanceBetweenChar));
		if (DistanceBetweenChar < 3000)
		{
			MyAIChar->m_player = MyPlayerChar;
			BBComp->SetValueAsBool(TEXT("WaitingPlayer"), true);
			//MyAIChar->init = true;
		}
	}

	

	if (nullptr != BBComp)
	{
		if (BBComp->GetValueAsBool(TEXT("MultipleProjectilePhase")))
		{
			BBComp->SetValueAsBool(TEXT("DoubleLaserPhase"), false);
			BBComp->SetValueAsVector(TEXT("Target1"), MyAIChar->GetActorLocation() + FVector::RightVector);
			BBComp->SetValueAsVector(TEXT("Target2"), MyAIChar->GetActorLocation() - FVector::RightVector);
			
		}


		//if ()) //condition pour change la target à deux joueur (un random ou le joueur le plus proche)
		{

			BBComp->SetValueAsObject(TEXT("Target"), MyPlayerChar);

		}

	}
}