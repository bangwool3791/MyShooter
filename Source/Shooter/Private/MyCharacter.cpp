


#include "MyCharacter.h"
#include "GameFramework\SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Containers/List.h"
#include "GameFramework/Actor.h"
#include "MyAnimInstance.h"
#include <Engine\ObjectLibrary.h>
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/HUD.h"
#include "item.h"
#include "CDoor.h"
#include "Components/WidgetComponent.h"
#include "Weapon.h"
#include "Trigger.h"
#include "Bullet.h"
// Sets default values

AMyCharacter::AMyCharacter()
	:BaseTurnRate(0.f)
	, BaseLookUpRate(0.f)
	, fMoveSpeed(1.5f)
	, bAiming(false)
	, CameraDefaultFOV(0.f)
	, CameraZoomedFOV(35)
	, ZoomInterpSpeed(10)
	, ZoomOutInterpSpeed(2)
	, HipTurnRate(90.f)
	, HipLookUpRate(90.f)
	, AimingTurnRate(20.f)
	, AimingLookUpRate(20.f)
	, MouseHipTurnRate(1.0f)
	, MouseAimingTurnRate(0.4f)
	, MouseHipLookUpRate(1.0f)
	, MouseAimingLookUpRate(0.4f)
	, ShootTimeDuration(0.3f)
	, bFiringBullet(false)
	// Automatic gun fire rate
	, AutomaticFireRate(0.1f)
	, AutomaticFireLightRate(0.05f)
	, bShouldFire(true)
	, bFireButtonPressed(false)
	, AutoCameraShakeRollRate(0.1f)
	// Item trace variables
	, bShouldTraceForItems(false)
	, TraceHitItem(nullptr)
	, CameraInterpDistance(250.f)
	, CameraInterpElevation(65.f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create a camera boom (pulls in towards the character if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 200.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.f, 80.f, 80.f);
	//Create a follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);//Attach camera to end of boom
	FollowCamera->bUsePawnControlRotation = false; //Camera dose not rotate relative to arm

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = true;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	//GunLightBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	//GunLightBoom->SetupAttachment(RootComponent);
	//GunLightBoom->TargetArmLength = 300.f;
	//GunLightBoom->bUsePawnControlRotation = true;
	//GunLightBoom->SocketOffset = FVector(0.f, 80.f, 80.f);
	static ConstructorHelpers::FObjectFinder<USoundCue> propellerCue(
		TEXT("/Game/Voyager/Demo/Audio/Weapons/AssaultRifle/SCue_Weapons_Rifle_Noise-Exterior-Close_01.SCue_Weapons_Rifle_Noise-Exterior-Close_01")
	);

	FireSound = propellerCue.Object;

	ConstructorHelpers::FObjectFinder<UParticleSystem> MuzzleFlashAsset(
		TEXT("/Game/Voyager/Demo/Particles/P_AssaultRifleMuzzle.P_AssaultRifleMuzzle")
	);
	MuzzleFlash = MuzzleFlashAsset.Object;

	ConstructorHelpers::FObjectFinder<UParticleSystem> MuzzleFlashAsset2(
		TEXT("/Game/Voyager/Demo/Particles/P_AssaultRifleMuzzle.P_AssaultRifleMuzzle2")
	);
	MuzzleFlash2 = MuzzleFlashAsset2.Object;

	ConstructorHelpers::FObjectFinder<UAnimMontage> AnimationMontage(
		TEXT("/Game/MyProject/HipFireMontage.HipFireMontage")
	);
	HipFireMontage = AnimationMontage.Object;

	ConstructorHelpers::FObjectFinder<UAnimMontage> AnimationMontageWalk(
		TEXT("/Game/MyProject/HipFireMontage_Walk.HipFireMontage_Walk")
	);
	HipFireMontageWalk = AnimationMontage.Object;

	ConstructorHelpers::FObjectFinder<UParticleSystem> ImpactParticleAsset(
		TEXT("/Game/Voyager/Demo/Levels/UndergroundBaseLevel/Particles/P_spark_burst.P_spark_burst")
	);

	if (ImpactParticleAsset.Succeeded())
		ImpactParticles = ImpactParticleAsset.Object;

	/// Script / Engine.ParticleSystem'/Game/MyProject/Assets/SmokeBeam/P_SmokeTrail_Faded.P_SmokeTrail_Faded'
	ConstructorHelpers::FObjectFinder<UParticleSystem> ImpactWallParticleAsset(
		TEXT("/Game/ParagonLtBelica/FX/Particles/Belica/Abilities/Primary/FX/P_BelicaHitWorld.P_BelicaHitWorld")
	);

	if (ImpactWallParticleAsset.Succeeded())
	{
		ImpactWallParticles = ImpactWallParticleAsset.Object;
	}

	/// Script / Engine.ParticleSystem'/Game/Voyager/Demo/Particles/Effects/FX_Mobile/Fire/combat/P_SlimeBall.P_SlimeBall'
	ConstructorHelpers::FObjectFinder<UParticleSystem> BulletParticlesAsset(
		TEXT("/Game/Voyager/Demo/Particles/Effects/FX_Mobile/Fire/combat/P_SlimeBall.P_SlimeBall")
	);

	BulletParticles = BulletParticlesAsset.Object;

	ConstructorHelpers::FObjectFinder<UParticleSystem> BeamParticlesAsset(
		TEXT("/Game/MyProject/Assets/SmokeBeam/P_SmokeTrail_Faded.P_SmokeTrail_Faded")
	);

	BeamParticles = BeamParticlesAsset.Object;


	{
		// Create an audio component, the audio component wraps the Cue, 
		// and allows us to ineract with
		// it, and its parameters from code.
		FireAudioComponent = CreateDefaultSubobject<UAudioComponent>(
			TEXT("PropellerAudioComp")
			);
		// I don't want the sound playing the moment it's created.
		FireAudioComponent->bAutoActivate = false;
		FireAudioComponent->bReverb = true;
		// I want the sound to follow the pawn around, so I attach it to the Pawns root.
		FireAudioComponent->AutoAttachParent = RootComponent;
		// I want the sound to come from slighty in front of the pawn.
		FireAudioComponent->SetRelativeLocation(FVector(0.f, 0.0f, 0.f));
		// Attach our sound cue to the SoundComponent (outside the constructor)
		if (FireAudioComponent->IsValidLowLevelFast()) {
			FireAudioComponent->SetSound(FireSound);
		}
	}

	ShootingParticle.SetNum(1000, false);

	vecCamearCenterPos = FVector(90.f, -10.f, -10.f);
	vecRightCameraPos = FVector(200.f, 90.f, -10.f);
	vecLeftCameraPos = FVector(200.f, -110.f, -10.f);
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (FollowCamera)
	{
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}

	// Spawn the default weapon and attach it to the mesh
	SpawnDefaultWeapon();
}

void AMyCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && Value != 0.0f)
	{
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0.f, Rotation.Yaw, 0 };
		const FVector Direction{ FRotationMatrix{YawRotation }.GetUnitAxis(EAxis::X) };
		AddMovementInput(Direction, Value * fMoveSpeed);
	}
}

void AMyCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && Value != 0.0f)
	{
		const FRotator Rotation{ Controller->GetControlRotation() };
		const FRotator YawRotation{ 0.f, Rotation.Yaw, 0 };
		const FVector Direction{ FRotationMatrix{YawRotation }.GetUnitAxis(EAxis::Y) };
		AddMovementInput(Direction, Value * fMoveSpeed);
	}
}

void AMyCharacter::TrunAtRate(float Rate)
{
	if (!Rate)
		BaseTurnRate = 0.f;

	BaseTurnRate = FMath::FInterpTo(
		BaseTurnRate,
		45,
		GetWorld()->GetDeltaSeconds(),
		0.01);

	AddControllerYawInput(Rate * BaseTurnRate);// deg/sec * sec/frame
}

void AMyCharacter::Turn(float Rate)
{
	float TurnScaleFactor{};

	if (bAiming)
	{
		TurnScaleFactor = MouseAimingTurnRate;
	}
	else
	{
		TurnScaleFactor = MouseHipTurnRate;
	}

	if (!Rate)
		BaseTurnRate = 0.f;

	BaseTurnRate = FMath::FInterpTo(
		BaseTurnRate,
		Rate,
		GetWorld()->GetDeltaSeconds(),
		30);

	AddControllerYawInput(TurnScaleFactor * BaseTurnRate);
}

