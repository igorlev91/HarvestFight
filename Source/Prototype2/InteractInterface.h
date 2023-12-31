#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractInterface.generated.h"

class APrototype2Character;
class APlant;

UENUM()
enum class EInterfaceType : uint8
{
	SellBin     UMETA(DisplayName = "SellShop"),
	Default      UMETA(DisplayName = "Default"),
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

	EInterfaceType InterfaceType = EInterfaceType::Default;
};
