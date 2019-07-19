// Fill out your copyright notice in the Description page of Project Settings.

#include "S47BTTask_MultiProjectilePhase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/Boss/S47BossDistance.h"

EBTNodeResult::Type US47BTTask_MultiProjectilePhase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("MultipleProjectilePhase")));
	AS47AIController* MyController = Cast<AS47AIController>(OwnerComp.GetAIOwner());
	AS47BossDistance* MyChar = Cast<AS47BossDistance>(MyController->GetPawn());
	if (!MyChar->isInPhase)
	{
		BBComp->SetValueAsBool(TEXT("MultipleProjectilePhase"), false);
	}
	return EBTNodeResult::Succeeded;
}