// Fill out your copyright notice in the Description page of Project Settings.

#include "S47BTTask_AttackRange.h"
#include "Enemy/Ranger/S47EnemyRange.h"
#include "AI/S47AIController.h"

EBTNodeResult::Type US47BTTask_AttackRange::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AS47AIController* MyController = Cast<AS47AIController>(OwnerComp.GetAIOwner());

	if (nullptr != MyController)
	{
		AS47EnemyRange* MyChar = Cast<AS47EnemyRange>(MyController->GetPawn());

		if (nullptr != MyChar)
		{
			MyChar->InitAttack();

			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}
