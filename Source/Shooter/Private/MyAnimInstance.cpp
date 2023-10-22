


#include "MyAnimInstance.h"
#include "MyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UMyAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (MyCharacter == nullptr)
	{
		MyCharacter = Cast<AMyCharacter>(TryGetPawnOwner());
	}

	if (MyCharacter)
	{
		// Get the lateral speed of the chracter from velocity
		FVector Velocity{ MyCharacter->GetVelocity() };
		Velocity.Z = 0.f;

		Speed = Velocity.Size();

		// Is the character in the air ?
		bIsInAir = MyCharacter->GetCharacterMovement()->IsFalling();

		// Is the chracter accerlating?
		if (MyCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0)
		{
			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}

		FRotator AimRotation = MyCharacter->GetBaseAimRotation();
		FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(MyCharacter->GetVelocity());

		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(
			MovementRotation,
			AimRotation).Yaw;

		bAiming = MyCharacter->GetAiming();
		bFire = MyCharacter->GetFireBullet();
	}
}

void UMyAnimInstance::NativeInitializeAnimation()
{
	MyCharacter = Cast<AMyCharacter>(TryGetPawnOwner());
}