void AMyCharacter::LookUp(float Rate)
{
	float TurnScaleFactor{};

	if (bAiming)
	{
		TurnScaleFactor = MouseAimingLookUpRate;
	}
	else
	{
		TurnScaleFactor = MouseHipLookUpRate;
	}

	if (!Rate)
		BaseLookUpRate = 0.f;

	BaseLookUpRate = FMath::FInterpTo(
		BaseLookUpRate,
		Rate,
		GetWorld()->GetDeltaSeconds(),
		30);

	AddControllerPitchInput(TurnScaleFactor * BaseLookUpRate);// deg/sec * sec/frame
}

void AMyCharacter::LookUpAtRate(float Rate)
{
	if (!Rate)
		BaseLookUpRate = 0.f;

	BaseLookUpRate = FMath::FInterpTo(
		BaseLookUpRate,
		45,
		GetWorld()->GetDeltaSeconds(),
		0.01);

	AddControllerYawInput(Rate * BaseLookUpRate);// deg/sec * sec/frame
}

void AMyCharacter::FireWeapon()
{
	static bool bMuzzelType = false;

	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}

	if (EquippedWeapon)
	{
		const USkeletalMeshSocket* BarrelSocket = EquippedWeapon->GetBarrelSocket();

		if (BarrelSocket)
		{
			FTransform SocketTransform = EquippedWeapon->GetSocketTransform();
			FVector3d vecScale(1.f, 1.f, 1.f);
			SocketTransform.SetScale3D(vecScale);

			if (bMuzzelType)
			{
				if (MuzzleFlash)
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
			}
			else
			{
				if (MuzzleFlash2)
				{
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash2, SocketTransform);
				}
			}
			bMuzzelType = !bMuzzelType;

			bool bBeamEnd = GetBeamEndLocation(
				SocketTransform.GetLocation(),
				BeamEndPoint);

			if (bBeamEnd)
			{
				if (ImpactParticles && ImpactWallParticles)
				{
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactWallParticles, BeamEndPoint);
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, BeamEndPoint);
				}


				if (BeamParticles)
				{
					UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, SocketTransform);

					if (Beam)
						Beam->SetVectorParameter(FName("Target"), BeamEndPoint);
				}

				if (BulletParticles)
				{
					//UParticleSystemComponent* Bullet = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletParticles, SocketTransform);
					//
					//if (!Bullet)
					//{
					//	Bullet->SetVectorParameter(FName("Target"), BeamEndPoint);
					//	Bullet->SetWorldLocation(Start);
					//	
					//	for (int i = 0; i < ShootingParticle.Num(); ++i)
					//	{
					//		if (ShootingParticle[i] == nullptr)
					//		{
					//			ShootingParticle[i] = Bullet;
					//			break;
					//		}
					//	}
					//	
					//	FVector vDir = BeamEndPoint - Start;
					//	double dDist = BeamEndPoint.Dist(BeamEndPoint, Start);
					//	vRocketDir = vDir / dDist;
					//}
					UObject* LightObject = Cast<UObject>(StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Game/MyProject/Weapons/Bullet/Bullet.Bullet")));

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
					FVector vDir = BeamEndPoint - Start;
					vDir.Normalize();

					FRotator Rot = vDir.Rotation().Quaternion().Rotator();
					FRotator ActorRot = GetActorRotation();
					ActorRot.Add(Rot.Pitch, Rot.Roll, Rot.Yaw);
					FVector Pos = GetActorLocation();

					ABullet* actor = World->SpawnActor<ABullet>(GeneratedLightBP->GeneratedClass, SocketTransform.GetLocation(), Rot, SpawnParams);
					actor->FireInDirection(vDir);
					//actor->AttachToComponent(EquippedWeapon->GetItemMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("BarrelSocket"));
				}
			}
		}
	}

	UMyAnimInstance* AnimInstance = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());

	if (AnimInstance)
	{
		if (AnimInstance->GetAccelearting())
		{
			if (HipFireMontageWalk)
			{
				AnimInstance->Montage_Play(HipFireMontageWalk);
				AnimInstance->Montage_JumpToSection(FName("StartFire"));
			}
		}
		else
		{
			if (HipFireMontage)
			{
				AnimInstance->Montage_Play(HipFireMontage);
				AnimInstance->Montage_JumpToSection(FName("StartFire"));
				UE_LOG(LogTemp, Warning, TEXT("Fire Weapon."));
			}
		}
	}

	StartCrosshairBulletFire();
}

void AMyCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (int i = 0; i < ShootingParticle.Num(); ++i)
	{
		UParticleSystemComponent* Component = ShootingParticle[i];
	
		if (Component)
		{
			if (!Component->bWasDeactivated)
			{
				Component->SetWorldRotation(vRocketDir.ToOrientationQuat());
				const FTransform ComponentTransform = Component->GetRelativeTransform();
				FVector vCurrentLocation = ComponentTransform.GetLocation();
	
				Component->SetWorldLocation(vCurrentLocation + vRocketDir * DeltaTime * 3000.f);
			}
			else
			{
				ShootingParticle[i] = nullptr;
			}
		}
	}

	CalculateCameraMove(DeltaTime);
	CameraInterpZoom(DeltaTime);
	SetLookUpRate();
	CalculateCrosshairSpread(DeltaTime);
	TraceForItems();
	TraceForTrigger();
}

void AMyCharacter::TraceForItems()
{
	if (bShouldTraceForItems)
	{
		FHitResult ItemTraceResult;
		FVector HitLocation;
		TraceUnderCrosshairs(ItemTraceResult, HitLocation);

		if (ItemTraceResult.bBlockingHit)
		{
			TraceHitItem = Cast<Aitem>(ItemTraceResult.GetActor());

			if (TraceHitItem && TraceHitItem->GetPickupWidget())
			{
				TraceHitItem->GetPickupWidget()->SetVisibility(true);
			}

			// We hit an AItem last frame
			if (TraceHitItemLastFrame)
			{
				if (TraceHitItem != TraceHitItemLastFrame)
				{
					// We are hitting a different AItem this frame form last frame
					// Or AItem is null
					TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
				}
			}
			// Store a reference to HitItem for next frame
			TraceHitItemLastFrame = TraceHitItem;
		}
		else
		{
			// No longer overlapping any items,
			// Item last frame shold not show widget
			if(TraceHitItemLastFrame)
			TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
		}
	}
	else
	{
		if (TraceHitItemLastFrame)
		{
			TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
			TraceHitItemLastFrame = nullptr;
		}
	}
}

void AMyCharacter::TraceForTrigger()
{
	if (bShouldTraceForTrigger)
	{
		FHitResult ItemTraceResult;
		FVector HitLocation;
		TraceUnderCrosshairs(ItemTraceResult, HitLocation);

		if (ItemTraceResult.bBlockingHit)
		{
			ATrigger* HitItem = Cast<ATrigger>(ItemTraceResult.GetActor());

			if (HitItem && HitItem->GetPickupWidget())
			{
				HitItem->GetPickupWidget()->SetVisibility(true);
			}

			// We hit an AItem last frame
			if (TraceHitTriggerLastFrame)
			{
				if (HitItem != TraceHitTriggerLastFrame)
				{
					// We are hitting a different AItem this frame form last frame
					// Or AItem is null
					TraceHitTriggerLastFrame->GetPickupWidget()->SetVisibility(false);
				}
			}
			// Store a reference to HitItem for next frame
			TraceHitTriggerLastFrame = HitItem;
		}
		else
		{
			// No longer overlapping any items,
			// Item last frame shold not show widget
			if(TraceHitTriggerLastFrame)
				TraceHitTriggerLastFrame->GetPickupWidget()->SetVisibility(false);
		}
	}
	else
	{
		if (TraceHitTriggerLastFrame)
		{
			TraceHitTriggerLastFrame->GetPickupWidget()->SetVisibility(false);
			TraceHitTriggerLastFrame = nullptr;
		}
	}
}

