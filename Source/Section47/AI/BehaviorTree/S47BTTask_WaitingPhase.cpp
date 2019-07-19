// Fill out your copyright notice in the Description page of Project Settings.

#include "S47BTTask_WaitingPhase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/Boss/S47BossDistance.h"
#include "AI/S47AIController.h"

EBTNodeResult::Type US47BTTask_WaitingPhase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//Super::ExecuteTask(OwnerComp, NodeMemory);

	//AS47AIController* MyController = Cast<AS47AIController>(OwnerComp.GetAIOwner());
	////GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("WaitingPhase")));
	//if (nullptr != MyController)
	//{
	//	AS47BossDistance* MyChar = Cast<AS47BossDistance>(MyController->GetPawn());

	//	if (nullptr != MyChar)
	//	{
	//		int phase = MyChar->SetPhase();
	//		
	//		UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	//		switch (phase)
	//		{
	//		case 1: BBComp->SetValueAsBool(TEXT("MultipleProjectilePhase"), true);
	//			MyChar->multiProjectilePhase = true;
	//			break;
	//		case 2: BBComp->SetValueAsBool(TEXT("JetAcidePhase"), true);
	//			MyChar->jetAcidePhase = true;
	//			break;
	//		default:
	//			break;
	//		}

	//		MyChar->isInPhase = true;
	//		MyChar->init = true;
	//		return EBTNodeResult::Succeeded;
	//	}
	//}

	//return EBTNodeResult::Failed;
	return EBTNodeResult::Succeeded;
}