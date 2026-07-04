// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LaunchComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MOCAP_1_API ULaunchComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	ULaunchComponent();
	UFUNCTION(BlueprintCallable)
	bool IsObstacleLaunch() const
	{
		return bObstacleLaunch;
	}

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void Launch(const FVector& Velocity);

	UFUNCTION(BlueprintCallable)
	bool IsLaunched() const;

private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	bool bObstacleLaunch = false;

	UPROPERTY()
	class ACharacter* OwnerCharacter;

	bool bLaunched = false;

	UFUNCTION()
	void OnMovementModeChanged(
		ACharacter* Character,
		EMovementMode PrevMovementMode,
		uint8 PreviousCustomMode);
};
