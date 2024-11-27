// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "FGAnimationAttackInterface.h"
#include "FGCharacterWidgetInterface.h"
#include "FGCharacterBase.generated.h"

UENUM()
enum class EViewMode : uint8
{
	NormalView,
	ShootView,
};


UCLASS()
class UFALLGUYS_API AFGCharacterBase
	: public ACharacter
	, public IFGAnimationAttackInterface
	, public IFGCharacterWidgetInterface
{
	GENERATED_BODY()

public:
	static constexpr float DefaultGroundFriction = 8.0f;
	static constexpr float DefaultBreakingFrictionFactor = 2.0f;
	static constexpr float DefaultBreakingDecelerationWalking = 200.f;

	AFGCharacterBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void SetFrictionInfoOnCurrentFloor();

public:	
	virtual void PostInitializeComponents() override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	virtual void SetupCharacterWidget(class UFGUserWidget* Widget) override;
	virtual void AttackHitCheck() override;

protected:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void SetDead();
	void PlayDeadAnim();

public:
	bool IsImmovable() const;
	void ReadyToJump();
	void Look(const FInputActionValue& Value);
	void Move(const FInputActionValue& Value);

	void AttackHeadbutt();

	UFUNCTION()
	void OnFloorChanged(AActor* NewFloorActor);

	UFUNCTION()
	void OnJumpStart();

	UFUNCTION()
	void OnJumpEnd();


	UFUNCTION()
	void OnMontageEnded(class UAnimMontage* Montage, bool bInterrupted);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> DeadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> AttackHeadbuttMontage;

	UPROPERTY(VisibleAnywhere, Category=Input)
	FRotator LastInputDir;

	UPROPERTY(EditAnywhere, Category = Input)
	uint32 bIsDead : 1;

	UPROPERTY(EditAnywhere, Category = Input)
	uint32 bIsAttacking:1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Stat, Meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UFGCharacterStatComponent> CharacterStatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Widget, Meta=(AllowPrivateAccess="true"))
	TObjectPtr<class UFGWidgetComponent> HpBar;

protected:
	virtual void SetCharacterControlData(const class UFGCharacterControlData* CharacterControlData);
	
	UPROPERTY(EditAnywhere, Category=CharacterControl, Meta=(AllowPrivateAccess="true"))
	TMap<EViewMode, TObjectPtr<class UFGCharacterControlData>> ViewModeControlMap;

public:
	virtual void Shoot();
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

	UPROPERTY(EditAnywhere, Category = Gameplay)
	TSubclassOf<class AFGBullet> BulletClass;

	UPROPERTY(EditAnywhere, Category = Gameplay)
	double BulletCountPerSec;

	UPROPERTY(EditAnywhere, Category = Gameplay)
	double LastBulletShoot;
};
