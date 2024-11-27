// Fill out your copyright notice in the Description page of Project Settings.


#include "FGCharacterPlayer.h"
#include "Camera/CameraComponent.h"
#include "InputMappingContext.h"
#include "FGCharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FGPlayerController.h"
#include "FGCharacterControlData.h"

AFGCharacterPlayer::AFGCharacterPlayer()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;

	static ConstructorHelpers::FObjectFinder<UInputAction> IAMoveConstructor(TEXT("/Game/Input/Actions/IA_Move.IA_Move"));
	if (IAMoveConstructor.Succeeded()) {
		InputActionMove = IAMoveConstructor.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IALookConstructor(TEXT("/Game/Input/Actions/IA_Look.IA_Look"));
	if (IALookConstructor.Succeeded()) {
		InputActionLook = IALookConstructor.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IAJumpConstructor(TEXT("/Game/Input/Actions/IA_Jump.IA_Jump"));
	if (IAJumpConstructor.Succeeded()) {
		InputActionJump = IAJumpConstructor.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IAHeadButtConstructor(TEXT("/Game/Input/Actions/IA_AttackHeadbutt.IA_AttackHeadbutt"));
	if (IAHeadButtConstructor.Succeeded()) {
		InputActionHeadbutt = IAHeadButtConstructor.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IAShootModeConstructor(TEXT("/Game/Input/Actions/IA_ShootMode.IA_ShootMode"));
	if (IAShootModeConstructor.Succeeded()) {
		InputActionShootMode = IAShootModeConstructor.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IAShootConstructor(TEXT("/Game/Input/Actions/IA_Shoot.IA_Shoot"));
	if (IAShootConstructor.Succeeded()) {
		InputActionShoot = IAShootConstructor.Object;
	}

	CurrentViewMode = EViewMode::NormalView;
}

void AFGCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();
	SetViewMode(CurrentViewMode);
}

void AFGCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (IsValid(Input)) {
		Input->BindAction(InputActionJump, ETriggerEvent::Triggered, this, &AFGCharacterBase::ReadyToJump);
		Input->BindAction(InputActionJump, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		Input->BindAction(InputActionLook, ETriggerEvent::Triggered, this, &AFGCharacterBase::Look);
		Input->BindAction(InputActionMove, ETriggerEvent::Triggered, this, &AFGCharacterBase::Move);
		Input->BindAction(InputActionHeadbutt, ETriggerEvent::Triggered, this, &AFGCharacterBase::AttackHeadbutt);
		Input->BindAction(InputActionShootMode, ETriggerEvent::Triggered, this, &AFGCharacterPlayer::SetShootViewMode);
		Input->BindAction(InputActionShootMode, ETriggerEvent::Completed, this, &AFGCharacterPlayer::SetNormalViewMode);
		Input->BindAction(InputActionShoot, ETriggerEvent::Triggered, this, &AFGCharacterBase::Shoot);
	}
}

void AFGCharacterPlayer::SetCharacterControlData(const UFGCharacterControlData* CharacterControlData)
{
	Super::SetCharacterControlData(CharacterControlData);

	SpringArm->TargetArmLength = CharacterControlData->TargetArmLength;
	SpringArm->SetRelativeRotation(CharacterControlData->RelativeRotation);
	SpringArm->bUsePawnControlRotation = CharacterControlData->bUserPawnControlRotation;
	SpringArm->bInheritPitch = CharacterControlData->bInheritPitch;
	SpringArm->bInheritRoll = CharacterControlData->bInheritRoll;
	SpringArm->bInheritYaw = CharacterControlData->bInheritYaw;
	SpringArm->bDoCollisionTest = CharacterControlData->bDoCollisionTest;
	SpringArm->TargetOffset = CharacterControlData->TargetOffset;
}

void AFGCharacterPlayer::SetShootViewMode()
{
	SetViewMode(EViewMode::ShootView);
}

void AFGCharacterPlayer::SetNormalViewMode()
{
	SetViewMode(EViewMode::NormalView);
}

void AFGCharacterPlayer::SetViewMode(EViewMode NewViewMode)
{
	TObjectPtr<UFGCharacterControlData>* NewCharacterControlData = ViewModeControlMap.Find(NewViewMode);
	check(!!NewCharacterControlData);

	SetCharacterControlData(*NewCharacterControlData);
	AFGPlayerController* PlayerController = CastChecked<AFGPlayerController>(GetController());
	if (PlayerController != nullptr) {
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem != nullptr) {
			Subsystem->ClearAllMappings();
			if (!!((*NewCharacterControlData)->InputMappingContext)) {
				Subsystem->AddMappingContext((*NewCharacterControlData)->InputMappingContext, 0);
			}
		}
	}
	CurrentViewMode = NewViewMode;
}
