

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Prototype2/GameInstances/PrototypeGameInstance.h"
#include "LobbyGamestate.generated.h"


class ALobbyCharacter;
UENUM(BlueprintType)
enum class EFarm : uint8 
{
	NONE,
	FARM,
	WINTERFARM,
	HONEYFARM,
	FLOATINGISLANDSFARM
};

UCLASS()
class PROTOTYPE2_API ALobbyGamestate : public AGameStateBase
{
	GENERATED_BODY()

	/* Public Functions */
public:
	ALobbyGamestate();
	

	void UpdateCharacterMaterial(int32 _Player,FCharacterDetails _Details);

	int32 GetNumberOfCharactersTaken(ECharacters _DesiredCharacter)  const;
	int32 GetNumberOfCharacterColoursTaken(FCharacterDetails _Details)  const;

	void SetIsReady(int32 _Player, bool _bIsReady);

	void VoteMap(EFarm _Level);
	void UpdatePlayerDetails(int32 _Player, FCharacterDetails _CharacterDetails);

	void SetMaxPlayersOnServer(int32 _maxPlayersOnServer);
	int32 GetMaxPlayersOnServer() const;

	bool ShouldShowMapChoices() const;
	int32 GetFarm() const;
	int32 GetWinterFarm() const;
	int32 GetHoneyFarm() const;
	int32 GetFloatingIslandFarm() const;


	bool HasMapBeenChosen() const;
	int32 GetMapChoiceTotalLengthSeconds() const;

	UFUNCTION(BlueprintCallable)
	void SetGameMode(int32 _Mode);

	int32 GetGameMode();
	
public:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<class ALobbyPlayerState>> Server_Players;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bTeams{};

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	EColours TeamOneColour;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	EColours TeamTwoColour;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<class ALobbyPlayerState>> Server_TeamOne;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<class ALobbyPlayerState>> Server_TeamTwo;


	
	/* Private Functions */
private:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float _DeltaSeconds) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;

	/* Picks a random map to play for the list of those maps most voted for */
	void PickMapToPlay();

	void TickTimers(float _DeltaSeconds);
	/* Private Variables */
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
	const int32 NumberOfMaps = 3; // not currently used

	// Timer between map choice and starting gameplay
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	float MapChoiceTotalLengthSeconds{30.0f};
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
	int32 HoneyFarm{0};
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	int32 FloatingIslandFarm{0};
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool bPreviousServerTravel{};

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	int32 GameMode{};

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bHasAllPlayersVoted{};
};
