// Fill out your copyright notice in the Description page of Project Settings.


#include "FGCharacterBase.h"
#include "FGCollision.h"
#include "FGAnimInstanceCharacter.h"
#include "FGStaticMeshFloor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "FGCharacterMovementComponent.h"
#include "Engine/DamageEvents.h"
#include "FGCharacterStatComponent.h"
#include "FGWidgetComponent.h"
#include "FGHPBarWidget.h"
#include "FGCharacterControlData.h"
#include "FGBullet.h"

AFGCharacterBase::AFGCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UFGCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = false;

	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	UCapsuleComponent* Capsule = GetCapsuleComponent();
	Capsule->InitCapsuleSize(42.f, 96.f);
	Capsule->SetCollisionProfileName(TEXT("FGCharacter"));

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (Movement != nullptr) {
		Movement->bOrientRotationToMovement = true;
		Movement->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
		Movement->JumpZVelocity = 700.f;
		Movement->AirControl = 0.35;
		Movement->MaxWalkSpeed = 500.f;
		Movement->MinAnalogWalkSpeed = 20.f;

		Movement->GroundFriction = DefaultGroundFriction;
		Movement->BrakingFrictionFactor = DefaultBreakingFrictionFactor;
		Movement->BrakingDecelerationWalking = DefaultBreakingDecelerationWalking;
	}

	USkeletalMeshComponent* MeshComponent = GetMesh();
	if (MeshComponent != nullptr) {
		MeshComponent->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.f), FRotator(0.0f, -90.0f, 0.0f));

		// Load SkeletaMesh
		static ConstructorHelpers::FObjectFinder<USkeletalMesh> SKConstructor(TEXT("/Game/Characters/SmolIna/SK_SmolIna.SK_SmolIna"));
		if (SKConstructor.Succeeded()) {
			MeshComponent->SetSkeletalMesh(SKConstructor.Object);
		}

		// Set ABP
		MeshComponent->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		static ConstructorHelpers::FClassFinder<UAnimInstance> ABPConstructor(TEXT("/Game/Characters/SmolIna/ABP_SmolIna.ABP_SmolIna_C"));
		if (ABPConstructor.Succeeded()) {
			MeshComponent->SetAnimInstanceClass(ABPConstructor.Class);
		}
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> M_AttackHeadbutt(TEXT("/Game/Characters/Animation/M_AttackHeadbutt.M_AttackHeadbutt"));
	if (M_AttackHeadbutt.Succeeded())
	{
		AttackHeadbuttMontage = M_AttackHeadbutt.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> M_Dead(TEXT("/Game/Characters/Animation/M_Dead.M_Dead"));
	if (M_Dead.Succeeded())
	{
		DeadMontage = M_Dead.Object;
	}

	bIsAttacking = false;

	CharacterStatComponent = CreateDefaultSubobject<UFGCharacterStatComponent>(TEXT("Stat"));

	HpBar = CreateDefaultSubobject<UFGWidgetComponent>(TEXT("Widget"));
	HpBar->SetupAttachment(GetMesh());
	HpBar->SetRelativeLocation(FVector(0.0f, 0.0f, 250.0f));
	static ConstructorHelpers::FClassFinder<UUserWidget> WBP_HpBar(TEXT("/Game/Characters/UI/WBP_HPBar.WBP_HPBar_C"));
	if (!!WBP_HpBar.Class) {
		HpBar->SetWidgetClass(WBP_HpBar.Class);
		HpBar->SetWidgetSpace(EWidgetSpace::Screen);
		HpBar->SetDrawSize(FVector2D(150.0f, 15.0f));
		HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	static ConstructorHelpers::FObjectFinder<UFGCharacterControlData> FGC_NormalView(TEXT("/Game/Data/FGC_NormalView.FGC_NormalView"));
	ensure(FGC_NormalView.Succeeded());
	ViewModeControlMap.Add(EViewMode::NormalView, FGC_NormalView.Object);

	static ConstructorHelpers::FObjectFinder<UFGCharacterControlData> FGC_ShootView(TEXT("/Game/Data/FGC_ShootView.FGC_ShootView"));
	ensure(FGC_ShootView.Succeeded());
	ViewModeControlMap.Add(EViewMode::ShootView, FGC_ShootView.Object);

	BulletClass = AFGBullet::StaticClass();
	BulletCountPerSec = 10.0;
	LastBulletShoot = 0.0;
	MuzzleOffset.Set(250.0f, 0.0f, 0.0f);
	bIsDead = false;
}

// Called when the game starts or when spawned
void AFGCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	UFGCharacterMovementComponent* MovementComponent = Cast<UFGCharacterMovementComponent>(GetCharacterMovement());
	if (MovementComponent != nullptr) {
		MovementComponent->OnFloorChanged.AddDynamic(this, &AFGCharacterBase::OnFloorChanged);
	}

	UFGAnimInstanceCharacter* AnimInstance = Cast<UFGAnimInstanceCharacter>(GetMesh()->GetAnimInstance());
	if (AnimInstance != nullptr) {
		AnimInstance->OnAnimJumpStart.AddDynamic(this, &AFGCharacterBase::OnJumpStart);
		AnimInstance->OnAnimJumpEnd.AddDynamic(this, &AFGCharacterBase::OnJumpEnd);
		AnimInstance->OnMontageEnded.AddDynamic(this, &AFGCharacterBase::OnMontageEnded);
	}
}

void AFGCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UFGCharacterMovementComponent* MovementComponent = Cast<UFGCharacterMovementComponent>(GetCharacterMovement());
	if (MovementComponent != nullptr) {
		MovementComponent->OnFloorChanged.RemoveAll(this);
	}

	UFGAnimInstanceCharacter* AnimInstance = Cast<UFGAnimInstanceCharacter>(GetMesh()->GetAnimInstance());
	if (AnimInstance != nullptr) {
		AnimInstance->OnAnimJumpStart.RemoveAll(this);
		AnimInstance->OnAnimJumpEnd.RemoveAll(this);
		AnimInstance->OnMontageEnded.RemoveAll(this);
	}
}

void AFGCharacterBase::SetFrictionInfoOnCurrentFloor()
{
	bool bResetFrictionFactor{ true };

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (MovementComponent->IsWalking()) {
		const FHitResult& FloorHit = MovementComponent->CurrentFloor.HitResult;
		AFGStaticMeshFloor* Floor = Cast<AFGStaticMeshFloor>(FloorHit.GetActor());
		if (Floor != nullptr) {
			MovementComponent->GroundFriction = Floor->GetGroundFriction();
			MovementComponent->BrakingFrictionFactor = Floor->GetBreakingFrictionFactor();
			MovementComponent->BrakingDecelerationWalking = Floor->GetBreakingDecelerationWalking();
			bResetFrictionFactor = false;
		}
	}

	if (bResetFrictionFactor) {
		MovementComponent->GroundFriction = DefaultGroundFriction;
		MovementComponent->BrakingFrictionFactor = DefaultBreakingFrictionFactor;
		MovementComponent->BrakingDecelerationWalking = DefaultBreakingDecelerationWalking;
	}
}

void AFGCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	CharacterStatComponent->OnHpZero.AddUObject(this, &AFGCharacterBase::SetDead);
}

void AFGCharacterBase::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	SetFrictionInfoOnCurrentFloor();
}

