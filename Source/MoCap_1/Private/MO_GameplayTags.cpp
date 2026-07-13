// Fill out your copyright notice in the Description page of Project Settings.


#include "MO_GameplayTags.h"

#include "GameplayTagsManager.h"

FMO_GameplayTags FMO_GameplayTags::GameplayTags;

void FMO_GameplayTags::InitializeNativeGameplayTags()
{
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();

	/*
	 * Input
	 */
	GameplayTags.InputTag_UseItem = Manager.AddNativeGameplayTag(
		FName("InputTag.UseItem"),
		FString("Input slot for the currently held Mystery Box item (LMB)"));

	/*
	 * Item Abilities
	 */
	GameplayTags.Abilities_Item_Bazooka = Manager.AddNativeGameplayTag(
		FName("Abilities.Item.Bazooka"),
		FString("Bazooka item ability, granted by the Mystery Box"));

	GameplayTags.Abilities_Item_Bonk = Manager.AddNativeGameplayTag(
		FName("Abilities.Item.Bonk"),
		FString("Bonk item ability, granted by the Mystery Box"));

	GameplayTags.Abilities_Item_Lube = Manager.AddNativeGameplayTag(
		FName("Abilities.Item.Lube"),
		FString("Lube item ability, granted by the Mystery Box"));

	GameplayTags.Abilities_Item_Mosquito = Manager.AddNativeGameplayTag(
		FName("Abilities.Item.Mosquito"),
		FString("Mosquito item ability, granted by the Mystery Box"));

	GameplayTags.Abilities_Item_Yeet = Manager.AddNativeGameplayTag(
		FName("Abilities.Item.Yeet"),
		FString("Yeet item ability, granted by the Mystery Box"));

	GameplayTags.Abilities_Item_Zeus = Manager.AddNativeGameplayTag(
		FName("Abilities.Item.Zeus"),
		FString("Zeus item ability, granted by the Mystery Box"));
}
