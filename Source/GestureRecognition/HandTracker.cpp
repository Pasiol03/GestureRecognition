#include "HandTracker.h"
#include "DataManager.h"
#include "VR_PlayerPawn.h"
#include "HiddenMarkovModel/hmm.h"
#include "Kismet/GameplayStatics.h" // For UGameplayStatics
#include "Serialization/ArrayWriter.h"

DEFINE_LOG_CATEGORY(LogHandTracker)


UHandTracker::UHandTracker() {
	PrimaryComponentTick.bCanEverTick = true;
	OnStartHandTracking.AddDynamic(this, &UHandTracker::HandleStartHandTrackingEvent);
	OnHandTracking.AddDynamic(this, &UHandTracker::HandleHandTrackingEvent);
	OnStopHandTracking.AddDynamic(this, &UHandTracker::HandleStopHandTrackingEvent);
}

void UHandTracker::BeginPlay() {
	Super::BeginPlay();
	DataManager = NewObject<UDataManager>(this);
	DataManager->NumRows = 8;
	DataManager->NumSegments = 16;
	DataManager->GenerateSpherePoints();
	TrainHMMs();
	// LoadHMMs();
}

double UHandTracker::HMMTestRealTime() {
	double prop = -1;
	int m = 0;
	int model = 0;
	
	TArray<uint8>* sequence = DataManager->GetSequence();
	if (sequence->Num() > 3) {

	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red,
										 FString::Printf(TEXT("Testing...")));
		vector<uint8>* obs = new vector<uint8>(sequence->Num());

		for (int i = 0; i < sequence->Num(); ++i) { (*obs)[i] = (*sequence)[i]; }

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green,
		                                 FString::Printf(TEXT("Num Models: %llu"), HMM_Models->size()));

		for (HMM* hmm : *HMM_Models) {
			double currentProp = hmm->testHMM(obs);
			if (currentProp > prop) {
				prop = currentProp;
				model = m;
			}

			++m;
		}
		FString text = "Detected Model: ";
		text.Append(ModelNames[model]);
		
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, text);
	}
	return 0;
}

void UHandTracker::LoadHMMs() {
	HMM* hmm1 = new HMM();
	HMM* hmm2 = new HMM();
	HMM* hmm3 = new HMM();
	HMM* hmm4 = new HMM();
	HMM* hmm5 = new HMM();
	HMM* hmm6 = new HMM();

	HMM_Models->push_back(hmm1);
	HMM_Models->push_back(hmm2);
	HMM_Models->push_back(hmm3);
	HMM_Models->push_back(hmm4);
	HMM_Models->push_back(hmm5);
	HMM_Models->push_back(hmm6);
	
	// for(int i = 0; i < HMM_Models->size(); ++i) {
	// 	LoadHMMFromSaveGame(*ModelNames[i], HMM_Models->at(i));
	// }

	LoadHMMFromSaveGame("SaveGameSlotName", hmm1);

	const string dataPath =
		"F:/Unreal/GestureRecognition/Source/GestureRecognition/HiddenMarkovModel/data/testing_data.txt";

	vector<vector<char>> test_data = readData(dataPath);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Data read."));

	// const double acc = test(HMM_Models, &test_data,
							// "F:/Unreal/GestureRecognition/Source/GestureRecognition/HiddenMarkovModel/data/testing_result.txt");
	// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Accuracy: %f"), acc));
}

