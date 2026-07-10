// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SpeedBoostComponent.h"

#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

USpeedBoostComponent::USpeedBoostComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USpeedBoostComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());
}

void USpeedBoostComponent::ApplySpeedBoost(float SpeedMultiplier, float Duration)
{
	if (!OwnerCharacter)
		return;

	UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement();

	if (!Movement)
		return;

	if (!bBoosted)
	{
		BaseMaxWalkSpeed = Movement->MaxWalkSpeed;
	}

	// Kept for characters using a plain CharacterMovementComponent. Characters whose
	// max speed is recomputed each tick from Blueprint (e.g. CalculateMaxSpeed) must
	// read CurrentMultiplier via GetCurrentSpeedMultiplier() instead.
	Movement->MaxWalkSpeed = BaseMaxWalkSpeed * SpeedMultiplier;
	CurrentMultiplier = SpeedMultiplier;
	bBoosted = true;

	OnSpeedBoostChanged.Broadcast(true, Duration);

	GetWorld()->GetTimerManager().SetTimer(
		BoostTimerHandle,
		this,
		&USpeedBoostComponent::RevertSpeedBoost,
		Duration,
		false);
}

void USpeedBoostComponent::RevertSpeedBoost()
{
	if (!OwnerCharacter)
		return;

	if (UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = BaseMaxWalkSpeed;
	}

	bBoosted = false;

	OnSpeedBoostChanged.Broadcast(false, 0.f);
}
