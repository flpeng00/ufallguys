// Fill out your copyright notice in the Description page of Project Settings.


#include "FGCharacterNonPlayer.h"
#include "FGAIController.h"

AFGCharacterNonPlayer::AFGCharacterNonPlayer()
{
	AIControllerClass = AFGAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}
