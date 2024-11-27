// Fill out your copyright notice in the Description page of Project Settings.


#include "FGWidgetComponent.h"
#include "FGUserWidget.h"

void UFGWidgetComponent::InitWidget()
{
	Super::InitWidget();
	
	UFGUserWidget* UserWidget = Cast<UFGUserWidget>(GetWidget());
	if (!!UserWidget) {
		UserWidget->SetOwner(GetOwner());
	}
}
