// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/MysteryBoxSpawner.h"

#include "Components/SceneComponent.h"
#include "Pickups/MysteryBox.h"
#include "TimerManager.h"
#include "Engine/World.h"

AMysteryBoxSpawner::AMysteryBoxSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;
}

void AMysteryBoxSpawner::BeginPlay()
{
	Super::BeginPlay();

	CheckAndSpawn();

	GetWorldTimerManager().SetTimer(
		CheckTimerHandle,
		this,
		&AMysteryBoxSpawner::CheckAndSpawn,
		CheckInterval,
		true);
}

void AMysteryBoxSpawner::CheckAndSpawn()
{
	if (IsValid(SpawnedBox))
	{
		return;
	}

	if (!MysteryBoxClass)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	SpawnedBox = GetWorld()->SpawnActor<AMysteryBox>(
		MysteryBoxClass,
		GetActorLocation(),
		GetActorRotation(),
		SpawnParams);
}
