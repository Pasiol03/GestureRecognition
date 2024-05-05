#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataManager.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GESTURERECOGNITION_API UDataManager : public UActorComponent {
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDataManager();

protected:
	FVector TwinNormalVector;
	TArray<FVector> SpherePoints;
	TArray<uint8> Sequence;
	std::string DataStream;
	std::string SequenceName;

	UPROPERTY(BlueprintReadWrite)
	bool Debug = true;

	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadWrite)
	int NumSegments;
	UPROPERTY(BlueprintReadWrite)
	int NumRows;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	void GenerateSpherePoints();
	FVector CalculateNormalizedVector(const FVector& PointA, const FVector& PointB);
	void FindTwinVector(const FVector& NormalizedAB);
	void WriteToSequence(uint8 ID);
	void ConvertSequenceToString();
	void SaveSequence();
	void PostSavingSequence();
	void DebugTwinVector(const FVector& NormalizedAB);
	TArray<uint8>* GetSequence() { return &Sequence; }
};
