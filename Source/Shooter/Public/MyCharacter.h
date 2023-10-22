

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

UCLASS()
class SHOOTER_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TrunAtRate(float Rate);
	void LookUpAtRate(float Rate);
	void FireWeapon();
	void CalculateCrosshairSpread(float DeltaTime);
	void CalculateCameraMove(float DeltaTime);
	void FireButtonPressed();
	void FireButtonReleased();

	UFUNCTION()
	void AutoFireReset();
	UFUNCTION()
	void AutoFireLightOnOff();
	UFUNCTION()
	void AutoCameraShakeRollFunc();

	bool TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation);

	/* Trace for items if OverlappedItemCount > 0*/
	void TraceForItems();
	void TraceForTrigger();

	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation);
	virtual void NotifyActorBeginOverlap(AActor* OtherActor);
	void StartFireTimer();

	void AimingButtonPressed();
	void AimingButtonReleased();

	void InteractObstaclePressed();
	void SetLookUpRate();

	void Turn(float Value);
	void LookUp(float Value);

	void CameraMoveLeftPressed();
	void CameraMoveLeftRelease();

	void CameraMoveRightPressed();
	void CameraMoveRightRelease();

	void SelectButtonPressed();
	void SelectButtonReleased();

	void StartCrosshairBulletFire();
	UFUNCTION(BlueprintCallable)
	void FinishCrosshairBulletFire();

	class AWeapon* SpawnDefaultWeapon();

	/* Takes a weapon and attaches it to the mesh*/
	void EquipWeapon(class AWeapon* WeaponToEquip);

	/* Detach weapon and let it fall to the ground*/
	void DropWeapon();

	/* Drops currently equipped Weapon and Euips TraceHitItem*/
	void SwapWeapon(AWeapon* WeaponToSwap);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
private:
	/* Camera boom positioning the camera beghin the character*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	class USpringArmComponent* CameraBoom;

	/* Camera taht follows the character*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	class UCameraComponent* FollowCamera;
public:

private:
	/* Returns CameraBoom subobject*/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/*Returns FollowCamera subobject*/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/* Base turn rate, in deg/sec. Other scaling may affect final turn rate*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	float BaseTurnRate;
	/*Base look up/down rate, in deg/sec. Other scaling may affect final turn rate*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"));
	float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"));
	class USoundCue* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"));
	class UAudioComponent* FireAudioComponent;

	/* Flahs spawnd at BarrelSocket*/
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	class UParticleSystem* MuzzleFlash;

	/* Flahs spawnd at BarrelSocket*/
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	class UParticleSystem* MuzzleFlash2;

	/*Montage for firing the weapon.*/
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	class UAnimMontage* HipFireMontage;

	/*Montage for firing the weapon.*/
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	class UAnimMontage* HipFireMontageWalk;

	/* Particles spawnd upon bullet impact*/
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	class UParticleSystem* ImpactParticles;

	/* Particles spawnd upon bullet impact*/
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	class UParticleSystem* ImpactWallParticles;

	/* Smoke trail for bullets */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	class UParticleSystem* BeamParticles;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	class UParticleSystem* BulletParticles;

	TArray<class UParticleSystemComponent*> ShootingParticle;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	FVector vRocketDir;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	TArray<float> RocketParticleSpeed;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	FVector BeamEndPoint;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	FVector Start;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	float fMoveSpeed;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"));
	bool bAiming;

	float CameraDefaultFOV;
	float CameraZoomedFOV;
	float CameraCurrentFOV;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"));
	float ZoomInterpSpeed;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"));
	float ZoomOutInterpSpeed;

	void CameraInterpZoom(float fDeltatime);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"));
	float HipTurnRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"));
	float HipLookUpRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"));
	float AimingTurnRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"));
	float AimingLookUpRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"));
	float MouseHipTurnRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"));
	float MouseAimingTurnRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"));
	float MouseHipLookUpRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"));
	float MouseAimingLookUpRate;

	FVector	vecCurrentCameraPos;
	FVector vecCamearCenterPos;

	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"));
	FVector	vecLeftCameraPos;
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"));
	FVector	vecRightCameraPos;

	bool bCameraLeftMove;
	bool bCameraRightMove;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	float CrosshairSpreadMultiplier;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	float CrosshairVelocityFactor;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	float CrosshairInAirFactor;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	float CrosshairAimFactor;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	float CrosshairShootingFactor;

	float ShootTimeDuration;
	bool bFiringBullet;
	FTimerHandle CrosshairShootTimer;

	bool bFireButtonPressed;

	bool bShouldFire;

	float AutomaticFireRate;
	float AutomaticFireLightRate;
	float AutoCameraShakeRollRate;

	FTimerHandle AutoFireTimer;

	/* True if we should trace every frame for items*/
	bool bShouldTraceForItems;

	/* True if we should trace every frame for Doors*/
	bool bShouldTraceForTrigger;

	/* Number of overlapped AItems*/
	int8 OverlappedItemCount;

	/* Number of overlapped Doors*/
	int8 OverlappedTriggerCount;

	/* The AItem we hit last frame*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"));
	class Aitem* TraceHitItemLastFrame;

	/* The ACDoor we hit last frame*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"));
	class ATrigger* TraceHitTriggerLastFrame;

	FTimerHandle AutoFireLightTimer;
	FTimerHandle AutoCameraShakeRoll;

	float fFiringAimOffset;
	FRotator rotFiringCameraShake;

	bool bInteractObjectPressed;

	/*Currently equipped Weapone*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"));
	class AWeapon* EquippedWeapon;

	/* Set this in Bluepinrts for the default Weapon class*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"));
	TSubclassOf<AWeapon> DefaultWeaponClass;

	/* The item currently hit by our trace in TraceForItem(could be null)*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"));
	Aitem* TraceHitItem;

	/* Distnace outward from the camera for the interp destination*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"));
	float CameraInterpDistance;

	/* Distnace upward form the camera for the interp destination*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"));
	float CameraInterpElevation;

public:
	FORCEINLINE bool GetAiming() const { return bAiming; }
	FORCEINLINE bool GetFireBullet() const { return bFiringBullet; }

	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;

	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }

	/* Adds/subtracts to/from OverlappedItemCount and update bSHouldTraceForItems*/
	void IncrementOverlappedItemCount(int8 Amount);
	void IncrementOverlappedTriggerCount(int8 Amount);

	FVector GetCameraInterpLocation();

	void GetPickupItem(Aitem* Item);
};
