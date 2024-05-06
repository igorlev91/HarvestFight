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
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	bool IsReady{};

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	int32 Player_ID{};

	UPROPERTY(Replicated, VisibleAnywhere)
	FString PlayerName{};

	UFUNCTION()
	void SetIsReady(bool _bIsReady);
	UFUNCTION(Server, Reliable)
	void Server_SetIsReady(bool _bIsReady);

	UFUNCTION()
	void VoteMap(EFarm _Map);
	UFUNCTION(Server, Reliable)
	void Server_VoteMap(EFarm _Map);
	
	UFUNCTION()
	void UpdateCharacterMaterial(FCharacterDetails _Details);
	UFUNCTION(Server, Reliable)
	void Server_UpdateCharacterMaterial(FCharacterDetails _Details);

	UFUNCTION()
	void OnRep_CharacterDetails();
	
	UPROPERTY(ReplicatedUsing=OnRep_CharacterDetails, EditAnywhere, BlueprintReadWrite)
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
	
	UPROPERTY(EditAnywhere)
	TArray<UTexture2D*> BeeTextures{{}, {}, {}, {}, {}, {}, {}, {}};
};
