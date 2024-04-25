

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "LobbyGamestate.h"
#include "Prototype2/GameInstances/PrototypeGameInstance.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"
#include "Prototype2Gamestate.generated.h"

UENUM(BlueprintType)
enum EGameSpeed : int
{
	SHORT_GAME,
	MEDIUM_GAME,
	LONG_GAME,
	INDEX
};

USTRUCT(BlueprintType)
struct FHHExtraSettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TEnumAsByte<EGameSpeed> GameSpeed{EGameSpeed::MEDIUM_GAME};
	
	UPROPERTY(BlueprintReadWrite)
	bool bStealing{true};

	UPROPERTY(BlueprintReadWrite)
	bool bGoldFertiliser{true};
	UPROPERTY(BlueprintReadWrite)
	bool bConcreteFertiliser{true};
	UPROPERTY(BlueprintReadWrite)
	bool bPoisonFertiliser{true};
};

UCLASS()
class PROTOTYPE2_API APrototype2Gamestate : public AHHGameStateBase
{
	GENERATED_BODY()

public:
	APrototype2Gamestate();

	int32 GetFinalConnectionCount() const;
	void SetFinalConnectionCount(int32 _FinalConnectionCount);
	void SetMaxPlayersOnServer(int32 _FinalConnectionCount);
	void SetPlayerWinner(int32 _WinnerID);
	void SetWinningScore(int32 _WinningScore);
	int32 RegisterPlayer(class APrototype2PlayerState* _Player);
	void UnRegisterPlayer(APrototype2PlayerState* _Player);
	int32 GetCurrentConnectionCount();
	bool HasGameFinished();
	bool HasGameStarted();
	bool IsGameReadyForVote();
	int32 GetMatchLengthMinutes();
	void SetMatchLengthMinutes(int32 _Minutes);
	int32 GetMatchLengthSeconds();
	int32 GetCountdownLengthMinutes();
	int32 GetCountdownLengthSeconds();
	int32 GetBriefTimesUpLengthSeconds();
	int32 GetPlayerWinner();
	int32 GetWinningScore();
	void UpdatePlayerDetails(int32 _Player, FCharacterDetails _CharacterDetails);
	void PupeteerCharactersForEndGame();
	UFUNCTION(BlueprintCallable)
	void SetGameTime();
	int32 GetSellMultiplier();
	void SetSellMultiplier(int32 _Multiplier);

	void VoteMap(EFarm _Level);

	UFUNCTION()
	void AddCoinsTeams(APrototype2PlayerState* _Player, int32 _Amount);
	
public:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<class APrototype2PlayerState>> Server_Players;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	bool bTeams{};

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	EColours TeamOneColour;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	EColours TeamTwoColour;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	FString TeamOneName;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	FString TeamTwoName;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	FHHExtraSettings ExtraSettings;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWorldOverrideData* DefaultWorldOverrideData;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<class APrototype2PlayerState>> Server_TeamOne;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<class APrototype2PlayerState>> Server_TeamTwo;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	int32 Team1Points{0};
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	int32 Team2Points{0};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UPrototypeGameInstance* GameInstanceRef;
	
	
private:
	virtual void BeginPlay() override;
	virtual void Tick(float _DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;
	
	void TickCountdownTimer(float _DeltaSeconds);
	void TickMatchTimer(float _DeltaSeconds);
	void TickEndGameTimer(float _DeltaSeconds);
	void UpdateTeamsScores();

	void TickMapRotationTimer(float _DeltaSeconds);

	void TickTimers(float _DeltaSeconds);
	
	/* Picks a random map to play for the list of those maps most voted for */
	void PickMapToPlay();

	

public:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool bIsCountingDown{};
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool bPreviousServerTravel{};
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	int32 MatchLengthMinutes{5};
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	float MatchLengthSeconds{0};
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool bIsLastMinute{false};

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	int32 CountdownLengthMinutes{0};
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	float CountdownLengthSeconds{8.0f};

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	int32 EndGameLengthMinutes{0};
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	float EndGameLengthSeconds{1.0f};

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	float BriefTimesUpEndGameLengthSeconds{5.0f};
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool bGameHasStarted{};

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool bHasGameFinished{};

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool bGameReadyForVote{};

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	int32 MaxPlayersOnServer{0};

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	int32 FinalConnectionCount{0};

	UPROPERTY(Replicated,EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	int32 FinalTeamACount{0};

	UPROPERTY(Replicated,EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	int32 FinalTeamBCount{0};

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	int32 PlayerWinner{-1};

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	int32 WinningScore{0};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	class APrototype2GameMode* GameModeRef{};

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	int32 SellMultiplier{1};

	/* Endgame Mapchoice */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	float MapRotationShowTimer{20.0f};

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	bool bIsMapChoiceShowing{false};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bShouldServerTravel{false};

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bHasAllPlayersVoted{};
	
	// Map choice
	//UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	//bool bShowMapChoice{false};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	FString MapChoice{"Level_Main"};

	// Timer between map choice and starting gameplay
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	float MapChoiceTotalLengthSeconds{15.0f};
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
	int32 ClockworkFarm{0};

	/* Maps */

	// Friendly Farm
	FString FriendlyFarmClassicLarge = "/Game/Maps/Level_FF_ClassicLarge";
	FString FriendlyFarmClassicMedium = "/Game/Maps/Level_FF_ClassicMedium";
	FString FriendlyFarmClassicSmall = "/Game/Maps/Level_FF_ClassicSmall";
	FString FriendlyFarmBrawl = "/Game/Maps/Level_FF_Brawl";
	FString FriendlyFarmBlitz = "/Game/Maps/Level_FF_BlitzV3";
	
	// Frosty Fields (Winter)
	FString FrostyFieldsClassicLarge = "/Game/Maps/Level_Winter_ClassicV2Large";
	FString FrostyFieldsClassicMedium = "/Game/Maps/Level_Winter_ClassicV2Medium";
	FString FrostyFieldsClassicSmall = "/Game/Maps/Level_Winter_ClassicV2Small";
	FString FrostyFieldsBrawl = "/Game/Maps/Level_Winter_Brawl";
	FString FrostyFieldsBlitz = "/Game/Maps/Level_Winter_BlitzV2";

	// Honey
	FString HoneyClassicLarge = "/Game/Maps/Level_Honey_ClassicLarge";
	FString HoneyClassicMedium = "/Game/Maps/Level_Honey_ClassicMedium";
	FString HoneyClassicSmall = "/Game/Maps/Level_Honey_ClassicSmall";
	FString HoneyBrawl = "/Game/Maps/Level_Honey_Brawl";
	//FString HoneyBlitz = "/Game/Maps/Level_Honey_Blitz"; // No honey blitz

	// Floating Islands
	FString FloatingIslandsClassic = "/Game/Maps/Upcoming/Level_SkyIslandV2_Flat";
	FString FloatingIslandsBrawl = "/Game/Maps/Upcoming/AttackAltarVariants/Level_SkyIsland_Brawl_Attack";
	FString FloatingIslandsBlitz = "/Game/Maps/Upcoming/AttackAltarVariants/Level_SkyIsland_Blitz_Attack";
	
	// Floating Islands
	FString ClockworkClassic = "/Game/Maps/Upcoming/Level_Clockwork_W_Cogs";
};


