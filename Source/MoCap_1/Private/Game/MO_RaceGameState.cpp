// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MO_RaceGameState.h"

#include "Components/SplineComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/MO_PlayerState.h"
#include "TimerManager.h"

void AMO_RaceGameState::RegisterTrack(USplineComponent* InTrackSpline, const int32 InTotalLaps)
{
	if (!HasAuthority() || !InTrackSpline)
	{
		return;
	}

	TrackSpline = InTrackSpline;
	TotalLaps = FMath::Max(1, InTotalLaps);
	CheckpointDistances.Reset();

	for (APlayerState* PS : PlayerArray)
	{
		if (AMO_PlayerState* MOPS = Cast<AMO_PlayerState>(PS))
		{
			InitRacer(MOPS);
		}
	}

	GetWorld()->GetTimerManager().SetTimer(
		PositionUpdateTimer, this, &AMO_RaceGameState::UpdateRacePositions, PositionUpdatePeriod, true);
}

void AMO_RaceGameState::RegisterCheckpoint(const int32 CheckpointIndex, const FVector& WorldLocation)
{
	if (!HasAuthority() || CheckpointIndex < 0)
	{
		return;
	}

	CheckpointLocations.Add(CheckpointIndex, WorldLocation);
	CheckpointDistances.Reset();
}

void AMO_RaceGameState::RacerCrossedCheckpoint(AMO_PlayerState* RacerPS, const int32 CheckpointIndex)
{
	const int32 NumCheckpoints = CheckpointLocations.Num();
	if (!HasAuthority() || !RacerPS || RacerPS->HasFinishedRace() || NumCheckpoints == 0)
	{
		return;
	}

	// Out-of-order hit: driving backward, cutting, or re-touching — ignore.
	if (CheckpointIndex != RacerPS->GetNextCheckpointIndex())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow,
				FString::Printf(TEXT("checkpoint %d ignored (out of order, expected %d) - %s"),
					CheckpointIndex, RacerPS->GetNextCheckpointIndex(), *RacerPS->GetPlayerName()));
		}
		return;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green,
			FString::Printf(TEXT("checkpoint reached %d - %s"), CheckpointIndex, *RacerPS->GetPlayerName()));
	}
	UE_LOG(LogTemp, Log, TEXT("MO_RaceGameState: checkpoint reached %d - %s"),
		CheckpointIndex, *RacerPS->GetPlayerName());

	if (CheckpointIndex == 0)
	{
		// Crossed the finish line with all checkpoints validated: lap complete.
		const float Now = GetServerWorldTimeSeconds();
		if (RacerPS->GetLapStartServerTime() >= 0.f)
		{
			RacerPS->RecordLapTime(Now - RacerPS->GetLapStartServerTime());
		}

		if (RacerPS->GetCurrentLap() >= TotalLaps)
		{
			RacerPS->SetFinishedRace(true);
			RacerPS->SetRacePosition(++NumFinished);
		}
		else
		{
			RacerPS->SetCurrentLap(RacerPS->GetCurrentLap() + 1);
			RacerPS->StartLap(Now);
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan,
					FString::Printf(TEXT("lap__started (lap %d/%d) - %s"),
						RacerPS->GetCurrentLap(), TotalLaps, *RacerPS->GetPlayerName()));
			}
			UE_LOG(LogTemp, Log, TEXT("MO_RaceGameState: lap__started (lap %d/%d) - %s"),
				RacerPS->GetCurrentLap(), TotalLaps, *RacerPS->GetPlayerName());
		}
		RacerPS->SetNextCheckpointIndex(NumCheckpoints > 1 ? 1 : 0);
	}
	else
	{
		RacerPS->SetNextCheckpointIndex((CheckpointIndex + 1) % NumCheckpoints);
	}
}

void AMO_RaceGameState::RespawnRacerAtLastCheckpoint(AMO_PlayerState* RacerPS)
{
	const int32 NumCheckpoints = CheckpointLocations.Num();
	if (!HasAuthority() || !RacerPS || NumCheckpoints == 0)
	{
		return;
	}

	APawn* Pawn = RacerPS->GetPawn();
	if (!Pawn)
	{
		return;
	}

	const int32 Next = RacerPS->GetNextCheckpointIndex() % NumCheckpoints;
	const int32 LastCrossed = (Next - 1 + NumCheckpoints) % NumCheckpoints;
	const FVector* LastLocation = CheckpointLocations.Find(LastCrossed);
	if (!LastLocation)
	{
		return;
	}

	// Face the gate the racer still has to cross rather than trusting the
	// checkpoint actor's rotation (gates are oriented across the track).
	FRotator FacingRotation = Pawn->GetActorRotation();
	if (const FVector* NextLocation = CheckpointLocations.Find(Next); NextLocation && Next != LastCrossed)
	{
		FacingRotation = (*NextLocation - *LastLocation).GetSafeNormal2D().Rotation();
	}

	Pawn->TeleportTo(*LastLocation + FVector(0.f, 0.f, RespawnHeightOffset), FacingRotation);
	if (const ACharacter* Character = Cast<ACharacter>(Pawn))
	{
		Character->GetCharacterMovement()->StopMovementImmediately();
	}
	if (AController* Controller = Pawn->GetController())
	{
		Controller->SetControlRotation(FacingRotation);
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red,
			FString::Printf(TEXT("respawned at checkpoint %d - %s"), LastCrossed, *RacerPS->GetPlayerName()));
	}
	UE_LOG(LogTemp, Log, TEXT("MO_RaceGameState: respawned at checkpoint %d - %s"),
		LastCrossed, *RacerPS->GetPlayerName());
}

