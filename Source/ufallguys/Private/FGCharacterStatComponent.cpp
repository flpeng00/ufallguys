// Fill out your copyright notice in the Description page of Project Settings.


#include "FGCharacterStatComponent.h"

// Sets default values for this component's properties
UFGCharacterStatComponent::UFGCharacterStatComponent()
{
	MaxHp = 200;
	CurrentHp = MaxHp;
}


// Called when the game starts
void UFGCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

float UFGCharacterStatComponent::GetMaxHp() const
{
	return MaxHp;
}

float UFGCharacterStatComponent::GetCurrentHp() const
{
	return CurrentHp;
}

float UFGCharacterStatComponent::ApplyDamage(float Damage, AActor* Attacker, EDamageType DamageType)
{
	if (Damage < KINDA_SMALL_NUMBER)
		return 0.0f;

	const float PrevHp = CurrentHp;
	const float ActualDamage = FMath::Clamp<float>(Damage, 0, Damage);
	SetHp(PrevHp - Damage);
	if (CurrentHp < KINDA_SMALL_NUMBER) {
		OnHpZero.Broadcast();
	}

	return 0.0f;
}

void UFGCharacterStatComponent::SetHp(float NewHp)
{
	CurrentHp = FMath::Clamp<float>(NewHp, 0.0f, MaxHp);
	OnHpChanged.Broadcast(CurrentHp);
}