void UHandTracker::TrainHMMs() {

	double StartTime = FPlatformTime::Seconds();
	
	
	HMM* hmm1 = new HMM();
	HMM* hmm2 = new HMM();
	HMM* hmm3 = new HMM();
	HMM* hmm4 = new HMM();
	HMM* hmm5 = new HMM();
	HMM* hmm6 = new HMM();
	int iteration = 20;

	const char* model_init =
		"F:/Unreal/GestureRecognition/Source/GestureRecognition/HiddenMarkovModel/data/big_model_init.txt";
	const char* modelName1 =
		"F:/Unreal/GestureRecognition/Source/GestureRecognition/HiddenMarkovModel/data/trained/model_01.txt";
	string dataPath1 =
		"F:/Unreal/GestureRecognition/Source/GestureRecognition/HiddenMarkovModel/data/Sequence_Fig1.txt";

	const char* modelName2 =
		"F:/Unreal/GestureRecognition/Source/GestureRecognition/HiddenMarkovModel/data/trained/model_02.txt";
	string dataPath2 =
		"F:/Unreal/GestureRecognition/Source/GestureRecognition/HiddenMarkovModel/data/Sequence_Fig2.txt";

	const char* modelName3 =
		"F:/Unreal/GestureRecognition/Source/GestureRecognition/HiddenMarkovModel/data/trained/model_03.txt";
	string dataPath3 =
		"F:/Unreal/GestureRecognition/Source/GestureRecognition/HiddenMarkovModel/data/Sequence_Fig3.txt";

	const char* modelName4 =
		"F:/Unreal/GestureRecognition/Source/GestureRecognition/HiddenMarkovModel/data/trained/model_04.txt";
	string dataPath4 =
		"F:/Unreal/GestureRecognition/Source/GestureRecognition/HiddenMarkovModel/data/Sequence_Fig4.txt";

	const char* modelName5 =
		"F:/Unreal/GestureRecognition/Source/GestureRecognition/HiddenMarkovModel/data/trained/model_05.txt";
	string dataPath5 =
		"F:/Unreal/GestureRecognition/Source/GestureRecognition/HiddenMarkovModel/data/Sequence_Fig5.txt";

	const char* modelName6 =
		"F:/Unreal/GestureRecognition/Source/GestureRecognition/HiddenMarkovModel/data/trained/model_06.txt";
	string dataPath6 =
		"F:/Unreal/GestureRecognition/Source/GestureRecognition/HiddenMarkovModel/data/Sequence_Fig6.txt";

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Start Training Model 1."));
	hmm1->loadHMM(model_init);
	hmm1->setName(modelName1);
	HMM::train(hmm1, iteration, dataPath1);

	// UE_LOG(LogBlueprint, Warning, TEXT("Initial Vector:"));
	// for (double Value : hmm1->Initial()) {
	// 	UE_LOG(LogBlueprint, Warning, TEXT("%f"), Value);
	// }
	//
	// // Log TransitionMatrix
	// UE_LOG(LogBlueprint, Warning, TEXT("Transition Matrix:"));
	// for (const auto& Row :hmm1->Transition()) {
	// 	FString RowString;
	// 	for (double Value : Row) {
	// 		RowString += FString::Printf(TEXT("%f "), Value);
	// 	}
	// 	UE_LOG(LogBlueprint, Warning, TEXT("%s"), *RowString);
	// }
	//
	// // Log ObservationMatrix
	// UE_LOG(LogBlueprint, Warning, TEXT("Observation Matrix:"));
	// for (const auto& Row :hmm1->Observation()) {
	// 	FString RowString;
	// 	for (double Value : Row) {
	// 		RowString += FString::Printf(TEXT("%f "), Value);
	// 	}
	// 	UE_LOG(LogBlueprint, Warning, TEXT("%s"), *RowString);
	// }
	
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Start Training Model 2."));
	hmm2->loadHMM(model_init);
	hmm2->setName(modelName2);
	HMM::train(hmm2, iteration, dataPath2);
	//
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Start Training Model 3."));
	hmm3->loadHMM(model_init);
	hmm3->setName(modelName3);
	HMM::train(hmm3, iteration, dataPath3);
	
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Start Training Model 4."));
	hmm4->loadHMM(model_init);
	hmm4->setName(modelName4);
	HMM::train(hmm4, iteration, dataPath4);
	
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Start Training Model 5."));
	hmm5->loadHMM(model_init);
	hmm5->setName(modelName5);
	HMM::train(hmm5, iteration, dataPath5);
	
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Start Training Model 6."));
	hmm6->loadHMM(model_init);
	hmm6->setName(modelName6);
	HMM::train(hmm6, iteration, dataPath6);

	double EndTime = FPlatformTime::Seconds();
	double ElapsedTime = EndTime - StartTime;

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("Time to train all: %f"), ElapsedTime));

	
	// HMM::generateTestFiles();

	HMM_Models->push_back(hmm1);
	HMM_Models->push_back(hmm2);
	HMM_Models->push_back(hmm3);
	HMM_Models->push_back(hmm4);
	HMM_Models->push_back(hmm5);
	HMM_Models->push_back(hmm6);

	for(int i = 0; i < HMM_Models->size(); ++i) {
		SaveHmmToSaveGame(ModelNames[i], HMM_Models->at(i));
	}

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Saved Models"));

	// HMM_Models->push_back(hmm3);

	const string dataPath =
		"F:/Unreal/GestureRecognition/Source/GestureRecognition/HiddenMarkovModel/data/testing_data.txt";

	vector<vector<char>> test_data = readData(dataPath);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Data read."));

	const double acc = test(HMM_Models, &test_data,
	                        "F:/Unreal/GestureRecognition/Source/GestureRecognition/HiddenMarkovModel/data/testing_result.txt");
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Accuracy: %f"), acc));
}


