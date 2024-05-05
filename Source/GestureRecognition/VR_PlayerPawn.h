// Fill out your copyright notice in the Description page of Project Settings.


#pragma once

class UHandTracker;
class UXRHandComponent;
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EHand.h"
#include "Gesture.h"
#include "VR_PlayerPawn.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGestureRecognizedDelegate, FString&, GestureName, EHand&, HandSide);


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGestureReleased, const FGesture&, Gesture);

UCLASS()
class GESTURERECOGNITION_API AVR_PlayerPawn : public APawn {
	GENERATED_BODY()

	

public:
	AVR_PlayerPawn();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HandTracking")
	UHandTracker* HandTracker;

protected:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	bool BIsTracking = false;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void HandleGestureRecognizedEvent(FString& Name, EHand& Hand);
	UFUNCTION(BlueprintCallable)
	void HandleGestureReleasedEvent();

public:
	UPROPERTY(BlueprintAssignable, Category="Events")
	FGestureRecognizedDelegate OnGestureRecognized;
	UPROPERTY(BlueprintAssignable, Category="Events")
	FGestureReleased OnGestureReleased;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	EHand HandSide;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UXRHandComponent* XRHandRight;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	UXRHandComponent* XRHandLeft;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FString GestureName;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FVector PositionLeftHand;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FVector PositionRightHand;
	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION(BlueprintCallable)
	FVector GetPositionLeftHand() const { return PositionLeftHand; }

	UFUNCTION(BlueprintCallable)
	FVector GetPositionRightHand() const { return PositionRightHand; }

	FVector GetPositionHand(const EHand& Hand) const;

	UFUNCTION(BlueprintCallable)
	void OnRPressed();

	UFUNCTION(BlueprintCallable)
	void OnLPressed();

	UFUNCTION(BlueprintCallable)
	void OnCPressed();
};
