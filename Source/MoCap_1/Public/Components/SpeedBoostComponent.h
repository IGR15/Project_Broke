// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpeedBoostComponent.generated.h"


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

private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	bool bBoosted = false;

	UPROPERTY()
	class ACharacter* OwnerCharacter;

	float BaseMaxWalkSpeed = 0.f;

	FTimerHandle BoostTimerHandle;

	void RevertSpeedBoost();
};
