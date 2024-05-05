#pragma once

#include "CoreMinimal.h"
#include "Gesture.generated.h"

USTRUCT(BlueprintType)
struct GESTURERECOGNITION_API FGesture
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gesture")
	FString GestureName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gesture")
	TArray<FVector> BoneLocations;
};
