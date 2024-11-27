// Fill out your copyright notice in the Description page of Project Settings.


#include "FGGameModeBase.h"
#include "FGCharacterPlayer.h"
#include "FGPlayerController.h"

AFGGameModeBase::AFGGameModeBase()
{
	DefaultPawnClass = AFGCharacterPlayer::StaticClass();
	PlayerControllerClass = AFGPlayerController::StaticClass();
}
