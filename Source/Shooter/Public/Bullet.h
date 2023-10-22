

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class SHOOTER_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullet Properties", meta = (AllowPrivateAccess = "true"));
	class UArrowComponent* Arrow;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullet Properties", meta = (AllowPrivateAccess = "true"));
	class USphereComponent* SphereCollision;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullet Properties", meta = (AllowPrivateAccess = "true"));
	class USpotLightComponent* SpotLight;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullet Properties", meta = (AllowPrivateAccess = "true"));
	class URadialForceComponent* RadialForce;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bullet Properties", meta = (AllowPrivateAccess = "true"));
	class UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, Category = "Movement")
	class UProjectileMovementComponent* ProjecttileMovement;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