void AMyCharacter::SetLookUpRate()
{
	if (bAiming)
	{
		BaseTurnRate = AimingTurnRate;
		BaseLookUpRate = AimingLookUpRate;;
	}
	else
	{
		BaseTurnRate = HipTurnRate;
		BaseLookUpRate = HipLookUpRate;
	}
}

void AMyCharacter::CameraInterpZoom(float DeltaTime)
{

	if (bAiming)
	{
		CameraCurrentFOV = FMath::FInterpTo(
			CameraCurrentFOV,
			CameraZoomedFOV,
			DeltaTime,
			ZoomInterpSpeed);

		GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
	}
	else
	{
		CameraCurrentFOV = FMath::FInterpTo(
			CameraCurrentFOV,
			CameraDefaultFOV,
			DeltaTime,
			ZoomOutInterpSpeed);

		GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
	}
}

bool AMyCharacter::GetBeamEndLocation(
	const FVector& MuzzleSocketLocation,
	FVector& OutBeamLocation)
{
	// Check for crosshair trace hit
	FHitResult CrosshairHitResult;
	bool bCrosshairHit = TraceUnderCrosshairs(CrosshairHitResult, OutBeamLocation);

	if (bCrosshairHit)
	{
		// Tentative beam location - still need to trace from gun
		OutBeamLocation = CrosshairHitResult.Location;
	}
	else // no crosshair trace hit
	{
		// OutBeam Location is the End location for the line trace
	}

	// Perform trace from gun barrel
	FHitResult WeaponTraceHit;
	Start = MuzzleSocketLocation;
	const FVector WeaponTraceStart{ MuzzleSocketLocation };
	const FVector StartToEnd{ OutBeamLocation - MuzzleSocketLocation };
	const FVector WeaponTraceEnd{ MuzzleSocketLocation + StartToEnd * 1.25f };
	GetWorld()->LineTraceSingleByChannel(
		WeaponTraceHit,
		WeaponTraceStart,
		WeaponTraceEnd,
		ECollisionChannel::ECC_Visibility);

	if (WeaponTraceHit.bBlockingHit)
	{
		OutBeamLocation = WeaponTraceHit.Location; //object between brrel and BeamEndPoint?
		return true;
	}
	return false;
}

void AMyCharacter::AimingButtonPressed()
{
	bAiming = true;
}

void AMyCharacter::AimingButtonReleased()
{
	bAiming = false;
}

void AMyCharacter::InteractObstaclePressed()
{
	if (TraceHitTriggerLastFrame)
	{
		ACDoor* DoorActor = nullptr; 
		DoorActor = TraceHitTriggerLastFrame->GetDoorActor();

		if (DoorActor)
			DoorActor->OpencloseDoor();
	}
}
void AMyCharacter::CameraMoveLeftPressed()
{
	bCameraLeftMove = true;
}

void AMyCharacter::CameraMoveLeftRelease()
{
	bCameraLeftMove = false;
}

void AMyCharacter::CameraMoveRightPressed()
{
	bCameraRightMove = true;
}

void AMyCharacter::CameraMoveRightRelease()
{
	bCameraRightMove = false;
}

void AMyCharacter::FireButtonPressed()
{
	bFireButtonPressed = true;
	StartFireTimer();
}

void AMyCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void AMyCharacter::StartFireTimer()
{
	if (bShouldFire)
	{
		FireWeapon();
		bShouldFire = false;
		GetWorldTimerManager().SetTimer(
			AutoFireTimer,
			this,
			&AMyCharacter::AutoFireReset,
			AutomaticFireRate);

		GetWorldTimerManager().SetTimer(
			AutoFireLightTimer,
			this,
			&AMyCharacter::AutoFireLightOnOff,
			AutomaticFireLightRate);

		GetWorldTimerManager().SetTimer(
			AutoCameraShakeRoll,
			this,
			&AMyCharacter::AutoCameraShakeRollFunc,
			AutoCameraShakeRollRate);
	}
}

