#pragma once

#include "CoreMinimal.h"
#include "PickUpItem.h"
#include "ItemComponent.h"
#include "Weapon.generated.h"

class APickUpItem;
UCLASS()
class PROTOTYPE2_API UWeapon : public UActorComponent, public IInteractInterface
{
public:
	GENERATED_BODY()
	UWeapon();

	virtual void Interact(APrototype2Character* _Player) override;
	virtual void OnDisplayInteractText(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner, int _PlayerID) override;
	virtual bool IsInteractable(APrototype2PlayerState* _Player) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UStaticMeshComponent* Mesh;
};
