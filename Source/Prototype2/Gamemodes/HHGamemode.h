
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Prototype2/DataAssets/ColourData.h"
#include "Prototype2/GameInstances/PrototypeGameInstance.h"
#include "HHGamemode.generated.h"

UCLASS()
class PROTOTYPE2_API AHHGamemodeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	AHHGamemodeBase();
	
	void UpdateSessionJoinability(int32 _MaxPlayers = 6);
	
	FCharacterDetails CreateDetailsFromColourEnum(EColours _Colour, bool _RandomCharacter = true);
	
	EColours GetFirstFreeColor(APlayerState* InPlayerState);

	UPROPERTY(EditAnywhere)
	class UColourData* SkinColourData{nullptr};

	UPROPERTY(EditAnywhere)
	class URandomNameData* RandomNameData;

	UPROPERTY(EditAnywhere)
	TArray<FString> AvailableNames{};
};
