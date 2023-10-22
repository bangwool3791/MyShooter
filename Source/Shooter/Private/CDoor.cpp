


#include "CDoor.h"
#include "Components/SceneComponent.h"
#include "Components\BoxComponent.h"
#include "Components\SphereComponent.h"
#include "Components\WidgetComponent.h"
#include "MyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Engine/StaticMeshSocket.h"
#include "Sound/SoundCue.h"

// Sets default values
ACDoor::ACDoor()
	: nOpenClose(0)
	, bDoorTimerActive(false)
	, OpenCloseDuration(0.6f)
	, bSteam(false)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//CreateStaticComponent();

	{
		ConstructorHelpers::FObjectFinder<UParticleSystem> SmokeParticleAsset
		(TEXT("/Game/Voyager/Demo/Particles/P_lit_steamLow.P_lit_steamLow"));
		SmokeParticle = SmokeParticleAsset.Object;
	}

	{
		ConstructorHelpers::FObjectFinder<USoundCue> SoundAsset(
			TEXT("/Game/Voyager/Demo/Audio/Other/SCue_Blueprints_DoorClose01.SCue_Blueprints_DoorClose01")
		);

		DoorCloseSound = SoundAsset.Object;
	}

	{
		ConstructorHelpers::FObjectFinder<USoundCue> SoundAsset(
			TEXT("/Game/Voyager/Demo/Audio/Other/SCue_Blueprints_DoorOpen01.SCue_Blueprints_DoorOpen01")
		);

		DoorOpenSound = SoundAsset.Object;
	}
}

// Called when the game starts or when spawned
void ACDoor::BeginPlay()
{
	Aitem::BeginPlay();
}

// Called every frame
void ACDoor::Tick(float DeltaTime)
{
	Aitem::Tick(DeltaTime);

}

void ACDoor::OpencloseDoor()
{
	if (bDoorTimerActive == false)
	{
		LeftDoorComponent->SetSimulatePhysics(true);
		LeftDoorComponent->SetEnableGravity(false);
		LeftDoorComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		LeftDoorComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		RightDoorComponent->SetSimulatePhysics(true);
		RightDoorComponent->SetEnableGravity(false);
		RightDoorComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		RightDoorComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		if (SmokeParticle)
		{
			if (bSteam == false)
			{
				bSteam = true;
				FTransform SocketTransform = LeftDoorComponent->GetSocketTransform("RootSocket");
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SmokeParticle, SocketTransform);
			}
		}

		bDoorTimerActive = true;

		if (nOpenClose == 1)
		{
			nOpenClose = 0;

			if (DoorCloseSound)
				UGameplayStatics::PlaySound2D(this, DoorCloseSound);

			FVector vecLeft(0.f, -50000.f, 0.f);
			LeftDoorComponent->AddImpulse(vecLeft);
			FVector vecRight(0.f, 50000.f, 0.f);
			RightDoorComponent->AddImpulse(vecRight);
		}
		else
		{
			nOpenClose = 1;

			if (DoorOpenSound)
				UGameplayStatics::PlaySound2D(this, DoorOpenSound);

			FVector vecLeft(0.f, 50000.f, 0.f);
			LeftDoorComponent->AddImpulse(vecLeft);
			FVector vecRight(0.f, -50000.f, 0.f);
			RightDoorComponent->AddImpulse(vecRight);
		}

		GetWorldTimerManager().SetTimer(
			DoorOpenCloseTimer,
			this,
			&ACDoor::DoorOpenCloseFunc,
			OpenCloseDuration);
	}
}

