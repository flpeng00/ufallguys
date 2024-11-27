// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FGUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class UFALLGUYS_API UFGUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetOwner(AActor* NewOwner);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Actor");
	TObjectPtr<AActor> Owner;
	
};
