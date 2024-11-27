// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackHeadbuttAnimNotify.h"
#include "FGAnimationAttackInterface.h"

void UAttackHeadbuttAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp == nullptr)
		return;
	
	IFGAnimationAttackInterface* owner = Cast<IFGAnimationAttackInterface>(MeshComp->GetOwner());
	if (owner == nullptr)
		return;

	owner->AttackHitCheck();
}
