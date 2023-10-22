

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)

	void UpdateAnimationProperties(float DeltaTime);
	virtual void NativeInitializeAnimation() override;

private:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class AMyCharacter* MyCharacter;

	/*The speed of the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"));
	float Speed;
	/* Whether or not the chracter is in the air*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"));
	bool bIsInAir;
	/*Wheter or not the chracter is moving*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"));
	bool bIsAccelerating;

	/*Montage for firing the weapon.*/
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	class UAnimMontage* HipFireMontage;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"));
	float MovementOffsetYaw;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"));
	bool bAiming;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"));
	bool bFire;
public :
	bool GetAccelearting() { return bIsAccelerating; }
};
