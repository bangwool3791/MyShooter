

#pragma once

#include "CoreMinimal.h"
#include "item.h"
#include "CDoor.generated.h"

UCLASS()
class SHOOTER_API ACDoor : public Aitem
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACDoor();
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Map, meta = (AllowPrivateAccess = "true"));
	class USceneComponent* SceneComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Map, meta = (AllowPrivateAccess = "true"));
	class UStaticMeshComponent* LeftDoorComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Map, meta = (AllowPrivateAccess = "true"));
	class UStaticMeshComponent* RightDoorComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Map, meta = (AllowPrivateAccess = "true"));
	class UStaticMeshComponent* CenterDoorComponent;

	/* Particles spawnd upon bullet impact*/
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = Map, meta = (AllowPrivateAccess = "true"));
	class UParticleSystem* SmokeParticle;

	TArray<class UMaterial*> LoadMaterial;

	TArray<class UMaterialInstanceDynamic*> DynamicMaterialInst;

	//if 0 Open 1 Close
	int8 nOpenClose;
	//is Open?
	bool bDoorTimerActive;
	//Timer Duration
	float OpenCloseDuration;
	//Timer for Door open,close
	FTimerHandle DoorOpenCloseTimer;

	void DoorOpenCloseFunc();
	void CreateStaticComponent();

	void BindSocketTransform(class UStaticMeshComponent* target, class UStaticMeshComponent* source, const FString& SocketName);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"));
	class USoundCue* DoorCloseSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"));
	class USoundCue* DoorOpenSound;

	bool bSteam;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// When called this function Door Open,close timer Running
	void OpencloseDoor();
	FORCEINLINE class UWidgetComponent* GetPickupWidget() const { return PickupWidget; }

};
