

#pragma once

#include "CoreMinimal.h"
#include "HHGameStateBase.h"
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
	FLOATINGISLANDSFARM,
	CLOCKWORKFARM
};

USTRUCT(BlueprintType)
struct FTeamsDetails
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	EColours TeamOneColour = EColours::BLUE;
	UPROPERTY(VisibleAnywhere)
	EColours TeamTwoColour = EColours::GREEN;
	UPROPERTY(VisibleAnywhere)
	FString TeamOneName = "Team One";
	UPROPERTY(VisibleAnywhere)
	FString TeamTwoName = "Team Two";

	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<class ALobbyPlayerState>> Server_TeamOne{};
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<class ALobbyPlayerState>> Server_TeamTwo{};
};

UCLASS()
class PROTOTYPE2_API ALobbyGamestate : public AHHGameStateBase
{
	GENERATED_BODY()

	/* Public Functions */
public:
	ALobbyGamestate();
	

	int32 GetNumberOfCharactersTaken(ECharacters _DesiredCharacter)  const;
	int32 GetNumberOfCharacterColoursTaken(FCharacterDetails _Details)  const;

	void CheckAllReady();

	void VoteMap(EFarm _Level);

	void SetMaxPlayersOnServer(int32 _maxPlayersOnServer);
	int32 GetMaxPlayersOnServer() const;

	bool ShouldShowMapChoices() const;
	int32 GetFarm() const;
	int32 GetWinterFarm() const;
	int32 GetHoneyFarm() const;
	int32 GetFloatingIslandFarm() const;
	int32 GetClockworkFarm() const;

	bool HasMapBeenChosen() const;
	int32 GetMapChoiceTotalLengthSeconds() const;

	UFUNCTION()
	void OnRep_TeamsDetails();
	
public:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<class ALobbyPlayerState>> Server_Players;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bTeams{};

	UPROPERTY(VisibleAnywhere)
	bool bInitializedOnRepTeams{};

	UPROPERTY(ReplicatedUsing=OnRep_TeamsDetails, VisibleAnywhere)
	FTeamsDetails TeamsDetails{};
	
	UPROPERTY(VisibleAnywhere)
	class UTeamNames* TeamNamesData;
	
	/* Private Functions */
private:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float _DeltaSeconds) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;

	/* Picks a random map to play for the list of those maps most voted for */
	void PickMapToPlay();

	void TickTimers(float _DeltaSeconds);

	void UpdateTeams();


	
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
	int32 ClockworkFarm{0};
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool bPreviousServerTravel{};

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bHasAllPlayersVoted{};


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
