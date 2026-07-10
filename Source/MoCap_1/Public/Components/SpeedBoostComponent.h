// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpeedBoostComponent.generated.h"

// Fired when a boost starts, is refreshed (bActive=true each time), or ends (bActive=false).
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpeedBoostChanged, bool, bActive, float, Duration);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MOCAP_1_API USpeedBoostComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USpeedBoostComponent();

	virtual void BeginPlay() override;

	// Multiplies the owner's MaxWalkSpeed for Duration seconds, then reverts.
	// Re-triggering while already boosted refreshes the duration without compounding.
	UFUNCTION(BlueprintCallable)
	void ApplySpeedBoost(float SpeedMultiplier, float Duration);

	UFUNCTION(BlueprintCallable)
	bool IsBoosted() const
	{
		return bBoosted;
	}

	// Multiplier to apply on top of any Blueprint-computed max speed (e.g. Gait-based
	// CalculateMaxSpeed). Returns 1.0 when not boosted.
	UFUNCTION(BlueprintCallable)
	float GetCurrentSpeedMultiplier() const
	{
		return bBoosted ? CurrentMultiplier : 1.f;
	}

	UPROPERTY(BlueprintAssignable, Category="Speed")
	FOnSpeedBoostChanged OnSpeedBoostChanged;

private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	bool bBoosted = false;

	UPROPERTY()
	class ACharacter* OwnerCharacter;

	float BaseMaxWalkSpeed = 0.f;

	float CurrentMultiplier = 1.f;

	FTimerHandle BoostTimerHandle;

	void RevertSpeedBoost();
};
