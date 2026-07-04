// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/LaunchComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

ULaunchComponent::ULaunchComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void ULaunchComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());

	if (OwnerCharacter)
	{
		OwnerCharacter->MovementModeChangedDelegate.AddDynamic(
			this,
			&ULaunchComponent::OnMovementModeChanged);
	}
}

void ULaunchComponent::Launch(const FVector& Velocity)
{
	if (!OwnerCharacter)
		return;

	bObstacleLaunch = true;
	OwnerCharacter->LaunchCharacter(
		Velocity,
		true,
		true);
}

bool ULaunchComponent::IsLaunched() const
{
	return bLaunched;
}

void ULaunchComponent::OnMovementModeChanged(
	ACharacter* Character,
	EMovementMode PrevMovementMode,
	uint8 PreviousCustomMode)
{
	if (Character->GetCharacterMovement()->IsMovingOnGround())
	{
		bObstacleLaunch = false;
	}
}

