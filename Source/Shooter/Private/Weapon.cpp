


#include "Weapon.h"
#include "MyCharacter.h"
#include "Components\SphereComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SpotLightComponent.h"
#include "Camera/CameraComponent.h"

AWeapon::AWeapon()
	:ThrowWeaponTime(0.3f)
	, bFalling(false)
	, ZCurveTime(0.3f)
	, ItemInterpStartLocation(FVector(0.f))
	, CameraTargetLocation(FVector(0.f))
	, bInterping(false)
	, ItemInterpX(0.f)
	, ItemInterpY(0.f)
	, InterpInitialYawOffset(0.f)
{
	//총 쏠 때 빛 효과를 위해 포인트 라이트를 생성한다.
	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("WeaponsFlashlight"));
	PointLight->Mobility = EComponentMobility::Movable;
	PointLight->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	PointLight->SetVisibility(false);
	PointLight->UpdateColorAndBrightness();
	PointLight->AttenuationRadius = 200.f;
	PointLight->SetRelativeLocation(FVector(50.f, 0.f, 0.f));
	PointLight->SetupAttachment(GetRootComponent());

	PrimaryActorTick.bCanEverTick = true;

}
// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Aitem::BeginPlay();

	SetActiveStars();

	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
	CreateMesh();
	CreateLightFromBP("FlashLightSocket");

	// Set Item properties based on ItemState
	SetItemProperties(ItemState);
}

/*핸드 라이트 블루프린트 객체를 생성 후 총의 BarrelSocket에 부착한다.*/
void AWeapon::CreateLightFromBP(const FString& socketName)
{
	UObject* LightObject = Cast<UObject>(StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Game/Voyager/Blueprints/BP_FlashLightWeapon.BP_FlashLightWeapon")));

	UBlueprint* GeneratedLightBP = Cast<UBlueprint>(LightObject);
	if (!GeneratedLightBP)
		return;
	UClass* LightClass = GeneratedLightBP->StaticClass();
	if (LightClass == NULL)
		return;

	UWorld* World = GetWorld();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	/*호출되면 항상 스폰되도록 설정한다.*/
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AActor* actor = World->SpawnActor<AActor>(GeneratedLightBP->GeneratedClass, GetActorLocation(), GetActorRotation(), SpawnParams);
	actor->AttachToComponent(ItemMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, *socketName);

}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent,
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
			myCharacter->IncrementOverlappedItemCount(1);
		}
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMyCharacter* myCharacter = Cast<AMyCharacter>(OtherActor);
		if (myCharacter)
		{
			myCharacter->IncrementOverlappedItemCount(-1);
		}
	}
}

/*케릭터 클래스에서 무기 타이머가 종료되면 라이트를 끈다*/
void AWeapon::TurnLight(bool bLight)
{
	if (PointLight)
	{
		PointLight->SetVisibility(bLight);
	}
}

/*케릭터 클래스에서 호출하는 총의 라이팅 효과 토글*/
void AWeapon::ToggleLight()
{
	if (PointLight)
	{
		PointLight->SetVisibility(!PointLight->IsVisible());
	}
}

/*총 메시를 생성한다.*/
void AWeapon::CreateMesh()
{
	//Get Mesh
	USkeletalMesh* GunMesh = Cast<USkeletalMesh>(StaticLoadObject(USkeletalMesh::StaticClass(), NULL, TEXT("/Game/MyProject/Weapons/Meshs/SK_HeavyMachineGun.SK_HeavyMachineGun")));
	ItemMesh->SetSkeletalMesh(GunMesh);
	SetRootComponent(ItemMesh);
}

const USkeletalMeshSocket* AWeapon::GetBarrelSocket()
{
	return ItemMesh->GetSocketByName("BarrelSocket");;
}

FTransform AWeapon::GetSocketTransform()
{
	const USkeletalMeshSocket* MeshSocket = ItemMesh->GetSocketByName("BarrelSocket");
	return MeshSocket->GetSocketTransform(ItemMesh);
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetItemState() == EItemState::EIS_Falling && bFalling)
	{
		const FRotator MeshRotation{ 0.f, GetItemMesh()->GetComponentRotation().Yaw + 10.f, 0.f };
		GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}

	ItemInterp(DeltaTime);
}

