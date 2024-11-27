// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FGCharacterStatComponent.generated.h"

UENUM(BlueprintType)
enum class EDamageType : uint8
{
	None UMETA(DisplayName = "None"),
	ByEnemy UMETA(DisplayName = "ByEnemy"),
};

DECLARE_MULTICAST_DELEGATE(FOnHpZeroDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHpChangedDelegate, float /*CurrentHp*/);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UFALLGUYS_API UFGCharacterStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFGCharacterStatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	FOnHpZeroDelegate OnHpZero;
	FOnHpChangedDelegate OnHpChanged;

	float GetMaxHp() const;
	float GetCurrentHp() const;
	float ApplyDamage(float Damage, AActor* Attacker, EDamageType DamageType);

protected:
	void SetHp(float NewHp);

	UPROPERTY(EditInstanceOnly, Category=Stat)
	float MaxHp;

	UPROPERTY(Transient, EditInstanceOnly, Category=Stat)
	float CurrentHp;
};
