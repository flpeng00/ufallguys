// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "FGStaticMeshFloor.generated.h"

/**
 * 
 */
UCLASS()
class UFALLGUYS_API AFGStaticMeshFloor : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	AFGStaticMeshFloor();

public:
	float GetGroundFriction() const;
	float GetBreakingFrictionFactor() const;
	float GetBreakingDecelerationWalking() const;

protected:
	UPROPERTY(EditAnywhere, Category = "Floor");
	float GroundFriction;

	UPROPERTY(EditAnywhere, Category = "Floor");
	float BreakingFrictionFactor;

	UPROPERTY(EditAnywhere, Category = "Floor");
	float BreakingDecelerationWalking;
};
