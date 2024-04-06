
#pragma once

#include "CoreMinimal.h"
#include "HHGamemode.h"
#include "GameFramework/GameModeBase.h"
#include "Prototype2/InteractInterface.h"
#include "Prototype2/GameInstances/PrototypeGameInstance.h"
#include "LobbyGamemode.generated.h"

UCLASS()
class PROTOTYPE2_API ALobbyGamemode : public AHHGamemodeBase
{
	GENERATED_BODY()
	/* Public Functions */
public:
	/* Constructor */
	ALobbyGamemode();

	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere)
	TArray<UMaterialInstance*> PlayerMaterials{{},{},{},{}};

	UPROPERTY(EditAnywhere)
	TArray<USkeletalMesh*> PlayerMeshes;

	//
	UPROPERTY(EditAnywhere)
	class UColourData* SkinColourData{nullptr};

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bTeams{};
private:
	/* Called after logging in */
	virtual void PostLogin(APlayerController* _NewPlayer) override;

	/* Called when logging out */
	virtual void Logout(AController* _Exiting) override;

	/* Your classic tick, tried and true */
	virtual void Tick(float _DeltaSeconds) override;

	void UpdateAllPlayerInfo(class ALobbyGamestate* _GameStateReference, class UPrototypeGameInstance* _gameInstanceReference);

	FCharacterDetails CreateDetailsFromColourEnum(EColours _Colour);
private:
	/* For holding all the costumes */

	///* Player 1 character lobby starting positions  - used for V2 lobby */
	//UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	//FVector Player1StartPosition = FVector{2000.0f,40.0f, 90.0f};

	/* Player distances in lobby */
	//UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	//float DistanceBetweenPlayers = 230.0f;
};
