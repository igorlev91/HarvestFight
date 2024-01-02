#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGamemode.generated.h"


UCLASS()
class PROTOTYPE2_API ALobbyGamemode : public AGameModeBase
{
	GENERATED_BODY()

	ALobbyGamemode();
	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Tick(float DeltaSeconds) override;
	
public:
	
	UPROPERTY(EditAnywhere)
	TArray<UMaterialInstance*> PlayerMaterials{{},{},{},{}};
};
