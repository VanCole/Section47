// Fill out your copyright notice in the Description page of Project Settings.

#include "S47BTTask_AttackContact.h"
#include "Enemy/S47Enemy.h"
#include "AI/S47AIController.h"

EBTNodeResult::Type US47BTTask_AttackContact::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AS47AIController* MyController = Cast<AS47AIController>(OwnerComp.GetAIOwner());

	if (nullptr != MyController)
	{
		AS47Enemy* MyChar = Cast<AS47Enemy>(MyController->GetPawn());

		if (nullptr != MyChar)
		{
			MyChar->InitAttack();
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}
