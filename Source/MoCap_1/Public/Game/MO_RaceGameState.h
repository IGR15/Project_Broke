// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MO_RaceGameState.generated.h"

class USplineComponent;
class AMO_PlayerState;

/**
 * Server-authoritative race manager. BP_RaceTrack registers its centerline
 * spline and BP_Checkpoint actors register their index/location on BeginPlay;
 * checkpoint triggers report racer crossings here. This class validates
 * checkpoint order, counts laps, stamps lap times (server clock), and sorts
 * live race positions on a timer — all results land on each racer's
 * AMO_PlayerState replicated fields, which the overlay UI listens to.
 */
UCLASS()
class MOCAP_1_API AMO_RaceGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	// Called by BP_RaceTrack on server BeginPlay; starts race bookkeeping.
	UFUNCTION(BlueprintCallable, Category="Race")
	void RegisterTrack(USplineComponent* InTrackSpline, int32 InTotalLaps);

	// Called by each BP_Checkpoint on server BeginPlay. Index 0 = start/finish.
	UFUNCTION(BlueprintCallable, Category="Race")
	void RegisterCheckpoint(int32 CheckpointIndex, const FVector& WorldLocation);

	// Called by BP_Checkpoint overlap (server). Ignores out-of-order hits.
	UFUNCTION(BlueprintCallable, Category="Race")
	void RacerCrossedCheckpoint(AMO_PlayerState* RacerPS, int32 CheckpointIndex);

	virtual void AddPlayerState(APlayerState* PlayerState) override;

protected:
	void UpdateRacePositions();
	void InitRacer(AMO_PlayerState* RacerPS);
	float GetRacerTotalProgress(const AMO_PlayerState* RacerPS) const;
	// Lazily maps registered checkpoint locations to spline distances.
	bool EnsureCheckpointDistances();

	UPROPERTY()
	TObjectPtr<USplineComponent> TrackSpline;

	UPROPERTY(EditDefaultsOnly, Category="Race")
	float PositionUpdatePeriod = 0.2f;

	int32 TotalLaps = 3;
	// Racers that crossed the final finish line, in finish order; their
	// positions are locked ahead of everyone still racing.
	int32 NumFinished = 0;

	TMap<int32, FVector> CheckpointLocations;
	TArray<float> CheckpointDistances;
	FTimerHandle PositionUpdateTimer;
};
