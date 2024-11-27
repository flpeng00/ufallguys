// Fill out your copyright notice in the Description page of Project Settings.


#include "FGAIController.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"

AFGAIController::AFGAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BB_FGCharacter(TEXT("/Game/AI/BB_FGCharacter.BB_FGCharacter"));
	if (BB_FGCharacter.Succeeded()) {
		BBAsset = BB_FGCharacter.Object;
	}
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BT_FGCharacter(TEXT("/Game/AI/BT_FGCharacter.BT_FGCharacter"));
	if (BT_FGCharacter.Succeeded()) {
		BTAsset = BT_FGCharacter.Object;
	}
}

void AFGAIController::RunAI()
{
	UBlackboardComponent* BlackboardComponent = Blackboard.Get();
	if (UseBlackboard(BBAsset, BlackboardComponent))
	{
		bool bRunResult = RunBehaviorTree(BTAsset);
		ensure(bRunResult);
	}
}

void AFGAIController::StopAI()
{
	UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BehaviorTreeComponent) {
		BehaviorTreeComponent->StopTree();
	}
}

void AFGAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	RunAI();
}
