#pragma once

class AVR_PlayerPawn; // Forward declaration

#include "CoreMinimal.h"
#include "EHand.h"
#include "Gesture.h"
#include "OculusXRHandComponent.h"
#include "SaveGameGesture.h"
#include "XRHandComponent.generated.h"

UCLASS()
class GESTURERECOGNITION_API UXRHandComponent : public UOculusXRHandComponent
{
	GENERATED_BODY()

	UPROPERTY() TArray<FGesture> Gestures;
	UPROPERTY() FGesture CurrentGesture;
	UPROPERTY() FGesture EmptyGesture;

protected:
	FTimerHandle MemberTimerHandle;
	
	virtual void BeginPlay() override;

public:
	USaveGameGesture* LoadGestures() const;
	void RecordGesture(FString* Name);
	bool DoesGestureExist(USaveGameGesture* SaveGame, const FString* Name);
	void DeleteGesture(const FString* Name);
	std::tuple<bool, FGesture> GetCurrentGesture();
	bool EqualGestures(FGesture* A, FGesture* B);
	void DetectGesture();
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) FString GestureGameSlot = "GestureSlot";
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) float RefreshRate = 0.1f;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) float Threshold = 3.5f;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) bool Debug = true;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) AVR_PlayerPawn* PlayerPawn;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly) EHand HandSide;
};
