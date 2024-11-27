// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FGCharacterMovementComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFloorChanged, AActor*, NewFloorActor);

/**
 * 
 */
UCLASS()
class UFALLGUYS_API UFGCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UFGCharacterMovementComponent();

public:
	virtual void PhysWalking(float deltaTime, int32 Iterations) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Friction")
	FOnFloorChanged OnFloorChanged;

protected:
	UPROPERTY(EditAnywhere, Category="Friction")
	float SlipVelocity;

	UPROPERTY(VisibleAnywhere, Category="Friction")
	int32 LastFloor;
};
