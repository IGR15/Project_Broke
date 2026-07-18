// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MO_BaseCharacter.h"

#include "AbilitySystemComponent.h"
#include "Character/MO_CharacterMovementComponent.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputComponent.h"
#include "Components/LaunchComponent.h"
#include "Components/SpeedBoostComponent.h"
#include "GameFramework/PlayerController.h"
#include "Player/MO_PlayerState.h"
#include "UI/HUD/MO_HUD.h"


// Sets default values
AMO_BaseCharacter::AMO_BaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UMO_CharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{

	LaunchComponent= CreateDefaultSubobject<ULaunchComponent>("LaunchComponent");
	SpeedBoostComponent = CreateDefaultSubobject<USpeedBoostComponent>("SpeedBoostComponent");
	PrimaryActorTick.bCanEverTick = true;
}

void AMO_BaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMO_BaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (LaunchComponent)
	{
		bIsObstacleLaunch = LaunchComponent->IsObstacleLaunch();
	}
}

// Called to bind functionality to input
void AMO_BaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInput || !SlideAction)
	{
		return;
	}

	// Hold-to-slide: Canceled covers releases the trigger classifies as aborts.
	EnhancedInput->BindAction(SlideAction, ETriggerEvent::Started, this, &AMO_BaseCharacter::OnSlidePressed);
	EnhancedInput->BindAction(SlideAction, ETriggerEvent::Completed, this, &AMO_BaseCharacter::OnSlideReleased);
	EnhancedInput->BindAction(SlideAction, ETriggerEvent::Canceled, this, &AMO_BaseCharacter::OnSlideReleased);
}

void AMO_BaseCharacter::OnSlidePressed()
{
	if (UMO_CharacterMovementComponent* MOMovement = Cast<UMO_CharacterMovementComponent>(GetCharacterMovement()))
	{
		MOMovement->SetWantsToSlide(true);
	}
}

void AMO_BaseCharacter::OnSlideReleased()
{
	if (UMO_CharacterMovementComponent* MOMovement = Cast<UMO_CharacterMovementComponent>(GetCharacterMovement()))
	{
		MOMovement->SetWantsToSlide(false);
	}
}

float AMO_BaseCharacter::GetSpeedBoostMultiplier() const
{
	return SpeedBoostComponent ? SpeedBoostComponent->GetCurrentSpeedMultiplier() : 1.f;
}

void AMO_BaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init ability actor info for the server
	InitAbilityActorInfo();

	UE_LOG(LogTemp, Log, TEXT("MO_BaseCharacter::PossessedBy: %s possessed by %s (PlayerHUDClass=%s)"),
		*GetName(), *GetNameSafe(NewController), *GetNameSafe(PlayerHUDClass));

	if (PlayerHUDWidget || !PlayerHUDClass)
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(NewController);

	if (!PC || !PC->IsLocalController())
	{
		UE_LOG(LogTemp, Log, TEXT("MO_BaseCharacter::PossessedBy: skipping HUD creation (not a local player controller)"));
		return;
	}

	PlayerHUDWidget = CreateWidget<UUserWidget>(PC, PlayerHUDClass);

	UE_LOG(LogTemp, Log, TEXT("MO_BaseCharacter::PossessedBy: created HUD widget %s"), *GetNameSafe(PlayerHUDWidget));

	if (PlayerHUDWidget)
	{
		PlayerHUDWidget->AddToViewport();
	}
}

void AMO_BaseCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init ability actor info for the client
	InitAbilityActorInfo();
}

UAbilitySystemComponent* AMO_BaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AMO_BaseCharacter::InitAbilityActorInfo()
{
	AMO_PlayerState* MO_PlayerState = GetPlayerState<AMO_PlayerState>();

	if (!MO_PlayerState)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("MO_BaseCharacter::InitAbilityActorInfo: PlayerState is not AMO_PlayerState. Set 'Player State Class' to MO_PlayerState in your GameMode."));
		return;
	}

	MO_PlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(MO_PlayerState, this);
	AbilitySystemComponent = MO_PlayerState->GetAbilitySystemComponent();

	// Aura pattern: the overlay comes up once the ASC is wired, from both the
	// server (PossessedBy) and client (OnRep_PlayerState) paths. GetHUD() is
	// only valid on the local player's controller.
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (AMO_HUD* MO_HUD = Cast<AMO_HUD>(PC->GetHUD()))
		{
			MO_HUD->InitOverlay(PC, MO_PlayerState, AbilitySystemComponent, /*AttributeSet*/ nullptr);
		}
	}
}

