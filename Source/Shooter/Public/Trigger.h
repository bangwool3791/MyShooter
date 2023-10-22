

#pragma once

#include "CoreMinimal.h"
#include "item.h"
#include "Trigger.generated.h"

UCLASS()
class SHOOTER_API ATrigger : public Aitem
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
public:
	ATrigger();

private:

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

	class ACDoor* DoorActor;
public:
	FORCEINLINE class ACDoor* GetDoorActor() { return DoorActor; }
FORCEINLINE class UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
};
