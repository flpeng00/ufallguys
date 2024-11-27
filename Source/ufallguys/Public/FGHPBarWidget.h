// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FGUserWidget.h"
#include "FGHPBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class UFALLGUYS_API UFGHPBarWidget : public UFGUserWidget
{
	GENERATED_BODY()
	
public:
	UFGHPBarWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	void SetMaxHp(float val);
	void UpdateHpBar(float NewCurrentHp);

protected:
	UPROPERTY()
	TObjectPtr<class UProgressBar> HpProgressBar;

	UPROPERTY()
	float MaxHp;
};