void AWeapon::ThrowWeapon()
{
	FRotator MeshRotation{ 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f };
	GetItemMesh()->SetWorldRotation(MeshRotation, false, &WeaponeHitResult, ETeleportType::ResetPhysics);
	
	const FVector MeshForward{ GetItemMesh()->GetForwardVector() };
	const FVector MeshRight{ GetItemMesh()->GetRightVector() };
	// Direction in which we throw the Weapon
	FVector ImpulseDirection = MeshRight.RotateAngleAxis(-20.f, MeshForward);
	float RandomRotation{ 10.f };
	ImpulseDirection = ImpulseDirection.RotateAngleAxis(RandomRotation, FVector(0.f, 0.f, 1.f));
	ImpulseDirection.X *= 100.f;
	ImpulseDirection.Y *= 20'000.f;
	ImpulseDirection.Z *= 100.f;
	GetItemMesh()->AddImpulse(ImpulseDirection);

	bFalling = true;

	GetWorldTimerManager().SetTimer(
		ThrowWeaponTimer, 
		this, 
		&AWeapon::StopFalling, 
		ThrowWeaponTime);
}


void AWeapon::StopFalling()
{
	bFalling = false;

	SetItemState(EItemState::EIS_Pickup);
}

void AWeapon::SetItemState(EItemState State)
{
	ItemState = State;
	SetItemProperties(State);
}

void AWeapon::StartItemCurve(AMyCharacter* Char)
{
	/* Store a handle to the Charater*/
	Character = Char;
	ItemInterpStartLocation = GetActorLocation();
	bInterping = true;
	SetItemState(EItemState::EIS_EquipInterping);

	GetWorldTimerManager().SetTimer(
		ItemInterpTimer, 
		this, 
		&AWeapon::FinishInterping,
		ZCurveTime);

	// Get initial Yaw of the Camera
	const double CameraRotationYaw{ Character->GetFollowCamera()->GetComponentRotation().Yaw };
	// Get initial Yaw of the Item
	const double ItemRotationYaw{ GetActorRotation().Yaw };
	// Initial Yaw offset between Camera and Item
	InterpInitialYawOffset = ItemRotationYaw - CameraRotationYaw;
}

void AWeapon::FinishInterping()
{
	bInterping = false;

	if (Character)
	{
		Character->GetPickupItem(this);
	}
	// Set scale back to normal
	SetActorScale3D(FVector(1.f));
}

void AWeapon::ItemInterp(float DeltaTime)
{
	if (!bInterping)
		return;

	if (Character && ItemZCurve)
	{
		// Elapsed time since we started ItemInterpTimer
		const float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);
		// Get curve value crrespoding to ElapsedTime
		const float CurveValue = ItemZCurve->GetFloatValue(ElapsedTime);
		// Get the itme's initial location when the curve started
		FVector ItemLocation = ItemInterpStartLocation;
		// Get location in front of the camera
		const FVector CameraInterpLocation{ Character->GetHandInterpLocation() };

		// Vector from Item to Camera Interp Location, X and Y are zerod out
		const FVector ItemToCamera{ FVector(0.f, 0.f, (CameraInterpLocation - ItemLocation).Z) };
		// Scale factor to multiply with CurveValue
		const float DeltaZ = ItemToCamera.Size();

		const FVector CurrentLocation{ GetActorLocation() };
		const float InterpXValue = FMath::FInterpTo(
			CurrentLocation.X, 
			CameraInterpLocation.X, 
			DeltaTime, 
			1.f);
		//Interpolated X value
		const float InterpYValue = FMath::FInterpTo(
			CurrentLocation.Y,
			CameraInterpLocation.Y,
			DeltaTime,
			1.f);

		//Interpolated Y value
		
		// Set X and Y of ItemLocation to Interped values;
		ItemLocation.X = InterpXValue;
		ItemLocation.Y = InterpYValue;

		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("%f %f"), ItemLocation.X, ItemLocation.Y));

		// Adding curve value to the Z Component of the Initial Locatio (scaled by BeltaZ)
		ItemLocation.Z += CurveValue * DeltaZ;

		SetActorLocation(ItemLocation, true, nullptr, ETeleportType::TeleportPhysics);

		/* Camera rotation this frame*/
		const FRotator CameraRotation{ Character->GetFollowCamera()->GetComponentRotation() };
		/* Camera rotation plus inital Yaw Offset*/
		FRotator ItemRotation{ 0.f, CameraRotation.Yaw + InterpInitialYawOffset, 0.f };
		SetActorRotation(ItemRotation, ETeleportType::TeleportPhysics);

		if (ItemScaleCurve)
		{
			const float ScaleCurveValue = ItemScaleCurve->GetFloatValue(ElapsedTime);
			SetActorScale3D(FVector(ScaleCurveValue, ScaleCurveValue, ScaleCurveValue));
		}
	}
}