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

	void UpdateCharacterMaterial(int32 _Player,ECharacters _Character, ECharacterColours _CharacterColour);

	int32 GetNumberOfCharactersTaken(ECharacters _DesiredCharacter)  const;
	int32 GetNumberOfCharacterColoursTaken(ECharacters _DesiredCharacter, ECharacterColours _DesiredCharacterColour)  const;

	void SetIsReady(int32 _Player, bool _bIsReady);

	void VoteMap(EFarm _Level);

	void SetMaxPlayersOnServer(int32 _maxPlayersOnServer);
	int32 GetMaxPlayersOnServer() const;

	bool ShouldShowMapChoices() const;
	int32 GetFarm() const;
	int32 GetWinterFarm() const;

	bool HasMapBeenChosen() const;
	int32 GetMapChoiceLengthSeconds() const;
	
public:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<class ALobbyPlayerState>> Server_Players;

private:
	virtual void BeginPlay() override;
	virtual void Tick(float _DeltaSeconds) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bShouldServerTravel{false};

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	int32 MaxPlayersOnServer{0};

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool bIsCountingDown{};

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool bHasCountedDown{};

	// Time between players being ready and map choice showing up
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	int32 LobbyLengthMinutes{0};
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	float LobbyLengthSeconds{3.0f};
	
	// Map choice
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool bShowMapChoice{false};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	FString MapChoice{"Level_Main"};

	// Timer between map choice and starting gameplay
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	float MapChoiceLengthSeconds{5.0f};
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool bMapChosen{false};

	// Maps
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	int32 Farm{0};
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	int32 WinterFarm{0};
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool bPreviousServerTravel{};
};
