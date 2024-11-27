// Fill out your copyright notice in the Description page of Project Settings.


#include "FGCharacterMovementComponent.h"

UFGCharacterMovementComponent::UFGCharacterMovementComponent()
{
    SlipVelocity = 2500.f;
    LastFloor = INDEX_NONE;
}

void UFGCharacterMovementComponent::PhysWalking(float deltaTime, int32 Iterations)
{
	Super::PhysWalking(deltaTime, Iterations);

    const FHitResult& FloorHit = CurrentFloor.HitResult;
    AActor* CurrentFloorActor = FloorHit.GetActor();
    int32 CurrentFloorID = INDEX_NONE;
    if (CurrentFloorActor != nullptr)
        CurrentFloorID = CurrentFloorActor->GetUniqueID();
    if (LastFloor != CurrentFloorID)
    {
        LastFloor = CurrentFloorID;
        OnFloorChanged.Broadcast(CurrentFloorActor);
    }

    FVector FloorNormal = CurrentFloor.HitResult.ImpactNormal;
    float SlopeAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(FloorNormal, FVector::UpVector)));
    if (SlopeAngle > 10.0f)
    {
        FVector Gravity = FVector(0, 0, GetGravityZ());
        FVector SlideDirection = (Gravity - FVector::DotProduct(Gravity, FloorNormal) * FloorNormal).GetSafeNormal();
        Velocity += (SlideDirection * SlipVelocity * deltaTime);
    }
}
