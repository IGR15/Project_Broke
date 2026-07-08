// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SpeedBoostComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

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

	Movement->MaxWalkSpeed = BaseMaxWalkSpeed * SpeedMultiplier;
	bBoosted = true;

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
}
