

#pragma once

#include "CoreMinimal.h"
#include "PickUpItem.h"
#include "Fertiliser.generated.h"

UCLASS()
class PROTOTYPE2_API AFertiliser : public APickUpItem, public IInteractInterface
{
	GENERATED_BODY()

public:
	AFertiliser();

protected:
	virtual void BeginPlay() override;
	virtual void Interact(APrototype2Character* _Player) override;
	virtual void HoldInteract(APrototype2Character* _Player) override;
	virtual void ClientInteract(APrototype2Character* _Player) override;
	virtual void OnDisplayInteractText(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner, int _PlayerID) override;
	virtual bool IsInteractable(APrototype2PlayerState* _Player) override;
};
