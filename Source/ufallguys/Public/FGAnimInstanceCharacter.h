// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FGAnimInstanceCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnimNotifyTriggered);

/**
 * 
 */
UCLASS()
class UFALLGUYS_API UFGAnimInstanceCharacter : public UAnimInstance
{
	GENERATED_BODY()

public:
	UFGAnimInstanceCharacter();

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	void ReadyToJump();
	bool IsFalling() const;
	bool IsReadyToJump() const;

private:
	UFUNCTION()
	void AnimNotify_JumpStart();
	
	UFUNCTION()
	void AnimNotify_JumpEnd();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AnimTrigger")
	FOnAnimNotifyTriggered OnAnimJumpStart;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AnimTrigger")
	FOnAnimNotifyTriggered OnAnimJumpEnd;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, Meta=(AllowPrivateAccess = true))
	float CurrentPawnSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Pawn, Meta = (AllowPrivateAccess = true))
	uint8 bIsFalling:1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Pawn, Meta = (AllowPrivateAccess = true))
	uint8 bIsReadyToJump:1;
};
