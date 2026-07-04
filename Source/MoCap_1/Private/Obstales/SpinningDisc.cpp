// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstales/SpinningDisc.h"


#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"

ASpinningDisc::ASpinningDisc()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	DiscMesh = CreateDefaultSubobject<UStaticMeshComponent>("DiscMesh");
	DiscMesh->SetupAttachment(Root);

	RotatingMovement = CreateDefaultSubobject<URotatingMovementComponent>("RotatingMovement");
}

void ASpinningDisc::BeginPlay()
{
	Super::BeginPlay();

	const float Direction = bRotateLeft ? -1.f : 1.f;

	// Flat disc spins around the Z axis
	RotatingMovement->RotationRate = FRotator(
		0.f,
		Direction * RotationSpeed,
		0.f
		
	);
}

