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

// This class does not need to be modified.
UINTERFACE()
class UInteractInterface : public UInterface
{
	GENERATED_BODY()
};

class PROTOTYPE2_API IInteractInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface
public:
	virtual void Interact(APrototype2Character* player) = 0;
	virtual void ClientInteract(APrototype2Character* player){};
	virtual void OnDisplayInteractText(class UWidget_PlayerHUD* _invokingWiget, class APrototype2Character* owner, int _playerID) = 0;
	virtual bool IsInteractable(APrototype2PlayerState* player) = 0;
	EInterfaceType InterfaceType = EInterfaceType::Default;

	// For setting HUD text in Prototype2Character::SetHUDText
};
