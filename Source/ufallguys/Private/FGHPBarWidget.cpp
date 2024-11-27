// Fill out your copyright notice in the Description page of Project Settings.


#include "FGHPBarWidget.h"
#include "Components/ProgressBar.h"
#include "FGCharacterWidgetInterface.h"

UFGHPBarWidget::UFGHPBarWidget(const FObjectInitializer& ObjectInitializer)
{
	MaxHp = -1;
}

void UFGHPBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HpProgressBar = Cast<UProgressBar>(GetWidgetFromName(TEXT("PbHpBar")));
	ensure(!!HpProgressBar);

	IFGCharacterWidgetInterface* CharacterInterface = Cast<IFGCharacterWidgetInterface>(Owner);
	if (!!Owner)
	{
		CharacterInterface->SetupCharacterWidget(this);
	}
}

void UFGHPBarWidget::SetMaxHp(float val)
{
	MaxHp = val;
}

void UFGHPBarWidget::UpdateHpBar(float NewCurrentHp)
{
	ensure(MaxHp > 0.0f);
	if (!!HpProgressBar)
	{
		HpProgressBar->SetPercent(NewCurrentHp / MaxHp);
	}
}
