// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gesture.h"
#include "GameFramework/SaveGame.h"
#include "SaveGameGesture.generated.h"

UCLASS()
class GESTURERECOGNITION_API USaveGameGesture : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) TArray<FGesture> Gesture;
	
};
