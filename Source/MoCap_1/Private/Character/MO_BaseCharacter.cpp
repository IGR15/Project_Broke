// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MO_BaseCharacter.h"

#include "Components/LaunchComponent.h"


// Sets default values
AMO_BaseCharacter::AMO_BaseCharacter()
{

	LaunchComponent= CreateDefaultSubobject<ULaunchComponent>("LaunchComponent");
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