void ACDoor::BindSocketTransform(UStaticMeshComponent* target, UStaticMeshComponent* source, const FString& SocketName)
{
	const UStaticMeshSocket* Socket = source->GetSocketByName(*SocketName);
	FTransform SocketTransform;

	if (Socket)
	{
		if (Socket->GetSocketTransform(SocketTransform, source))
		{
			target->SetRelativeLocation(SocketTransform.GetLocation());
			target->AttachToComponent(source, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		}
	}
}

void ACDoor::DoorOpenCloseFunc()
{
	LeftDoorComponent->SetSimulatePhysics(false);
	LeftDoorComponent->SetEnableGravity(false);
	LeftDoorComponent->SetVisibility(true);
	LeftDoorComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	LeftDoorComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	RightDoorComponent->SetSimulatePhysics(false);
	RightDoorComponent->SetEnableGravity(false);
	RightDoorComponent->SetVisibility(true);
	RightDoorComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	RightDoorComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	bDoorTimerActive = false;

}

void ACDoor::CreateStaticComponent()
{
	//Create scenecomponent for root  
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("BaseScene"));
	SceneComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	SetRootComponent(SceneComponent);

	//Create Left Door Mesh
	LeftDoorComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftDoor"));
	LeftDoorComponent->SetupAttachment(SceneComponent);

	//Create Right Door Mesh
	RightDoorComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightDoor"));
	RightDoorComponent->SetupAttachment(SceneComponent);

	////Navigation Mesh
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(SceneComponent);
	//Get Mesh
	UStaticMesh* MeshToDoor = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("/Game/Voyager/Demo/Levels/UndergroundBaseLevel/Meshes/SM_Door.SM_Door")));

	if (MeshToDoor && LeftDoorComponent && RightDoorComponent)
	{
		//Set Mesh to UStaticMeshComponent
		LeftDoorComponent->SetStaticMesh(MeshToDoor);
		RightDoorComponent->SetStaticMesh(MeshToDoor);
		RightDoorComponent->SetRelativeLocation(FVector(0.f, 0.f, 0.f));

		//Rotate 180 degree z axis
		FQuat fRot;
		fRot.Z = 180.f;
		RightDoorComponent->SetRelativeRotation(fRot);

		//Set Material 
		{
			ConstructorHelpers::FObjectFinder<UMaterialInstance> FoundMaterial(TEXT("/Game/Voyager/Demo/Levels/UndergroundBaseLevel/Materials/M_DemoWall_Inst_3.M_DemoWall_Inst_3"));
			if (FoundMaterial.Succeeded())
			{
				LoadMaterial.Push(FoundMaterial.Object->GetMaterial());
			}
		}

		{
			ConstructorHelpers::FObjectFinder<UMaterialInstance> FoundMaterial(TEXT("/Game/Voyager/Demo/Levels/UndergroundBaseLevel/Materials/M_GlasDoor_Inst.M_GlasDoor_Inst"));
			if (FoundMaterial.Succeeded())
			{
				LoadMaterial.Push(FoundMaterial.Object->GetMaterial());
			}
		}

		//Set Material to Mesh
		if (LoadMaterial.Num() == 2)
		{
			DynamicMaterialInst.Push(UMaterialInstanceDynamic::Create(LoadMaterial[0], MeshToDoor));
			DynamicMaterialInst.Push(UMaterialInstanceDynamic::Create(LoadMaterial[1], MeshToDoor));

			LeftDoorComponent->SetMaterial(0, DynamicMaterialInst[0]);
			LeftDoorComponent->SetMaterial(1, DynamicMaterialInst[1]);
			RightDoorComponent->SetMaterial(0, DynamicMaterialInst[0]);
			RightDoorComponent->SetMaterial(1, DynamicMaterialInst[1]);
		}
	}
	//Create center accesorry 
	UStaticMesh* MeshToAcc = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), NULL, TEXT("/Game/Voyager/Demo/Levels/UndergroundBaseLevel/Meshes/SM_DoorLock.SM_DoorLock")));
	CenterDoorComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CenterDoor"));

	if (MeshToAcc && CenterDoorComponent)
	{
		//Set Mesh to UStaticMeshComponent
		CenterDoorComponent->SetStaticMesh(MeshToAcc);

		//Set Material 
		{
			ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial(TEXT("/Game/Voyager/Demo/Levels/UndergroundBaseLevel/Materials/M_UE4_Logo_Doors.M_UE4_Logo_Doors"));
			if (FoundMaterial.Succeeded())
			{
				LoadMaterial.Push(FoundMaterial.Object);
			}
		}

		{
			ConstructorHelpers::FObjectFinder<UMaterialInstance> FoundMaterial(TEXT("/Game/Voyager/Demo/Levels/UndergroundBaseLevel/Materials/M_DemoWall_Inst_3.M_DemoWall_Inst_3"));
			if (FoundMaterial.Succeeded())
			{
				LoadMaterial.Push(FoundMaterial.Object->GetMaterial());
			}
		}

		//Set Material to Mesh
		if (LoadMaterial.Num() == 4)
		{

			DynamicMaterialInst.Push(UMaterialInstanceDynamic::Create(LoadMaterial[2], MeshToAcc));
			DynamicMaterialInst.Push(UMaterialInstanceDynamic::Create(LoadMaterial[3], MeshToAcc));
			CenterDoorComponent->SetMaterial(0, DynamicMaterialInst[2]);
			CenterDoorComponent->SetMaterial(1, DynamicMaterialInst[3]);
		}
	}

	//Set root component to leftdoor
	if (CenterDoorComponent && LeftDoorComponent)
	{
		const UStaticMeshSocket* Socket = LeftDoorComponent->GetSocketByName("CenterSocket");
		FTransform SocketTransform;

		if (Socket)
		{
			if (Socket->GetSocketTransform(SocketTransform, LeftDoorComponent))
			{
				CenterDoorComponent->SetRelativeLocation(SocketTransform.GetLocation());
				CenterDoorComponent->AttachToComponent(LeftDoorComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			}
		}
	}

	ItemMesh->SetupAttachment(StaticMesh);
	FVector vLocation(0.f, 0.f, 150.f);
	AreaSphere->SetRelativeLocation(vLocation);
	AreaSphere->SetSphereRadius(250.f);
	vLocation.Z = 160.f;
	FVector vScale(1.f, 8.0f, 5.f);
	CollisionBox->SetRelativeLocation(vLocation);
	CollisionBox->SetRelativeScale3D(vScale);
	vLocation.X = 70.f;
	vLocation.Z = 140.f;
	PickupWidget->SetRelativeLocation(vLocation);
}