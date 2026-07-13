// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MO_PlayerController.h"

#include "AbilitySystemGlobals.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystem/MO_AbilitySystemComponent.h"
#include "Input/MO_InputComponent.h"
#include "Input/MO_InputConfig.h"

void AMO_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController() || !AbilityMappingContext)
	{
		return;
	}

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(AbilityMappingContext, AbilityMappingContextPriority);
	}
}

void AMO_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Requires DefaultInputComponentClass=MO_InputComponent (set in DefaultInput.ini)
	// or the cast fails and ability input is silently unbound.
	UMO_InputComponent* MO_InputComponent = Cast<UMO_InputComponent>(InputComponent);

	if (!MO_InputComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("AMO_PlayerController: InputComponent is not a UMO_InputComponent - ability input not bound. Check DefaultInputComponentClass in DefaultInput.ini."));
		return;
	}

	if (!InputConfig)
	{
		UE_LOG(LogTemp, Error, TEXT("AMO_PlayerController: no InputConfig assigned - ability input not bound."));
		return;
	}

	MO_InputComponent->BindAbilityActions(InputConfig, this,
		&AMO_PlayerController::AbilityInputTagPressed,
		&AMO_PlayerController::AbilityInputTagReleased,
		&AMO_PlayerController::AbilityInputTagHeld);
}

void AMO_PlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (UMO_AbilitySystemComponent* ASC = GetASC())
	{
		ASC->AbilityInputTagPressed(InputTag);
	}
}

void AMO_PlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (UMO_AbilitySystemComponent* ASC = GetASC())
	{
		ASC->AbilityInputTagReleased(InputTag);
	}
}

void AMO_PlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (UMO_AbilitySystemComponent* ASC = GetASC())
	{
		ASC->AbilityInputTagHeld(InputTag);
	}
}

UMO_AbilitySystemComponent* AMO_PlayerController::GetASC()
{
	if (!MO_AbilitySystemComponent)
	{
		// ASC lives on MO_PlayerState; PlayerState may not exist yet on early input.
		MO_AbilitySystemComponent = Cast<UMO_AbilitySystemComponent>(
			UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState.Get()));
	}

	return MO_AbilitySystemComponent;
}