void AFGCharacterBase::AttackHitCheck()
{
	UCapsuleComponent* Capsule = GetCapsuleComponent();
	if (Capsule == nullptr)
		return;

	TArray<FHitResult> OutHitResults;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(AttackHeadbutt), false, this);

	constexpr float AttackDamage = 30;
	constexpr float AttackLength = 250;
	constexpr float AttackWidthRate = 2.5f;
	const float AttackWidth = Capsule->GetScaledCapsuleRadius() * AttackWidthRate;
	const float AttackHeight = Capsule->GetScaledCapsuleHalfHeight();
	const FVector StartPos = GetActorLocation();
	FVector AttackVector = GetActorForwardVector();
	AttackVector.Z = 0;
	AttackVector.Normalize();
	const FVector EndPos = StartPos + AttackVector * AttackLength;

	bool bHitDetected = GetWorld()->SweepMultiByChannel(OutHitResults, StartPos, EndPos, FQuat::Identity, CCHANNEL_FGATTACK, FCollisionShape::MakeSphere(AttackWidth), Params );
	// GetWorld()->SweepMultiByChannel(OutHitResults, StartPos, EndPos, FQuat::Identity, CCHANNEL_FGATTACK, FCollisionShape::MakeCapsule(AttackWidth, AttackHeight), Params);
	if (bHitDetected) {
		TSet<uint32> DamagedActors;
		FDamageEvent DamageEvent;
		for (const FHitResult& HitResult : OutHitResults) {
			uint32 ActorID = HitResult.GetActor()->GetUniqueID();
			if (DamagedActors.Contains(ActorID))
				continue;
			HitResult.GetActor()->TakeDamage(AttackDamage, DamageEvent, GetController(), this);
			DamagedActors.Add(ActorID);
		}
	}

#if ENABLE_DRAW_DEBUG

	// Ä¸½¶ Áß½É °è»ê
	FVector CapsuleCenter = (StartPos + EndPos) * 0.5f;

	// Ä¸½¶ ¹æÇâ (End - Start)
	FVector CapsuleDirection = EndPos - StartPos;
	float CapsuleHalfHeight = CapsuleDirection.Size() * 0.5f;

	DrawDebugCapsule(GetWorld(), CapsuleCenter, CapsuleHalfHeight, AttackWidth, FQuat::FindBetweenNormals(FVector::UpVector, CapsuleDirection.GetSafeNormal()), // Ä¸½¶ È¸Àü
		bHitDetected ? FColor::Red : FColor::Green, false, 1.0f);
	DrawDebugLine(GetWorld(), StartPos, EndPos, FColor::Blue, false, 1.0f, 0, 1.0f);

	// È÷Æ®µÈ ¾×ÅÍ µð¹ö±× Ãâ·Â
	if (bHitDetected)
	{
		for (const FHitResult& HitResult : OutHitResults)
		{
			DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.0f, 12, FColor::Yellow, false, 1.0f);
		}
	}

#endif
}

float AFGCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	CharacterStatComponent->ApplyDamage(DamageAmount, DamageCauser, EDamageType::None);
	return DamageAmount;
}

void AFGCharacterBase::SetDead()
{
	if (bIsDead)
		return;

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (Movement != nullptr)
	{
		Movement->SetMovementMode(EMovementMode::MOVE_None);
	}
	PlayDeadAnim();
	bIsDead = true;
}

void AFGCharacterBase::SetupCharacterWidget(UFGUserWidget* Widget)
{
	UFGHPBarWidget* HpBarWidget = Cast<UFGHPBarWidget>(Widget);
	if (!!HpBarWidget && !!CharacterStatComponent) {
		HpBarWidget->SetMaxHp(CharacterStatComponent->GetMaxHp());
		HpBarWidget->UpdateHpBar(CharacterStatComponent->GetCurrentHp());
		CharacterStatComponent->OnHpChanged.AddUObject(HpBarWidget, &UFGHPBarWidget::UpdateHpBar);
	}
}

void AFGCharacterBase::PlayDeadAnim()
{
	UFGAnimInstanceCharacter* AnimInstance = Cast<UFGAnimInstanceCharacter>(GetMesh()->GetAnimInstance());
	if (AnimInstance == nullptr)
		return;

	AnimInstance->StopAllMontages(0.0f);
	AnimInstance->Montage_Play(DeadMontage);
}

bool AFGCharacterBase::IsImmovable() const
{
	if (bIsAttacking)
		return true;
	
	UFGAnimInstanceCharacter* AnimInstance = Cast<UFGAnimInstanceCharacter>(GetMesh()->GetAnimInstance());
	if (AnimInstance == nullptr)
		return true;
	if (AnimInstance->IsFalling() || AnimInstance->IsReadyToJump())
		return true;

	return false;
}

