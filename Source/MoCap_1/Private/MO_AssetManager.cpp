// Fill out your copyright notice in the Description page of Project Settings.


#include "MO_AssetManager.h"

#include "MO_GameplayTags.h"

UMO_AssetManager& UMO_AssetManager::Get()
{
	check(GEngine);

	UMO_AssetManager* MO_AssetManager = Cast<UMO_AssetManager>(GEngine->AssetManager);
	check(MO_AssetManager);

	return *MO_AssetManager;
}

void UMO_AssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FMO_GameplayTags::InitializeNativeGameplayTags();
}
