#include "VR_PlayerPawn.h"
#include "HandTracker.h"
#include "XRHandComponent.h"

AVR_PlayerPawn::AVR_PlayerPawn() {
	PrimaryActorTick.bCanEverTick = true;
}

void AVR_PlayerPawn::BeginPlay() {
	Super::BeginPlay();
	OnGestureRecognized.AddDynamic(this, &AVR_PlayerPawn::HandleGestureRecognizedEvent);
}

FVector AVR_PlayerPawn::GetPositionHand(const EHand& Hand) const {
	switch (Hand) {
	case EHand::Left:
		return GetPositionLeftHand();
	case EHand::Right:
		return GetPositionRightHand();

	default:
		throw std::invalid_argument("Unexpected enum value: Must be Left or Right");
	}
}

void AVR_PlayerPawn::HandleGestureRecognizedEvent(FString& Name, EHand& Hand) {
	this->HandSide = Hand;
	if (Hand == EHand::Right) {
		if (Name.Equals(TEXT("Fist"), ESearchCase::IgnoreCase)) {
			if (!BIsTracking) {
				HandTracker->OnStartHandTracking.Broadcast();
				BIsTracking = true;
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Fist Recognized"));
			}
		}
		else if (Name.Equals(TEXT("Null"), ESearchCase::IgnoreCase)) {
			if (BIsTracking) {
				HandTracker->OnStopHandTracking.Broadcast();
				BIsTracking = false;
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Null Recognized"));
			}
		}
	}
}

void AVR_PlayerPawn::HandleGestureReleasedEvent() {
}

void AVR_PlayerPawn::OnCPressed() {
	XRHandRight->DeleteGesture(&GestureName);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("Delete Gesture"));
}

void AVR_PlayerPawn::OnLPressed() {
	XRHandLeft->RecordGesture(&GestureName);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("Record Left"));
}

void AVR_PlayerPawn::OnRPressed() {
	XRHandRight->RecordGesture(&GestureName);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("Record Right"));
}

void AVR_PlayerPawn::Tick(float DeltaTime) { Super::Tick(DeltaTime); }

void AVR_PlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}