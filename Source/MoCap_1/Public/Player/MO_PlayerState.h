// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "MO_PlayerState.generated.h"

class UAbilitySystemComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChanged, int32 /*StatValue*/);

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

	UFUNCTION(BlueprintPure, Category="Race")
	FORCEINLINE int32 GetPlayerScore() const { return PlayerScore; }

	UFUNCTION(BlueprintPure, Category="Race")
	FORCEINLINE int32 GetCurrentLap() const { return CurrentLap; }

	UFUNCTION(BlueprintPure, Category="Race")
	FORCEINLINE int32 GetTotalLaps() const { return TotalLaps; }

	// Server-only mutators (pickups, lap triggers, game mode).
	UFUNCTION(BlueprintCallable, Category="Race")
	void AddToScore(int32 InScore);

	UFUNCTION(BlueprintCallable, Category="Race")
	void SetCurrentLap(int32 InCurrentLap);

	UFUNCTION(BlueprintCallable, Category="Race")
	void SetTotalLaps(int32 InTotalLaps);

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

	UFUNCTION()
	void OnRep_PlayerScore(int32 OldPlayerScore);

	UFUNCTION()
	void OnRep_CurrentLap(int32 OldCurrentLap);

	UFUNCTION()
	void OnRep_TotalLaps(int32 OldTotalLaps);
};
