// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/MO_ItemInfo.h"

#include "AbilitySystem/Abilities/MO_BazookaAbility.h"
#include "AbilitySystem/Abilities/MO_BonkAbility.h"
#include "AbilitySystem/Abilities/MO_LubeAbility.h"
#include "AbilitySystem/Abilities/MO_MosquitoAbility.h"
#include "AbilitySystem/Abilities/MO_YeetAbility.h"
#include "AbilitySystem/Abilities/MO_ZeusAbility.h"

UMO_ItemInfo::UMO_ItemInfo()
{
	auto AddItem = [this](const FString& ItemName, const TSubclassOf<UMO_ItemAbility>& AbilityClass)
	{
		FMO_ItemAbilityInfo& Info = ItemAbilities.AddDefaulted_GetRef();
		Info.ItemName = ItemName;
		Info.AbilityClass = AbilityClass;
	};

	AddItem(TEXT("Bazooka"), UMO_BazookaAbility::StaticClass());
	AddItem(TEXT("Bonk"), UMO_BonkAbility::StaticClass());
	AddItem(TEXT("Lube"), UMO_LubeAbility::StaticClass());
	AddItem(TEXT("Mosquito"), UMO_MosquitoAbility::StaticClass());
	AddItem(TEXT("Yeet"), UMO_YeetAbility::StaticClass());
	AddItem(TEXT("Zeus"), UMO_ZeusAbility::StaticClass());
}

FMO_ItemAbilityInfo UMO_ItemInfo::GetRandomItem() const
{
	if (ItemAbilities.IsEmpty())
	{
		return FMO_ItemAbilityInfo();
	}

	return ItemAbilities[FMath::RandRange(0, ItemAbilities.Num() - 1)];
}
