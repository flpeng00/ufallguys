// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FGBullet.generated.h"

UCLASS()
class UFALLGUYS_API AFGBullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFGBullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	virtual void SetAttacker(AController* NewAttackerController);
	virtual void FireInDirection(const FVector& ShootDirection);

	// Function that is called when the projectile hits something.
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

protected:
	UPROPERTY(VisibleDefaultsOnly, Category = Bullet)
	TObjectPtr<class USphereComponent> CollisionComponent;

	UPROPERTY(VisibleAnywhere, Category = Movement)
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovementComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Bullet)
	TObjectPtr<class UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, Category = Bullet)
	TWeakObjectPtr<class AController> AttackController;
};