void AFGCharacterBase::ReadyToJump()
{
	if (IsImmovable())
		return;

	UFGAnimInstanceCharacter* AnimInstance = Cast<UFGAnimInstanceCharacter>(GetMesh()->GetAnimInstance());
	if (AnimInstance != nullptr) {
		AnimInstance->ReadyToJump();
	}
}

void AFGCharacterBase::Look(const FInputActionValue& Value)
{
	FVector2D LookValue = Value.Get<FVector2D>();
	AddControllerYawInput(LookValue.X);
	AddControllerPitchInput(LookValue.Y);
}

void AFGCharacterBase::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	FVector DesiredDirection = ForwardDirection * MovementVector.Y + RightDirection * MovementVector.X;
	LastInputDir = DesiredDirection.Rotation();

	if (IsImmovable())
		return;

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void AFGCharacterBase::AttackHeadbutt()
{
	if (bIsAttacking)
		return;

	UFGAnimInstanceCharacter* AnimInstance = Cast<UFGAnimInstanceCharacter>(GetMesh()->GetAnimInstance());
	if (AnimInstance == nullptr)
		return;

	if (AnimInstance->Montage_IsPlaying(AttackHeadbuttMontage))
		return;

	SetActorRotation(LastInputDir);
	AnimInstance->Montage_PlayWithBlendIn(AttackHeadbuttMontage, FAlphaBlendArgs(0.2f));
	bIsAttacking = true;
}

void AFGCharacterBase::OnFloorChanged(AActor* NewFloorActor)
{
	SetFrictionInfoOnCurrentFloor();
}

void AFGCharacterBase::OnJumpStart()
{
	Jump();
}

void AFGCharacterBase::OnJumpEnd()
{
	return;
}

void AFGCharacterBase::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == nullptr)
		return;

	if (Montage->GetFName() == FName("M_AttackHeadbutt")) {
		bIsAttacking = false;
	}
}

void AFGCharacterBase::SetCharacterControlData(const UFGCharacterControlData* CharacterControlData)
{
	ensure(!!CharacterControlData);

	bUseControllerRotationYaw = CharacterControlData->bUseControllerRotationYaw;

	GetCharacterMovement()->bOrientRotationToMovement = CharacterControlData->bOrientRotationToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation = CharacterControlData->bUseControllerDesiredRotation;
	GetCharacterMovement()->RotationRate = CharacterControlData->RotationRate;
}

void AFGCharacterBase::Shoot()
{
	if (!BulletClass)
		return;
	if (BulletCountPerSec < KINDA_SMALL_NUMBER)
		return;

	const double CurrentTime = FPlatformTime::Seconds();
	const double BulletDelay = 1.0 / BulletCountPerSec;
	if (LastBulletShoot != 0.0 && (CurrentTime - LastBulletShoot) < BulletDelay)
		return;

	// Get the camera transform.
	FVector CameraLocation;
	FRotator CameraRotation;
	GetActorEyesViewPoint(CameraLocation, CameraRotation);

	// Transform MuzzleOffset from camera space to world space.
	FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);

	// Skew the aim to be slightly upwards.
	FRotator MuzzleRotation = CameraRotation;
	MuzzleRotation.Pitch += 10.0f;

	UWorld* World = GetWorld();
	if (!!World)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		// Spawn the projectile at the muzzle.
		AFGBullet* Bullet = World->SpawnActor<AFGBullet>(BulletClass, MuzzleLocation, MuzzleRotation, SpawnParams);
		if (Bullet)
		{
			Bullet->SetAttacker(GetController());
			// Set the projectile's initial trajectory.
			FVector LaunchDirection = MuzzleRotation.Vector();
			Bullet->FireInDirection(LaunchDirection);
		}

		LastBulletShoot = FPlatformTime::Seconds();
	}
}
