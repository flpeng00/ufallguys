// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FGCharacterBase.h"
#include "FGCharacterPlayer.generated.h"

/**
 * 
 */
UCLASS()
class UFALLGUYS_API AFGCharacterPlayer : public AFGCharacterBase
{
	GENERATED_BODY()

public:
	AFGCharacterPlayer();

protected:
	virtual void BeginPlay() override;

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<class USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<class UCameraComponent> Camera;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<class UInputAction> InputActionMove;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<class UInputAction> InputActionLook;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<class UInputAction> InputActionJump;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<class UInputAction> InputActionHeadbutt;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<class UInputAction> InputActionShootMode;

	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<class UInputAction> InputActionShoot;

protected:
	virtual void SetCharacterControlData(const class UFGCharacterControlData* CharacterControlData) override;
	void SetShootViewMode();
	void SetNormalViewMode();
	void SetViewMode(EViewMode NewViewMode);

	EViewMode CurrentViewMode;
};
