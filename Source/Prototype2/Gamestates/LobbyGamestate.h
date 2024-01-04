#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Prototype2/PrototypeGameInstance.h"
#include "LobbyGamestate.generated.h"


class ALobbyCharacter;
UENUM(BlueprintType)
enum class EFarm : uint8 
{
	NONE,
	FARM,
	WINTERFARM
};

UCLASS()
class PROTOTYPE2_API ALobbyGamestate : public AGameStateBase
{
	GENERATED_BODY()

public:
	ALobbyGamestate();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void SetIsReady(int _player, bool _isReady);

	void VoteMap(int _player, EFarm _level);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void UpdateCharacterMaterial(int _player,ECharacters _character, ECharacterColours _characterColour);

	int GetNumberOfCharactersTaken(ECharacters);
	int GetNumberOfCharacterColoursTaken(ECharacters, ECharacterColours);
	
	// Server travelling
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShouldServerTravel{false};

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	int MaxPlayersOnServer{0};

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool IsCountingDown{};

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool bHasCountedDown{};

	// Time between players being ready and map choice showing up
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
	int LobbyLengthMinutes{0};
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
	float LobbyLengthSeconds{3.0f};
	
	// Map choice
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool bShowMapChoice{false};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MapChoice{"Level_Main"};

	// Timer between map choice and starting gameplay
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
	float MapChoiceLengthSeconds{5.0f};
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
	bool bMapChosen{false};

	// Maps
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	int Farm{0};
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	int WinterFarm{0};

	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<class ALobbyPlayerState>> Server_Players;
	
private:

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool PreviousServerTravel{};
};
