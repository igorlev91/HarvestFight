#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Prototype2/Gamestates/LobbyGamestate.h"
#include "Prototype2/GameInstances/PrototypeGameInstance.h"
#include "LobbyPlayerState.generated.h"

UCLASS()
class PROTOTYPE2_API ALobbyPlayerState : public APlayerState
{
	GENERATED_BODY()

	ALobbyPlayerState();
	virtual void BeginPlay() override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;

	virtual void Tick(float _DeltaSeconds) override;
	
public:
	UPROPERTY(Replicated, VisibleAnywhere)
	bool IsReady{};

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	int32 Player_ID{};

	UPROPERTY(Replicated, VisibleAnywhere)
	FString PlayerName{};
	
	void SetIsReady(bool _bIsReady);

	void UpdateCharacterMaterial(FCharacterDetails _Details);

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	FCharacterDetails Details;

	/* Player UI Textures */
public:
	UPROPERTY(EditAnywhere)
	TArray<UTexture2D*> CowTextures{{}, {}, {}, {}, {}, {}, {}, {}};

	UPROPERTY(EditAnywhere)
	TArray<UTexture2D*> PigTextures{{}, {}, {}, {}, {}, {}, {}, {}};

	UPROPERTY(EditAnywhere)
	TArray<UTexture2D*> ChickenTextures{{}, {}, {}, {}, {}, {}, {}, {}};

	UPROPERTY(EditAnywhere)
	TArray<UTexture2D*> DuckTextures{{}, {}, {}, {}, {}, {}, {}, {}};
	
};
