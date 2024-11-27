// Fill out your copyright notice in the Description page of Project Settings.


#include "FGStaticMeshFloor.h"
#include "FGCharacterBase.h"

AFGStaticMeshFloor::AFGStaticMeshFloor()
{
	GroundFriction = AFGCharacterBase::DefaultGroundFriction;
	BreakingFrictionFactor = AFGCharacterBase::DefaultBreakingFrictionFactor;
	BreakingDecelerationWalking = AFGCharacterBase::DefaultBreakingDecelerationWalking;
}

float AFGStaticMeshFloor::GetGroundFriction() const
{
	return GroundFriction;
}

float AFGStaticMeshFloor::GetBreakingFrictionFactor() const
{
	return BreakingFrictionFactor;
}

float AFGStaticMeshFloor::GetBreakingDecelerationWalking() const
{
	return BreakingDecelerationWalking;
}
