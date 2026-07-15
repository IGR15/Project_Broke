// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MO_RaceTrack.generated.h"

class USplineComponent;

/**
 * Track centerline. Place one per race level and shape the closed-loop spline
 * along the course; on server BeginPlay it hands the spline and lap count to
 * AMO_RaceGameState, which uses it to rank racers by distance.
 */
UCLASS()
class MOCAP_1_API AMO_RaceTrack : public AActor
{
	GENERATED_BODY()

public:
	AMO_RaceTrack();

	UFUNCTION(BlueprintPure, Category="Race")
	FORCEINLINE USplineComponent* GetTrackSpline() const { return TrackSpline; }

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Race")
	TObjectPtr<USplineComponent> TrackSpline;

	// Authoring aid: when it has 3+ entries, the spline is rebuilt from these
	// local-space points (closed loop, auto tangents) on construction. Clear
	// the array to hand-edit the spline in the viewport instead.
	UPROPERTY(EditAnywhere, Category="Race")
	TArray<FVector> SplinePointOverrides;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Race", meta=(ClampMin=1))
	int32 TotalLaps = 3;
};
