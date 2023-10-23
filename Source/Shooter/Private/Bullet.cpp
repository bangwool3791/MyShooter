


#include "Bullet.h"
#include "Components/ArrowComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components\SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "UObject/ConstructorHelpers.h"
// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Mesh"));
	RadialForce = CreateDefaultSubobject<URadialForceComponent>(TEXT("SpotLight"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RadialForce"));
	ProjecttileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjecttileMovement"));

	SetRootComponent(SphereCollision);
	Arrow->SetupAttachment(GetRootComponent());
	Mesh->SetupAttachment(GetRootComponent());
	SpotLight->SetupAttachment(Mesh);
	RadialForce->SetupAttachment(Mesh);
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	SphereCollision->SetSimulatePhysics(true);
	SphereCollision->SetEnableGravity(false);
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	FVector vDir = GetActorRotation().Vector();
	vDir.Normalize();
	vDir *= 12000.f;
	SphereCollision->AddImpulse(vDir * 1.f);
	
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OnSphereOverlap);
	SphereCollision->OnComponentEndOverlap.AddDynamic(this, &ABullet::OnSphereEndOverlap);
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABullet::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{

}

void ABullet::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
}

void ABullet::FireInDirection(const FVector& ShootDirection)
{
	ProjecttileMovement->Velocity = ShootDirection * ProjecttileMovement->InitialSpeed;
}
