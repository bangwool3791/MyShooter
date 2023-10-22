


#include "Trigger.h"
#include "MyCharacter.h"
#include "CDoor.h"
#include "Components\BoxComponent.h"
#include "Components\SphereComponent.h"
#include "Components\WidgetComponent.h"
#include "Kismet/GameplayStatics.h"

ATrigger::ATrigger()
	:DoorActor(nullptr)
{
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ButtonMesh"));

	UStaticMesh* ButtonMesh = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("/Game/Voyager/Demo/Levels/UndergroundBaseLevel/Meshes/SM_TriggerButton.SM_TriggerButton")));
	if (ButtonMesh)
	{
		StaticMesh->SetStaticMesh(ButtonMesh);
	}

	SetRootComponent(StaticMesh);
	ItemMesh->SetupAttachment(StaticMesh);
	FVector vLocation(0.f, 0.f, 150.f);
	AreaSphere->SetRelativeLocation(vLocation);
	AreaSphere->SetSphereRadius(250.f);
	vLocation.Z = 160.f;
	FVector vScale(1.f, 8.0f, 5.f);
	CollisionBox->SetRelativeLocation(vLocation);
	CollisionBox->SetRelativeScale3D(vScale);
	vLocation.X = 60.f;
	vLocation.Y = 130.f;
	vLocation.Z = 220.f;
	PickupWidget->SetRelativeLocation(vLocation);
}

void ATrigger::BeginPlay()
{
	Super::BeginPlay();

	// Hide Pickup widget
	if (PickupWidget)
		PickupWidget->SetVisibility(false);

	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ATrigger::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &ATrigger::OnSphereEndOverlap);
	
	TSubclassOf<ACDoor> classToFind;
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACDoor::StaticClass(), FoundActors);

	if(FoundActors.Num())
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Find Actor Num"));

	double dist = 0.f;

	for (int i = 0; i < FoundActors.Num(); ++i)
	{
		FString strName = "";
		FoundActors[i]->GetActorLocation();
		double currentdist =  GetActorLocation().Distance(FoundActors[i]->GetActorLocation(), GetActorLocation());


		if (currentdist > dist)
		{
			dist = currentdist;
			DoorActor = Cast<ACDoor>(FoundActors[i]);
		}
	}
}

void ATrigger::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMyCharacter* myCharacter = Cast<AMyCharacter>(OtherActor);
		if (myCharacter)
		{
			myCharacter->IncrementOverlappedTriggerCount(1);
		}
	}
}

void ATrigger::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMyCharacter* myCharacter = Cast<AMyCharacter>(OtherActor);
		if (myCharacter)
		{
			myCharacter->IncrementOverlappedTriggerCount(-1);
		}
	}
}