

#pragma once

#include "CoreMinimal.h"
#include "Prototype2/GameInstances/PrototypeGameInstance.h"
#include "GameFramework/Character.h"
#include "LobbyCharacter.generated.h"

class UWidgetComponent;
UCLASS()
class PROTOTYPE2_API ALobbyCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere)
	class ALobbyHUD* HUD;
	
protected:
	ALobbyCharacter();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void SyncCharacterSkin();

	virtual void OnRep_PlayerState() override;
};
