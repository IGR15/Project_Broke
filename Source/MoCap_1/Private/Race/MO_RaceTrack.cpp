// Fill out your copyright notice in the Description page of Project Settings.


#include "Race/MO_RaceTrack.h"

#include "Components/SplineComponent.h"
#include "Game/MO_RaceGameState.h"

AMO_RaceTrack::AMO_RaceTrack()
{
	PrimaryActorTick.bCanEverTick = false;

	TrackSpline = CreateDefaultSubobject<USplineComponent>("TrackSpline");
	SetRootComponent(TrackSpline);
	TrackSpline->SetClosedLoop(true);
}

void AMO_RaceTrack::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (SplinePointOverrides.Num() >= 3)
	{
		TrackSpline->ClearSplinePoints(false);
		for (const FVector& Point : SplinePointOverrides)
		{
			TrackSpline->AddSplinePoint(Point, ESplineCoordinateSpace::Local, false);
		}
		TrackSpline->SetClosedLoop(true, false);
		TrackSpline->UpdateSpline();
	}
}

void AMO_RaceTrack::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		return;
	}

	if (AMO_RaceGameState* RaceGS = GetWorld()->GetGameState<AMO_RaceGameState>())
	{
		RaceGS->RegisterTrack(TrackSpline, TotalLaps);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
			TEXT("MO_RaceTrack: GameState is not AMO_RaceGameState — set it on the GameMode or laps/positions won't run."));
	}
}