void AMyCharacter::AutoFireReset()
{
	bShouldFire = true;

	if (bFireButtonPressed)
	{
		StartFireTimer();
	}
	else
	{
		if (EquippedWeapon)
		{
			EquippedWeapon->TurnLight(false);
		}

		fFiringAimOffset = 0.f;
		rotFiringCameraShake.Pitch = 0.f;
		rotFiringCameraShake.Roll = 0.f;
		FollowCamera->SetRelativeRotation(rotFiringCameraShake);
	}
}

void AMyCharacter::AutoCameraShakeRollFunc()
{
	double dYaw = FollowCamera->GetRelativeRotation().Yaw;
	static bool bYaw = false;

	if (bYaw)
	{
		rotFiringCameraShake.Roll = -0.6;
		rotFiringCameraShake.Pitch = 0.6;
	}
	else
	{
		rotFiringCameraShake.Roll = 0.6;
		rotFiringCameraShake.Pitch = -0.6;
	}

	bYaw = !bYaw;
	FollowCamera->SetRelativeRotation(rotFiringCameraShake);
}

void AMyCharacter::AutoFireLightOnOff()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->ToggleLight();
	}
}

void AMyCharacter::CalculateCrosshairSpread(float DeltaTime)
{
	FVector2D WalkSpeedRange{ 0.f, 600.f };
	FVector2D VelocityMultiplierRange{ 0.f, 1.f };
	FVector Velocity{ GetVelocity() };
	Velocity.Z = 0.f;

	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped
	(WalkSpeedRange,
		VelocityMultiplierRange,
		Velocity.Size());

	if (GetCharacterMovement()->IsFalling())
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
	}
	else
	{
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
	}

	if (bAiming)
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.6f, DeltaTime, 30.f);
	}
	else
	{
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
	}

	if (bFiringBullet)
	{
		CrosshairShootingFactor = FMath::FInterpTo(
			CrosshairShootingFactor,
			1.f,
			DeltaTime,
			1.f);
	}
	else
	{
		CrosshairShootingFactor = FMath::FInterpTo(
			CrosshairShootingFactor,
			0.f,
			DeltaTime,
			2.f);
	}

	CrosshairSpreadMultiplier =
		1.f +
		CrosshairVelocityFactor +
		CrosshairInAirFactor -
		CrosshairAimFactor +
		CrosshairShootingFactor;
}

void AMyCharacter::CalculateCameraMove(float DeltaTime)
{
	if (bCameraLeftMove)
	{
		vecCurrentCameraPos.X = FMath::FInterpTo(
			vecCurrentCameraPos.X,
			vecLeftCameraPos.X,
			DeltaTime,
			ZoomInterpSpeed);

		vecCurrentCameraPos.Y = FMath::FInterpTo(
			vecCurrentCameraPos.Y,
			vecLeftCameraPos.Y,
			DeltaTime,
			ZoomInterpSpeed);

		vecCurrentCameraPos.Z = FMath::FInterpTo(
			vecCurrentCameraPos.Z,
			vecLeftCameraPos.Z,
			DeltaTime,
			ZoomInterpSpeed);
	}
	else if (bCameraRightMove)
	{
		vecCurrentCameraPos.X = FMath::FInterpTo(
			vecCurrentCameraPos.X,
			vecRightCameraPos.X,
			DeltaTime,
			ZoomInterpSpeed);

		vecCurrentCameraPos.Y = FMath::FInterpTo(
			vecCurrentCameraPos.Y,
			vecRightCameraPos.Y,
			DeltaTime,
			ZoomInterpSpeed);

		vecCurrentCameraPos.Z = FMath::FInterpTo(
			vecCurrentCameraPos.Z,
			vecRightCameraPos.Z,
			DeltaTime,
			ZoomInterpSpeed);
	}
	else
	{
		vecCurrentCameraPos.X = FMath::FInterpTo(
			vecCurrentCameraPos.X,
			vecCamearCenterPos.X,
			DeltaTime,
			ZoomInterpSpeed);

		vecCurrentCameraPos.Y = FMath::FInterpTo(
			vecCurrentCameraPos.Y,
			vecCamearCenterPos.Y,
			DeltaTime,
			ZoomInterpSpeed);

		vecCurrentCameraPos.Z = FMath::FInterpTo(
			vecCurrentCameraPos.Z,
			vecCamearCenterPos.Z,
			DeltaTime,
			ZoomInterpSpeed);
	}
	FollowCamera->SetRelativeLocation(vecCurrentCameraPos);
}
float AMyCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplier;
}

