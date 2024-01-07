#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractInterface.generated.h"

class APrototype2PlayerState;
class APrototype2Character;
class APlant;

UENUM()
enum class EInterfaceType : uint8
{
	SellBin			UMETA(DisplayName = "SellShop"),
	GrowSpot		UMETA(DisplayName = "GrowSpot"),	
	Weapon			UMETA(DisplayName = "Weapon"),
	Default			UMETA(DisplayName = "Default"),
};

UENUM()
enum class EGrowSpotState : uint8
{
	Empty			UMETA(DisplayName = "Empty"),
	Growing			UMETA(DisplayName = "Growing"),	
	Grown			UMETA(DisplayName = "Grown"),
	Default			UMETA(DisplayName = "Default"),
};


UINTERFACE()
class UInteractInterface : public UInterface
{
	// This class does not need to be modified.
	GENERATED_BODY()
	// This class does not need to be modified.
};

class PROTOTYPE2_API IInteractInterface
{
	GENERATED_BODY()
public:
	virtual void Interact(APrototype2Character* _Player) = 0;
	virtual void ClientInteract(APrototype2Character* _Player){};
	virtual void OnDisplayInteractText(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner, int _PlayerID) = 0;
	virtual bool IsInteractable(APrototype2PlayerState* _Player) = 0;
	EInterfaceType InterfaceType = EInterfaceType::Default;
};
