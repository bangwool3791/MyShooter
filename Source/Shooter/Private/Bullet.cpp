


#include "Bullet.h"
#include "Components/ArrowComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components\SphereComponent.h"

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
	vDir *= 10000.f;
	SphereCollision->AddImpulse(vDir * 1.f);

	//Arrow->SetSimulatePhysics(true);
	//Arrow->SetEnableGravity(false);
	//Arrow->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//Arrow->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	//Arrow->AddImpulse(vDir * 1.f);

	Mesh->SetSimulatePhysics(true);
	Mesh->SetEnableGravity(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Mesh->AddImpulse(vDir * 1.f);
	//Super::BeginPlay();
	//FVector FRot{ 0.f, 0.f, 359.999997 };
	//Arrow->SetRelativeRotation(FRot.ToOrientationQuat());
	//Arrow->SetArrowColor(FLinearColor::Red);
	//Arrow->SetArrowSize(1.f);
	//Arrow->ArrowLength = 80.f;
	//Arrow->ScreenSize = 0.0025f;
	//Arrow->bUseInEditorScaling = true;
	//Arrow->SetComponentTickEnabled(true);

}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

