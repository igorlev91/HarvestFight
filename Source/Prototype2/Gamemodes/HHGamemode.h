
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Prototype2/DataAssets/ColourData.h"
#include "HHGamemode.generated.h"

UCLASS()
class PROTOTYPE2_API AHHGamemodeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	UFUNCTION()
	void UpdateSessionJoinability(int32 _MaxPlayers = 6);
	
	UFUNCTION()
	FCharacterDetails CreateDetailsFromColourEnum(EColours _Colour);
	UFUNCTION()
	EColours GetFirstFreeColor(APlayerState* InPlayerState);

	UPROPERTY(EditAnywhere)
	class UColourData* SkinColourData{nullptr};
};
