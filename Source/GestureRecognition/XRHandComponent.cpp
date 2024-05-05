#include "XRHandComponent.h"
#include "VR_PlayerPawn.h"
#include "Kismet/GameplayStatics.h"

void UXRHandComponent::BeginPlay() {
	Super::BeginPlay();
	Gestures = LoadGestures()->Gesture;
	EmptyGesture.GestureName = "Null";
	CurrentGesture = EmptyGesture;
	GetWorld()->GetTimerManager().SetTimer(MemberTimerHandle, this, &UXRHandComponent::DetectGesture, RefreshRate, true,
	                                       0.0f);
}

USaveGameGesture* UXRHandComponent::LoadGestures() const {
	USaveGameGesture* SaveGame;
	if (UGameplayStatics::DoesSaveGameExist(GestureGameSlot, 0)) {
		SaveGame = Cast<USaveGameGesture>(UGameplayStatics::LoadGameFromSlot(GestureGameSlot, 0));
		return SaveGame;
	}
	SaveGame = Cast<USaveGameGesture>(UGameplayStatics::CreateSaveGameObject(USaveGameGesture::StaticClass()));
	return SaveGame;
}

void UXRHandComponent::RecordGesture(FString* Name) {
	if (USaveGameGesture* SaveGame = LoadGestures();
		!DoesGestureExist(SaveGame, &GestureGameSlot)) {
		TArray<FVector> BoneLocations;
		for (int i = 0; i < GetNumBones() - 1; ++i) {
			BoneLocations.Add(GetBoneLocationByName(GetBoneName(i), EBoneSpaces::ComponentSpace));
		}
		FGesture HandGesture;
		HandGesture.GestureName = *Name;
		HandGesture.BoneLocations = BoneLocations;
		SaveGame->Gesture.Add(HandGesture);
		UGameplayStatics::SaveGameToSlot(SaveGame, GestureGameSlot, 0);
	}
}

bool UXRHandComponent::DoesGestureExist(USaveGameGesture* SaveGame, const FString* Name) {
	for (auto [GestureName, BoneLocations] : SaveGame->Gesture) { if (GestureName.Equals(*Name)) { return true; } }
	return false;
}

void UXRHandComponent::DeleteGesture(const FString* Name) {
	int Index = 0;
	for (USaveGameGesture* SaveGame = LoadGestures(); auto [GestureName, BoneLocations] : SaveGame->Gesture) {
		if (GestureName.Equals(*Name)) {
			SaveGame->Gesture.RemoveAt(Index);
			UGameplayStatics::SaveGameToSlot(SaveGame, GestureGameSlot, 0);
		}
		++Index;
	}
}

std::tuple<bool, FGesture> UXRHandComponent::GetCurrentGesture() {
	bool ValidGesture = true;
	for (FGesture Gesture : Gestures) {
		for (int i = 0; i < Gesture.BoneLocations.Num() - 1; ++i) {
			FVector CurrentBoneLocation = GetBoneLocationByName(GetBoneName(i), EBoneSpaces::ComponentSpace);
			if (abs((CurrentBoneLocation - Gesture.BoneLocations[i]).Length()) > Threshold) { ValidGesture = false; }
		}
		if (ValidGesture) { return std::make_tuple(ValidGesture, Gesture); }
		ValidGesture = true;
	}
	FGesture NullGesture;
	NullGesture.GestureName = "Null";
	return std::make_tuple(ValidGesture, NullGesture);
}

void UXRHandComponent::DetectGesture() {
	if (std::tuple<bool, FGesture> Result = GetCurrentGesture(); std::get<0>(Result)) {
		if (!EqualGestures(&std::get<1>(Result), &CurrentGesture)) {
			CurrentGesture = std::get<1>(Result);
			PlayerPawn->OnGestureRecognized.Broadcast(CurrentGesture.GestureName, HandSide);
		}
	}
	else {
		// GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Orange, TEXT("Detected"));
		PlayerPawn->OnGestureRecognized.Broadcast(CurrentGesture.GestureName, HandSide);
	}
}

bool UXRHandComponent::EqualGestures(FGesture* A, FGesture* B) { return A->GestureName.Equals(B->GestureName); }