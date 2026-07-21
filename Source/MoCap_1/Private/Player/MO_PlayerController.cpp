// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MO_PlayerController.h"

#include "AbilitySystemGlobals.h"
#include "Camera/PlayerCameraManager.h"
#include "EngineUtils.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystem/MO_AbilitySystemComponent.h"
#include "Character/MO_BaseCharacter.h"
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

AActor* AMO_PlayerController::GetAimedRacerTarget(float MaxDistance, float MaxAngleDegrees) const
{
	APawn* MyPawn = GetPawn();
	if (!MyPawn || !PlayerCameraManager)
	{
		return nullptr;
	}

	const FVector CameraLocation = PlayerCameraManager->GetCameraLocation();
	const FVector CameraForward = PlayerCameraManager->GetCameraRotation().Vector();
	const float BestAngleCosThreshold = FMath::Cos(FMath::DegreesToRadians(MaxAngleDegrees));

	AActor* BestTarget = nullptr;
	float BestAngleCos = BestAngleCosThreshold;

	for (TActorIterator<AMO_BaseCharacter> It(GetWorld()); It; ++It)
	{
		AMO_BaseCharacter* Candidate = *It;
		if (!Candidate || Candidate == MyPawn)
		{
			continue;
		}

		const FVector ToCandidate = Candidate->GetActorLocation() - CameraLocation;
		const float Distance = ToCandidate.Size();
		if (Distance <= KINDA_SMALL_NUMBER || Distance > MaxDistance)
		{
			continue;
		}

		const float AngleCos = FVector::DotProduct(CameraForward, ToCandidate / Distance);
		if (AngleCos < BestAngleCos)
		{
			continue;
		}

		FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(AimedRacerTargetTrace), false, MyPawn);
		TraceParams.AddIgnoredActor(Candidate);

		FHitResult Hit;
		const bool bBlocked = GetWorld()->LineTraceSingleByChannel(
			Hit, CameraLocation, Candidate->GetActorLocation(), ECC_Visibility, TraceParams);
		if (bBlocked)
		{
			continue;
		}

		BestAngleCos = AngleCos;
		BestTarget = Candidate;
	}

	return BestTarget;
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
