// Fill out your copyright notice in the Description page of Project Settings.


#include "FGAnimInstanceCharacter.h"
#include "FGCharacterBase.h"
#include "GameFramework/PawnMovementComponent.h"

UFGAnimInstanceCharacter::UFGAnimInstanceCharacter()
{
	CurrentPawnSpeed = 0.0;
	bIsFalling = false;
	bIsReadyToJump = false;
}

void UFGAnimInstanceCharacter::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	AFGCharacterBase* character = Cast<AFGCharacterBase>(TryGetPawnOwner());
	if (character != nullptr) {
		CurrentPawnSpeed = character->GetVelocity().Size();
		UPawnMovementComponent* movement = character->GetMovementComponent();
		if (movement != nullptr) {
			bIsFalling = movement->IsFalling();
		}
	}
}

void UFGAnimInstanceCharacter::ReadyToJump()
{
	bIsReadyToJump = true;
}

bool UFGAnimInstanceCharacter::IsFalling() const
{
	return !!bIsFalling;
}

bool UFGAnimInstanceCharacter::IsReadyToJump() const
{
	return !!bIsReadyToJump;
}

void UFGAnimInstanceCharacter::AnimNotify_JumpStart()
{
	bIsReadyToJump = false;
	OnAnimJumpStart.Broadcast();
}

void UFGAnimInstanceCharacter::AnimNotify_JumpEnd()
{
	bIsReadyToJump = false;
	OnAnimJumpEnd.Broadcast();
}