void AMyCharacter::StartCrosshairBulletFire()
{
	bFiringBullet = true;

	GetWorldTimerManager().SetTimer(
		CrosshairShootTimer,
		this,
		&AMyCharacter::FinishCrosshairBulletFire,
		ShootTimeDuration);
}

void AMyCharacter::FinishCrosshairBulletFire()
{
	bFiringBullet = false;
}

bool AMyCharacter::TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation)
{
	// Get Viewport Size
	FVector2D ViewportsSize;

	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportsSize);
	}

	FVector2D CrosshairLocation(ViewportsSize.X / 2.f, ViewportsSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		// Trace from Crosshair world location outward

		Start = CrosshairWorldPosition;
		const FVector End{ Start + CrosshairWorldDirection * 50'000.f };
		BeamEndPoint = OutHitLocation = End;
		GetWorld()->LineTraceSingleByChannel(
			OutHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility);

		if (OutHitResult.bBlockingHit)
		{
			BeamEndPoint = OutHitLocation = OutHitResult.Location;
			return true;
		}
	}

	return false;
}

void AMyCharacter::IncrementOverlappedItemCount(int8 Amount)
{
	if (OverlappedItemCount + Amount <= 0)
	{
		OverlappedItemCount = 0;
		bShouldTraceForItems = false;
	}
	else
	{
		OverlappedItemCount += Amount;
		bShouldTraceForItems = true;
	}
}

void AMyCharacter::IncrementOverlappedTriggerCount(int8 Amount)
{
	if (OverlappedTriggerCount + Amount <= 0)
	{
		OverlappedTriggerCount = 0;
		bShouldTraceForTrigger = false;
	}
	else
	{
		OverlappedTriggerCount += Amount;
		bShouldTraceForTrigger = true;
	}
}

void AMyCharacter::SelectButtonPressed()
{
	if (TraceHitItem)
	{
		auto TraceHitWeapon = Cast<AWeapon>(TraceHitItem);
		TraceHitWeapon->StartItemCurve(this);
	}
}

void AMyCharacter::SelectButtonReleased()
{

}

AWeapon* AMyCharacter::SpawnDefaultWeapon()
{
	//Spawn Weapon when using blueprint
	//// Check the TSubclassOf variable
	//if (DefaultWeaponClass)
	//{
	//	// Spawn the Weapon
	//	AWeapon* DefaultWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);

	//	// Get the Hand Socket
	//	const USkeletalMeshSocket* HandSokcet = GetMesh()->GetSocketByName(
	//		FName("hand_rSocket"));

	//	if (HandSokcet)
	//	{
	//		// Attach the Weapon to the hand socket RightHandSocket
	//		HandSokcet->AttachActor(DefaultWeapon, GetMesh());
	//	}
	//	EquippedWeapon = DefaultWeapon;
	//}

	UObject* WeaponObject = Cast<UObject>(StaticLoadObject(UObject::StaticClass(), NULL, TEXT("/Game/MyProject/Weapons/BaseWeapon/BaseWeapon.BaseWeapon")));

	UBlueprint* GeneratedBP = Cast<UBlueprint>(WeaponObject);
	if (!GeneratedBP)
	{
		return nullptr;
	}
	UClass* WeaponClass = GeneratedBP->StaticClass();

	if (WeaponClass == NULL)
	{
		return nullptr;
	}

	UWorld* World = GetWorld();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	/*호출되면 항상 스폰되도록 설정한다.*/
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	//무기를 월드에 스폰시킨다.
	AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(GeneratedBP->GeneratedClass, GetActorLocation(), GetActorRotation(), SpawnParams);

	if (DefaultWeapon == NULL)
		return nullptr;

	const USkeletalMeshSocket* HandSokcet = GetMesh()->GetSocketByName(
		FName("hand_rSocket"));

	if (HandSokcet == NULL)
	{
		return nullptr;
	}

	//무기를 핸드소켓에 장착시킨다.
	HandSokcet->AttachActor(DefaultWeapon, GetMesh());

	EquippedWeapon = DefaultWeapon;
	EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
	return DefaultWeapon;
}

