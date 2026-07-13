// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * MO_GameplayTags
 *
 * Singleton containing the project's native gameplay tags.
 * Registered once at startup from UMO_AssetManager::StartInitialLoading.
 * Access anywhere with FMO_GameplayTags::Get().
 */
struct FMO_GameplayTags
{
public:
	static const FMO_GameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeGameplayTags();

	/*
	 * Input
	 */

	// The single "use item" slot; whichever item ability holds this tag in its
	// spec's DynamicAbilityTags is the one LMB activates.
	FGameplayTag InputTag_UseItem;

	/*
	 * Item Abilities (granted by the Mystery Box)
	 */
	FGameplayTag Abilities_Item_Bazooka;
	FGameplayTag Abilities_Item_Bonk;
	FGameplayTag Abilities_Item_Lube;
	FGameplayTag Abilities_Item_Mosquito;
	FGameplayTag Abilities_Item_Yeet;
	FGameplayTag Abilities_Item_Zeus;

private:
	static FMO_GameplayTags GameplayTags;
};
