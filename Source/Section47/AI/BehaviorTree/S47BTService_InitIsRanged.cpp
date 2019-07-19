// Fill out your copyright notice in the Description page of Project Settings.

#include "S47BTService_InitIsRanged.h"
#include "Enemy/S47Enemy.h"
#include "AI/S47AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void US47BTService_InitIsRanged::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AS47AIController* MyController = Cast<AS47AIController>(OwnerComp.GetAIOwner());

	bool isRange = false;
	bool isAlive = true;
	bool isStun = false;
	if (nullptr != MyController)
	{
		AS47Enemy* MyAIChar = Cast<AS47Enemy>(MyController->GetPawn());


		if (nullptr != MyAIChar)
		{
			isRange = MyAIChar->isRanged;
			isAlive = MyAIChar->m_isAlive;
			isStun = MyAIChar->m_isStun;
			if (!MyAIChar->alertedByEnvironnement)
			{
				MyAIChar->m_nearDistance = MyAIChar->GetActorLocation();
				MyAIChar->m_nearDistance.X += FMath::RandRange(-500, 500);
				MyAIChar->m_nearDistance.Y += FMath::RandRange(-500, 500);
			}
		}

	}

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();

	if (nullptr != BBComp)
	{

		BBComp->SetValueAsBool(TEXT("IsRange"), isRange);
		BBComp->SetValueAsBool(TEXT("IsAlive"), isAlive);
		BBComp->SetValueAsBool(TEXT("IsStun"), isStun);


	}
}