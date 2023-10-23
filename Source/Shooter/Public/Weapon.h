

#pragma once

#include "CoreMinimal.h"
#include "item.h"
#include "Weapon.generated.h"

/**
 * 
 */

UCLASS()
class SHOOTER_API AWeapon : public Aitem
{
	GENERATED_BODY()
	
public:
	AWeapon();
private:
	void BeginPlay();
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Light, meta = (AllowPrivateAccess = "true"));
	class UPointLightComponent* PointLight;

private:
	void CreateMesh();
public:
	virtual void Tick(float DeltaTime) override;
protected:
	void CreateLightFromBP(const FString& socketName);
	void StopFalling();

	// Starting location when interping begins
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"));
	FVector ItemInterpStartLocation;

	// Target interp location in fornt of the camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"));
	FVector CameraTargetLocation;

	// true when interping
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"));
	bool bInterping;

	/* Plays when we start interping*/
	FTimerHandle ItemInterpTimer;

	/*Duration fo the curve and timer*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"));
	float ZCurveTime;

	/* The curve asset to use for the itme's Z location when interping*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"));
	class UCurveFloat* ItemZCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"));
	class UCurveFloat* ItemScaleCurve;

	/* Called when ItemInterpTImer is finished*/
	void FinishInterping();

	/* Handles item interpolation when in the EquipInterping state*/
	void ItemInterp(float DeltaTime);

	/* X and Y for the Item while interping in the EquipInterping*/
	float ItemInterpX;
	float ItemInterpY;

	/* Initial Yaw offset between the camera and the interping item*/
	float InterpInitialYawOffset;
private:
	FTimerHandle ThrowWeaponTimer;
	float ThrowWeaponTime;
	bool bFalling;
	FHitResult WeaponeHitResult;

public:
	void TurnLight(bool bLight);
	void ToggleLight();
	const class USkeletalMeshSocket* GetBarrelSocket();
	FTransform GetSocketTransform();
	/* Adds an impulse to the Weapon*/
	void ThrowWeapon();
	/* Called from the AShooterCharacet class*/
	void StartItemCurve(AMyCharacter* Char);

	void SetItemState(EItemState State);
};
