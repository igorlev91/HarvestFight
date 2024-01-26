

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Prototype2/GameInstances/PrototypeGameInstance.h"
#include "Prototype2/PlayerStates/Prototype2PlayerState.h"
#include "Prototype2Gamestate.generated.h"

UCLASS()
class PROTOTYPE2_API APrototype2Gamestate : public AGameStateBase
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
	void SetGameTime();
	int32 GetSellMultiplier();
	void SetSellMultiplier(int32 _Multiplier);
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
	TArray<TObjectPtr<class APrototype2PlayerState>> Server_TeamOne;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<class APrototype2PlayerState>> Server_TeamTwo;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	int32 Team1Points{0};
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	int32 Team2Points{0};
	
private:
	virtual void BeginPlay() override;
	virtual void Tick(float _DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;
	
	void TickCountdownTimer(float _DeltaSeconds);
	void TickMatchTimer(float _DeltaSeconds);
	void TickEndGameTimer(float _DeltaSeconds);
	void UpdateTeamsScores();

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

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	int32 PlayerWinner{-1};

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	int32 WinningScore{0};
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	class APrototype2GameMode* GameModeRef{};

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	int32 SellMultiplier{1};
};


