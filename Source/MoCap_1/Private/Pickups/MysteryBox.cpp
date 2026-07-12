// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/MysteryBox.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystem/MO_AbilitySystemComponent.h"
#include "AbilitySystem/Data/MO_ItemInfo.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "AbilitySystem/Abilities/MO_ItemAbility.h"
#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"

AMysteryBox::AMysteryBox()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	BoxMesh = CreateDefaultSubobject<UStaticMeshComponent>("BoxMesh");
	BoxMesh->SetupAttachment(Root);
	BoxMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TriggerCollision = CreateDefaultSubobject<UBoxComponent>("TriggerCollision");
	TriggerCollision->SetupAttachment(Root);
	TriggerCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerCollision->SetGenerateOverlapEvents(true);

	RotatingMovement = CreateDefaultSubobject<URotatingMovementComponent>("RotatingMovement");

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> DefaultPopEffect(
		TEXT("/Game/Effects/NS_MysteryBoxPop.NS_MysteryBoxPop"));
	if (DefaultPopEffect.Succeeded())
	{
		PopEffect = DefaultPopEffect.Object;
	}
}

void AMysteryBox::BeginPlay()
{
	Super::BeginPlay();

	InitialLocation = GetActorLocation();

	RotatingMovement->RotationRate = FRotator(0.f, RotationSpeed, 0.f);

	TriggerCollision->OnComponentBeginOverlap.AddDynamic(
		this,
		&AMysteryBox::OnOverlap);
}

void AMysteryBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	const float ZOffset = FMath::Sin(RunningTime * FloatFrequency * 2.f * PI) * FloatAmplitude;
	SetActorLocation(InitialLocation + FVector(0.f, 0.f, ZOffset));
}

void AMysteryBox::OnOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	ACharacter* Character = Cast<ACharacter>(OtherActor);

	if (!Character)
	{
		return;
	}

	// Abilities are granted on the server; clients see the result through the ASC.
	if (!HasAuthority())
	{
		return;
	}

	UMO_AbilitySystemComponent* MO_ASC = Cast<UMO_AbilitySystemComponent>(
		UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor));

	const UMO_ItemInfo* Info = ItemInfo ? ItemInfo.Get() : GetDefault<UMO_ItemInfo>();
	const FMO_ItemAbilityInfo Item = Info->GetRandomItem();

	if (MO_ASC && Item.AbilityClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("MysteryBox: %s received %s"), *Character->GetName(), *Item.ItemName);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				3.f,
				FColor::Yellow,
				FString::Printf(TEXT("Mystery Box: You got %s!"), *Item.ItemName));
		}

		MO_ASC->AddItemAbility(Item.AbilityClass);
	}
	else
	{
		// No AbilitySystemComponent on the character (GameMode not using MO_PlayerState yet)
		// - fall back to the old random-number reward.
		const int32 ItemNumber = FMath::RandRange(MinItemNumber, MaxItemNumber);

		UE_LOG(LogTemp, Warning, TEXT("MysteryBox: %s has no AbilitySystemComponent, falling back to item #%d"),
			*Character->GetName(), ItemNumber);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				3.f,
				FColor::Yellow,
				FString::Printf(TEXT("Mystery Box: You got item #%d"), ItemNumber));
		}
	}

	if (PopEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			PopEffect,
			GetActorLocation(),
			GetActorRotation());
	}

	Destroy();
}
