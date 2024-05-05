// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveGameHMM.generated.h"

UCLASS()
class GESTURERECOGNITION_API USaveGameHmm : public USaveGame
{
	GENERATED_BODY()
public:
	TArray<double> Initial;
	TArray<TArray<double>> Transition;
	TArray<TArray<double>> Observation;
	
};
