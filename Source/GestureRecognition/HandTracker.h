#pragma once

#pragma once
#include "SaveGameHMM.h"
#include "HiddenMarkovModel/hmm.h"

class AVR_PlayerPawn; // Forward declaration
#include "EHand.h"
#include "DataManager.h"
#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "DrawDebugHelpers.h" // For DrawDebugSphere
#include "HandTracker.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStartHandTrackingDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHandTrackingDelegate);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStopHandTrackingDelegate);

DECLARE_LOG_CATEGORY_EXTERN(LogHandTracker, Log, All);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GESTURERECOGNITION_API UHandTracker : public UActorComponent {
	GENERATED_BODY()
	UPROPERTY()
	bool bIsTracking;
	UPROPERTY()
	TArray<FVector> TrackingPoints;
	UPROPERTY()
	FVector StartTrackingPoint;
	UPROPERTY()
	FVector CurrentTrackingPoint;
	UPROPERTY()
	FVector OldTrackingPoint;
	UPROPERTY()
	float Velocity;
	UPROPERTY()
	float MinVelocity = 5;
	UPROPERTY()
	bool Debug = true;
	UPROPERTY()
	UDataManager* DataManager;
	UPROPERTY()
	TArray<FString> ModelNames = {"FORWARD", "J-Move", "UP", "SPIRAL", "CIRCLE", "THREE"};

public:
	vector<HMM*>* HMM_Models = new vector<HMM*>();
	
	UPROPERTY(BlueprintReadWrite)
	double NextPointRange = 5;
	UPROPERTY(BlueprintReadWrite)
	AVR_PlayerPawn* PlayerPawn;
	UPROPERTY(BlueprintReadWrite)
	EHand HandSide;

	UPROPERTY(BlueprintAssignable, Category="Events")
	FStartHandTrackingDelegate OnStartHandTracking;
	UPROPERTY(BlueprintAssignable, Category="Events")
	FHandTrackingDelegate OnHandTracking;
	UPROPERTY(BlueprintAssignable, Category="Events")
	FStopHandTrackingDelegate OnStopHandTracking;


	UFUNCTION(BlueprintCallable)
	void TrackHand();
	UFUNCTION(BlueprintCallable)
	void AddTrackingPoint(FVector& CurrentHandLocation);
	UFUNCTION(BlueprintCallable)
	void TestSequence();
	UFUNCTION(BlueprintCallable)
	void DebugDisplayPoints(TArray<FVector>& DebugPoints) const;

	UHandTracker();

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void HandleStartHandTrackingEvent();
	UFUNCTION(BlueprintCallable)
	void HandleHandTrackingEvent();
	UFUNCTION(BlueprintCallable)
	void HandleStopHandTrackingEvent();

	UFUNCTION(BlueprintCallable)
	float CalculateVelocity(FVector& Previous, FVector& Current, float DeltaTime);

	USaveGameHmm* LoadHMMFromSaveGame(const FString& SaveGameSlot, HMM* Hmm) const;

	void SaveHmmToSaveGame(const FString& SaveGameSlot, HMM* hmm) const;

	UFUNCTION(BlueprintCallable)
	void TrainHMMs();

	UFUNCTION(BlueprintCallable)
	double HMMTestRealTime();
	void LoadHMMs();

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
