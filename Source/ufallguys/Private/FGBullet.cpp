// Fill out your copyright notice in the Description page of Project Settings.


#include "FGBullet.h"
#include "Components/SphereComponent.h"
#include "FGCharacterBase.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AFGBullet::AFGBullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;
	if (!RootComponent) 
	{
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("BulletSceneComponent"));
	}
	if (!CollisionComponent)
	{
		CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		CollisionComponent->SetCollisionProfileName(TEXT("FGBullet"));
		CollisionComponent->OnComponentHit.AddDynamic(this, &AFGBullet::OnHit);
		CollisionComponent->InitSphereRadius(10.0f);
		RootComponent = CollisionComponent;
	}
	if (!ProjectileMovementComponent)
	{
		// Use this component to drive this projectile's movement.
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
		ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
		ProjectileMovementComponent->InitialSpeed = 3000.0f;
		ProjectileMovementComponent->MaxSpeed = 3000.0f;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = true;
		ProjectileMovementComponent->Bounciness = 0.3f;
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	}
	if (!MeshComponent)
	{
		MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh(TEXT("/Game/StarterContent/Props/MaterialSphere.MaterialSphere"));
		if (Mesh.Succeeded())
		{
			MeshComponent->SetStaticMesh(Mesh.Object);
			MeshComponent->SetupAttachment(RootComponent);
			MeshComponent->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
		}
	}

	InitialLifeSpan = 3.0f;
}

// Called when the game starts or when spawned
void AFGBullet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFGBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFGBullet::SetAttacker(AController* NewAttackerController)
{
	AttackController = NewAttackerController;
}

void AFGBullet::FireInDirection(const FVector& ShootDirection)
{
	check(!!ProjectileMovementComponent);
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}

void AFGBullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor)
		return;

	constexpr float BulletDamage = 5;
	AFGCharacterBase* Character = Cast<AFGCharacterBase>(OtherActor);
	if (Character) {
		FDamageEvent DamageEvent;
		Hit.GetActor()->TakeDamage(BulletDamage, DamageEvent, AttackController.Get(), this);
	}
}