USaveGameHmm* UHandTracker::LoadHMMFromSaveGame(const FString& SaveGameSlot, HMM* Hmm) const {
	USaveGameHmm* LoadedSaveGame = Cast<USaveGameHmm>(UGameplayStatics::LoadGameFromSlot(SaveGameSlot, 0));
	if (LoadedSaveGame) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("SaveGameFound"));
		// // Convert TArray to vectors
		// Hmm->Initial().clear();
		// int size = LoadedSaveGame->Initial.Num();
		// for (double Value : LoadedSaveGame->Initial) {
		// 	// FString Message = FString::Printf(TEXT("Value: %f"), Value);
		// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Purple, TEXT("HI"));
		// 	Hmm->Initial().push_back(Value);
		// }
		// FString Message = FString::Printf(TEXT("Initial Size: %d"), size);
		// GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Purple, Message);
		//
		// Hmm->Transition().clear();
		// for (const auto& Row : LoadedSaveGame->Transition) {
		// 	std::vector<double> ConvertedRow;
		// 	for (double Value : Row) { ConvertedRow.push_back(Value); }
		// 	Hmm->Transition().push_back(ConvertedRow);
		// }
		//
		// Hmm->Observation().clear();
		// for (const auto& Row : LoadedSaveGame->Observation) {
		// 	std::vector<double> ConvertedRow;
		// 	for (double Value : Row) { ConvertedRow.push_back(Value); }
		// 	Hmm->Observation().push_back(ConvertedRow);
		// }

		// Log InitialVector
		UE_LOG(LogBlueprint, Warning, TEXT("Initial Vector:"));
		for (double Value : LoadedSaveGame->Initial) {
			UE_LOG(LogBlueprint, Warning, TEXT("%f"), Value);
		}

		// Log TransitionMatrix
		UE_LOG(LogBlueprint, Warning, TEXT("Transition Matrix:"));
		for (const auto& Row :LoadedSaveGame->Transition) {
			FString RowString;
			for (double Value : Row) {
				RowString += FString::Printf(TEXT("%f "), Value);
			}
			UE_LOG(LogBlueprint, Warning, TEXT("%s"), *RowString);
		}

		// Log ObservationMatrix
		// UE_LOG(LogBlueprint, Warning, TEXT("Observation Matrix:"));
		for (const auto& Row :LoadedSaveGame->Observation) {
			FString RowString;
			for (double Value : Row) {
				RowString += FString::Printf(TEXT("%f "), Value);
			}
			UE_LOG(LogBlueprint, Warning, TEXT("%s"), *RowString);
		}

		return LoadedSaveGame;
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("SaveGameFound NOT"));
		return nullptr;
	}
}

