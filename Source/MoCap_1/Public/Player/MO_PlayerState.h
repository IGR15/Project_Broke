// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "MO_PlayerState.generated.h"

class UAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChanged, int32 /*StatValue*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStateFloatChanged, float /*StatValue*/);

/**
 * PlayerState owning the player's AbilitySystemComponent (Aura-style placement),
 * so abilities survive pawn death/respawn. Also holds the replicated race
 * stats (score, laps) the overlay widget controller listens to.
 */
UCLASS()
class MOCAP_1_API AMO_PlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AMO_PlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	FOnPlayerStateChanged OnScoreChangedDelegate;
	FOnPlayerStateChanged OnCurrentLapChangedDelegate;
	FOnPlayerStateChanged OnTotalLapsChangedDelegate;
	FOnPlayerStateChanged OnRacePositionChangedDelegate;
	FOnPlayerStateFloatChanged OnLastLapTimeChangedDelegate;
	FOnPlayerStateFloatChanged OnBestLapTimeChangedDelegate;
	FOnPlayerStateFloatChanged OnLapStartedDelegate;

	UFUNCTION(BlueprintPure, Category="Race")
	FORCEINLINE int32 GetPlayerScore() const { return PlayerScore; }

	UFUNCTION(BlueprintPure, Category="Race")
	FORCEINLINE int32 GetCurrentLap() const { return CurrentLap; }

	UFUNCTION(BlueprintPure, Category="Race")
	FORCEINLINE int32 GetTotalLaps() const { return TotalLaps; }

	UFUNCTION(BlueprintPure, Category="Race")
	FORCEINLINE int32 GetRacePosition() const { return RacePosition; }

	// Lap times are -1 until a lap has been completed / started.
	UFUNCTION(BlueprintPure, Category="Race")
	FORCEINLINE float GetLastLapTime() const { return LastLapTime; }

	UFUNCTION(BlueprintPure, Category="Race")
	FORCEINLINE float GetBestLapTime() const { return BestLapTime; }

	UFUNCTION(BlueprintPure, Category="Race")
	FORCEINLINE float GetLapStartServerTime() const { return LapStartServerTime; }

	// Server-only race-progress bookkeeping, driven by AMO_RaceGameState.
	FORCEINLINE int32 GetNextCheckpointIndex() const { return NextCheckpointIndex; }
	void SetNextCheckpointIndex(int32 InIndex) { NextCheckpointIndex = InIndex; }
	FORCEINLINE bool HasFinishedRace() const { return bFinishedRace; }
	void SetFinishedRace(bool bInFinished) { bFinishedRace = bInFinished; }

	// Server-only mutators (pickups, lap triggers, game mode).
	UFUNCTION(BlueprintCallable, Category="Race")
	void AddToScore(int32 InScore);

	UFUNCTION(BlueprintCallable, Category="Race")
	void SetCurrentLap(int32 InCurrentLap);

	UFUNCTION(BlueprintCallable, Category="Race")
	void SetTotalLaps(int32 InTotalLaps);

	UFUNCTION(BlueprintCallable, Category="Race")
	void SetRacePosition(int32 InPosition);

	// Stamps LastLapTime and improves BestLapTime if beaten.
	UFUNCTION(BlueprintCallable, Category="Race")
	void RecordLapTime(float InLapTime);

	// Marks the start of a new lap; clients render the running lap timer
	// locally as GetServerWorldTimeSeconds() - LapStartServerTime.
	UFUNCTION(BlueprintCallable, Category="Race")
	void StartLap(float InServerTime);

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

private:
	// Named PlayerScore/OnRep_PlayerScore because APlayerState already owns a
	// float Score with its own OnRep.
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_PlayerScore)
	int32 PlayerScore = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_CurrentLap)
	int32 CurrentLap = 1;

	UPROPERTY(EditAnywhere, ReplicatedUsing=OnRep_TotalLaps)
	int32 TotalLaps = 3;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_RacePosition)
	int32 RacePosition = 0;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_LastLapTime)
	float LastLapTime = -1.f;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_BestLapTime)
	float BestLapTime = -1.f;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_LapStartServerTime)
	float LapStartServerTime = -1.f;

	// Server-only: checkpoint this racer must cross next (0 = start/finish).
	int32 NextCheckpointIndex = 1;
	bool bFinishedRace = false;

	UFUNCTION()
	void OnRep_PlayerScore(int32 OldPlayerScore);

	UFUNCTION()
	void OnRep_CurrentLap(int32 OldCurrentLap);

	UFUNCTION()
	void OnRep_TotalLaps(int32 OldTotalLaps);

	UFUNCTION()
	void OnRep_RacePosition(int32 OldRacePosition);

	UFUNCTION()
	void OnRep_LastLapTime(float OldLastLapTime);

	UFUNCTION()
	void OnRep_BestLapTime(float OldBestLapTime);

	UFUNCTION()
	void OnRep_LapStartServerTime(float OldLapStartServerTime);
};