void AMO_RaceGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);

	// Late joiners on a live track start racing immediately.
	if (HasAuthority() && TrackSpline)
	{
		if (AMO_PlayerState* MOPS = Cast<AMO_PlayerState>(PlayerState))
		{
			InitRacer(MOPS);
		}
	}
}

void AMO_RaceGameState::InitRacer(AMO_PlayerState* RacerPS)
{
	RacerPS->SetTotalLaps(TotalLaps);
	RacerPS->SetCurrentLap(1);
	RacerPS->SetNextCheckpointIndex(CheckpointLocations.Num() > 1 ? 1 : 0);
	RacerPS->SetFinishedRace(false);
	RacerPS->StartLap(GetServerWorldTimeSeconds());
}

void AMO_RaceGameState::UpdateRacePositions()
{
	if (!TrackSpline || !EnsureCheckpointDistances())
	{
		return;
	}

	TArray<AMO_PlayerState*> Racers;
	for (APlayerState* PS : PlayerArray)
	{
		if (AMO_PlayerState* MOPS = Cast<AMO_PlayerState>(PS); MOPS && !MOPS->HasFinishedRace())
		{
			Racers.Add(MOPS);
		}
	}

	Racers.Sort([this](const AMO_PlayerState& A, const AMO_PlayerState& B)
	{
		return GetRacerTotalProgress(&A) > GetRacerTotalProgress(&B);
	});

	// Finished racers already own positions 1..NumFinished.
	for (int32 i = 0; i < Racers.Num(); ++i)
	{
		Racers[i]->SetRacePosition(NumFinished + i + 1);
	}
}

float AMO_RaceGameState::GetRacerTotalProgress(const AMO_PlayerState* RacerPS) const
{
	const int32 NumCheckpoints = CheckpointDistances.Num();
	const float TrackLength = TrackSpline->GetSplineLength();
	const int32 Next = RacerPS->GetNextCheckpointIndex();
	const int32 LastCrossed = (Next - 1 + NumCheckpoints) % NumCheckpoints;
	const int32 PassedThisLap = Next == 0 ? NumCheckpoints - 1 : Next - 1;

	float SegmentDist = 0.f;
	if (const APawn* Pawn = RacerPS->GetPawn())
	{
		const float Key = TrackSpline->FindInputKeyClosestToWorldLocation(Pawn->GetActorLocation());
		const float Dist = TrackSpline->GetDistanceAlongSplineAtSplineInputKey(Key);
		// Progress inside the current checkpoint segment, wrap-safe across the
		// spline's start/end seam.
		SegmentDist = FMath::Fmod(Dist - CheckpointDistances[LastCrossed] + TrackLength, TrackLength);

		// Backing up past the last validated checkpoint would wrap to ~full
		// track length and fake a huge lead — clamp it away.
		const float SegmentLength =
			FMath::Fmod(CheckpointDistances[Next] - CheckpointDistances[LastCrossed] + TrackLength, TrackLength);
		if (SegmentLength > 1.f && SegmentDist > SegmentLength * 1.1f)
		{
			SegmentDist = 0.f;
		}
	}

	return (static_cast<float>(RacerPS->GetCurrentLap() - 1) * NumCheckpoints + PassedThisLap) * TrackLength
		+ SegmentDist;
}

bool AMO_RaceGameState::EnsureCheckpointDistances()
{
	const int32 NumCheckpoints = CheckpointLocations.Num();
	if (NumCheckpoints == 0)
	{
		return false;
	}
	if (CheckpointDistances.Num() == NumCheckpoints)
	{
		return true;
	}

	CheckpointDistances.Reset();
	for (int32 i = 0; i < NumCheckpoints; ++i)
	{
		const FVector* Location = CheckpointLocations.Find(i);
		if (!Location)
		{
			UE_LOG(LogTemp, Warning,
				TEXT("MO_RaceGameState: checkpoint indices must be contiguous from 0 — index %d is missing."), i);
			CheckpointDistances.Reset();
			return false;
		}
		const float Key = TrackSpline->FindInputKeyClosestToWorldLocation(*Location);
		CheckpointDistances.Add(TrackSpline->GetDistanceAlongSplineAtSplineInputKey(Key));
	}
	return true;
}
