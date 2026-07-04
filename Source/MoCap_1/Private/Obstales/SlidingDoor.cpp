#include "Obstales/SlidingDoor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"

ASlidingDoor::ASlidingDoor()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>("DoorMesh");
	DoorMesh->SetupAttachment(Root);
}

void ASlidingDoor::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = DoorMesh->GetRelativeLocation();

	float Direction = bSlideLeft ? -1.f : 1.f;

	OpenLocation = StartLocation + FVector(0.f, Direction * SlideDistance, 0.f);

	TargetLocation = OpenLocation;
	bGoingToOpen = true;
}

void ASlidingDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Current = DoorMesh->GetRelativeLocation();

	// Move toward target
	FVector NewLocation = FMath::VInterpConstantTo(
		Current,
		TargetLocation,
		DeltaTime,
		SlideSpeed
	);

	DoorMesh->SetRelativeLocation(NewLocation);

	// Check if reached target
	if (FVector::Dist(NewLocation, TargetLocation) < 1.f)
	{
		HoldTimer += DeltaTime;

		if (HoldTimer >= HoldTime)
		{
			HoldTimer = 0.f;

			// Switch direction
			bGoingToOpen = !bGoingToOpen;

			TargetLocation = bGoingToOpen ? OpenLocation : StartLocation;
		}
	}
}