/* Takes a weapon and attaches it to the mesh*/
void AMyCharacter::EquipWeapon(class AWeapon* WeaponToEquip)
{
	if (WeaponToEquip)
	{
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("hand_rSocket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(WeaponToEquip, GetMesh());
		}
		EquippedWeapon = WeaponToEquip;
		WeaponToEquip->SetItemState(EItemState::EIS_Equipped);
	}
}

/* Detach weapon and let it fall to the ground*/
void AMyCharacter::DropWeapon()
{
	if (EquippedWeapon)
	{
		FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);
		EquippedWeapon->SetItemState(EItemState::EIS_Falling);
		EquippedWeapon->ThrowWeapon();
		EquippedWeapon = nullptr;
	}
}

void AMyCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{
	DropWeapon();
	EquipWeapon(WeaponToSwap);
	TraceHitItem = nullptr;
	TraceHitItemLastFrame = nullptr;
}

FVector AMyCharacter::GetCameraInterpLocation()
{
	const FVector CameraWorldLocation{ FollowCamera->GetComponentLocation() };
	const FVector CameraForward{ FollowCamera->GetForwardVector() };
	// Desired = CameraWorldLocation + Forward * A + Up * B
	return CameraWorldLocation + CameraForward * CameraInterpDistance + FVector(0.f, 0.f, CameraInterpElevation);
}

FVector AMyCharacter::GetHandInterpLocation()
{
	const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("hand_rSocket"));
	if (HandSocket)
	{
		FQuat fquad;

		FTransform transfrom = HandSocket->GetSocketTransform(GetMesh());
		const FVector HandWorldLocation{ transfrom.GetLocation()};
		const FVector HandForward{ transfrom.TransformRotation(fquad).Vector().ForwardVector};
		// Desired = CameraWorldLocation + Forward * A + Up * B
		return HandWorldLocation + HandForward * HandInterpDistance + FVector(0.f, 0.f, HandInterpElevation);
	}
	return FVector();
}

void AMyCharacter::GetPickupItem(Aitem* Item)
{
	auto Weapon = Cast<AWeapon>(Item);

	if (Weapon)
	{
		SwapWeapon(Weapon);
	}
}
// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);

	PlayerInputComponent->BindAxis("TurnRate", this, &AMyCharacter::TrunAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMyCharacter::LookUpAtRate);

	PlayerInputComponent->BindAxis("Turn", this, &AMyCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &AMyCharacter::LookUp);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AMyCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("FireButton", IE_Released, this, &AMyCharacter::FireButtonReleased);

	PlayerInputComponent->BindAction("CameraMoveRight", IE_Pressed, this, &AMyCharacter::CameraMoveRightPressed);
	PlayerInputComponent->BindAction("CameraMoveRight", IE_Released, this, &AMyCharacter::CameraMoveRightRelease);

	PlayerInputComponent->BindAction("CameraMoveLeft", IE_Pressed, this, &AMyCharacter::CameraMoveLeftPressed);
	PlayerInputComponent->BindAction("CameraMoveLeft", IE_Released, this, &AMyCharacter::CameraMoveLeftRelease);

	PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this,
		&AMyCharacter::AimingButtonPressed);
	PlayerInputComponent->BindAction("AimingButton", IE_Released, this,
		&AMyCharacter::AimingButtonReleased);

	PlayerInputComponent->BindAction("InteractObstacle", IE_Pressed, this,
		&AMyCharacter::SelectButtonPressed);
	PlayerInputComponent->BindAction("InteractObstacle", IE_Released, this,
		&AMyCharacter::SelectButtonReleased);

	PlayerInputComponent->BindAction("InteractObstacle", IE_Pressed, this,
		&AMyCharacter::InteractObstaclePressed);
}