void UHandTracker::SaveHmmToSaveGame(const FString& SaveGameSlot, HMM* hmm) const {

	USaveGameHmm* SaveGame = Cast<USaveGameHmm>(UGameplayStatics::CreateSaveGameObject(USaveGameHmm::StaticClass()));
	SaveGame->Initial = {1.0, 2.0, 3.0};
	SaveGame->Transition = {{1.1, 1.2, 1.3}, {2.1, 2.2, 2.3}, {3.1, 3.2, 3.3}};
	SaveGame->Observation = {{0.1, 0.2, 0.3}, {0.4, 0.5, 0.6}, {0.7, 0.8, 0.9}};

	// Save the save game object to a slot
	UGameplayStatics::SaveGameToSlot(SaveGame, "SaveGameSlotName", 0);
	
	USaveGameHmm* SaveGameInstance = Cast<USaveGameHmm>(
		UGameplayStatics::CreateSaveGameObject(USaveGameHmm::StaticClass()));

	// Convert class member vectors to TArray
	TArray<double> InitialTArray;
	for (double Value : hmm->Initial()) { InitialTArray.Add(Value); }
	const FString Message = FString::Printf(TEXT("Initial Size: %d"), InitialTArray.Num());
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Purple, Message);
	SaveGameInstance->Initial = InitialTArray;

	TArray<TArray<double>> TransitionTArray;
	for (const auto& Row : hmm->Transition()) {
		TArray<double> ConvertedRow;
		for (double Value : Row) { ConvertedRow.Add(Value); }
		TransitionTArray.Add(ConvertedRow);
	}
	SaveGameInstance->Transition = TransitionTArray;

	TArray<TArray<double>> ObservationTArray;
	for (const auto& Row : hmm->Observation()) {
		TArray<double> ConvertedRow;
		for (double Value : Row) { ConvertedRow.Add(Value); }
		ObservationTArray.Add(ConvertedRow);
	}
	SaveGameInstance->Observation = ObservationTArray;

	// // Log InitialVector
	// UE_LOG(LogHandTracker, Warning, TEXT("Initial Vector:"));
	// for (double Value : InitialTArray) {
	// 	UE_LOG(LogBlueprint, Warning, TEXT("%f"), Value);
	// }
	//
	// // Log TransitionMatrix
	// UE_LOG(LogBlueprint, Warning, TEXT("Transition Matrix:"));
	// for (const auto& Row :ObservationTArray) {
	// 	FString RowString;
	// 	for (double Value : Row) {
	// 		RowString += FString::Printf(TEXT("%f "), Value);
	// 	}
	// 	UE_LOG(LogBlueprint, Warning, TEXT("%s"), *RowString);
	// }
	//
	// // Log ObservationMatrix
	// UE_LOG(LogBlueprint, Warning, TEXT("Observation Matrix:"));
	// for (const auto& Row :TransitionTArray) {
	// 	FString RowString;
	// 	for (double Value : Row) {
	// 		RowString += FString::Printf(TEXT("%f "), Value);
	// 	}
	// 	UE_LOG(LogBlueprint, Warning, TEXT("%s"), *RowString);
	// }
	
	// Save the data
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameSlot, 0);
}


void UHandTracker::DebugDisplayPoints(TArray<FVector>& DebugPoints) const {
	const UWorld* World = GetWorld();
	for (auto Point : DebugPoints) {
		DrawDebugSphere(World, Point, 1.0, 8, FColor::Blue, false, 1.0f, 0, 0.1f);
	}
}


void UHandTracker::TrackHand() {
	CurrentTrackingPoint = PlayerPawn->GetPositionHand(HandSide);
	const float Distance = (CurrentTrackingPoint - OldTrackingPoint).Length();
	AddTrackingPoint(CurrentTrackingPoint);
}

void UHandTracker::AddTrackingPoint(FVector& CurrentHandLocation) {
	const float Length = (CurrentTrackingPoint - OldTrackingPoint).Length();
	const FString DebugMessage = FString::Printf(TEXT("Length: %f"), Length);
	GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Green, DebugMessage);
	if (Length > NextPointRange) {
		GEngine->AddOnScreenDebugMessage(-1, 4, FColor::Green, TEXT("Add Point"));
		TrackingPoints.Add(CurrentHandLocation);

		//DataManger Stuff
		const FVector NormalizedAB = DataManager->CalculateNormalizedVector(CurrentTrackingPoint, OldTrackingPoint);
		DataManager->FindTwinVector(NormalizedAB);

		OldTrackingPoint = CurrentHandLocation;
	}
}

void UHandTracker::TestSequence() {
}

void UHandTracker::HandleStartHandTrackingEvent() {
	StartTrackingPoint = PlayerPawn->GetPositionHand(HandSide);
	OldTrackingPoint = PlayerPawn->GetPositionHand(HandSide);
	TrackingPoints.Add(StartTrackingPoint);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Start Tracking Fist"));
	OnHandTracking.Broadcast();
}

void UHandTracker::HandleHandTrackingEvent() {
	bIsTracking = true;
	OldTrackingPoint = PlayerPawn->GetPositionHand(HandSide);
}

void UHandTracker::HandleStopHandTrackingEvent() {
	TrackingPoints.Empty();
	bIsTracking = false;
	DataManager->SaveSequence();
	HMMTestRealTime();
	// TestSequence();
	DataManager->PostSavingSequence();
}

float UHandTracker::CalculateVelocity(FVector& Previous, FVector& Current, float DeltaTime) {
	return abs((Current - Previous).Length() / DeltaTime);
}

void UHandTracker::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsTracking) {
		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Green, TEXT("Tracking..."));
		Velocity = CalculateVelocity(OldTrackingPoint, CurrentTrackingPoint, DeltaTime);
		TrackHand();
		DebugDisplayPoints(TrackingPoints);
	}
}
