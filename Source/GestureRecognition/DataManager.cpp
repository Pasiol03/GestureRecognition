// Fill out your copyright notice in the Description page of Project Settings.

#include "DataManager.h"
#include <fstream>
#include <string>

UDataManager::UDataManager() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UDataManager::BeginPlay() {
	Super::BeginPlay();
	SequenceName = "Sequence_Fig7";
}

void UDataManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UDataManager::GenerateSpherePoints() {
	for (int i = 0; i < NumSegments; ++i) {
		for (int j = 0; j < NumRows; ++j) {
			const float Theta = 2 * PI * i / NumSegments;
			const float Phi = PI * j / NumRows;

			float X = FMath::Sin(Phi) * FMath::Cos(Theta);
			float Y = FMath::Sin(Phi) * FMath::Sin(Theta);
			float Z = FMath::Cos(Phi);

			SpherePoints.Add(FVector(X, Y, Z));
		}
	}
}

FVector UDataManager::CalculateNormalizedVector(const FVector& PointB, const FVector& PointA) {
	const FVector VectorAB = PointB - PointA;
	const float VectorLength = VectorAB.Size();
	const FVector NormalizedVectorAB = VectorLength > SMALL_NUMBER ? VectorAB / VectorLength : FVector::ZeroVector;
	return NormalizedVectorAB;
}

void UDataManager::FindTwinVector(const FVector& NormalizedAB) {
	TwinNormalVector = FVector::ZeroVector;
	float HighestDotProduct = -FLT_MAX;
	uint8 ID = 0;
	int Ctr = 0;

	for (const FVector& NormalVector : SpherePoints) {
		const float DotProduct = FVector::DotProduct(NormalVector, NormalizedAB);

		if (DotProduct > HighestDotProduct) {
			HighestDotProduct = DotProduct;
			TwinNormalVector = NormalVector;
			ID = Ctr;
		}
		++Ctr;
	}
	DebugTwinVector(NormalizedAB);
	WriteToSequence(ID);
}

void UDataManager::WriteToSequence(const uint8 ID) {
	Sequence.Add(ID);
}

void UDataManager::ConvertSequenceToString() {
	if (Sequence.Num() > 0) {
		DataStream.clear();
		DataStream.resize(Sequence.Num());
		std::copy_n(Sequence.GetData(), Sequence.Num(), DataStream.begin());
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, DataStream.data());
	}
}

void UDataManager::SaveSequence() {
	ConvertSequenceToString();
	const std::string FilePath = "F:/Unreal/GestureRecognition/Source/GestureRecognition/HiddenMarkovModel/data/Sequence_Fig7.txt";

	std::ifstream FileCheck(FilePath);
	if (FileCheck.good()) {
		FileCheck.close();
		std::ofstream File(FilePath, std::ios_base::app | std::ios::binary); // Specify ios::binary for Windows
		if (File.is_open()) {
			File << "\n"; // Add a line break
			File << DataStream;
			File.close();
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Data saved to existing file."));
		} else {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Error opening existing file."));
		}
	} else {
		std::ofstream File(FilePath, std::ios::binary); // Specify ios::binary for Windows
		if (File.is_open()) {
			File << DataStream;
			File << "\n"; // Add a line break
			File.close();
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Data saved to new file."));
		} else {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Error creating new file."));
		}
	}
}

void UDataManager::PostSavingSequence() {
	Sequence.Empty();
}

void UDataManager::DebugTwinVector(const FVector& NormalizedAB) {
	if (Debug) {
		for (const FVector& SpherePoint : SpherePoints) {
			DrawDebugSphere(GetWorld(), SpherePoint * 100.0f, 2.0f, 4, FColor::Green, false, 1, 0, .3f);
		}

		//NormalizedVector
		DrawDebugLine(GetWorld(), FVector::ZeroVector, NormalizedAB * 100.0f, FColor::Red, false, 1, 0, 1);

		//TwinVector
		DrawDebugLine(GetWorld(), FVector::ZeroVector, TwinNormalVector * 100.0f, FColor::Blue, false, 1, 0, 1);
	}
}
