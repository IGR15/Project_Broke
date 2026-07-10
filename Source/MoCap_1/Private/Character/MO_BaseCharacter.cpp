// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MO_BaseCharacter.h"

#include "Blueprint/UserWidget.h"
#include "Components/LaunchComponent.h"
#include "Components/SpeedBoostComponent.h"
#include "GameFramework/PlayerController.h"


// Sets default values
AMO_BaseCharacter::AMO_BaseCharacter()
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
}

float AMO_BaseCharacter::GetSpeedBoostMultiplier() const
{
	return SpeedBoostComponent ? SpeedBoostComponent->GetCurrentSpeedMultiplier() : 1.f;
}

void AMO_BaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

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

