#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Engine/GameInstance.h"
#include "Engine.h"
#include "Online.h"
#include "Online/CoreOnline.h"
#include "Prototype2/DataAssets/AnimationData.h"
#include "PrototypeGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FSearchResults
{
	GENERATED_BODY()
};

UENUM(BlueprintType)
enum class ECharacters : uint8
{
	COW = 0,
	PIG = 1,
	CHICKEN = 2,
	DUCK = 3
};

UENUM(BlueprintType)
enum class EColours : uint8
{
	RED = 0,
	BLUE= 1,
	GREEN= 2,
	YELLOW= 3,
	PURPLE= 4,
	ORANGE= 5,
	BLACK= 6,
	WHITE= 7,
	MAXCOLOURS=8
};

USTRUCT(BlueprintType)
struct FCharacterDetails
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacters Character{ECharacters::COW};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimationData* AnimationData = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EColours Colour{EColours::RED};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector4 CharacterColour{228.0f/255.0f,154.0f/255.0f,154.0f/255.0f,1.000000};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector4 CharacterSubColour{175.0f/255.0f,90.0f/255.0f,90.0f/255.0f,1.000000};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector4 PureToneColour{0.571125,0.031896,0.016807,1};
};

UCLASS()
class PROTOTYPE2_API UPrototypeGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UPrototypeGameInstance();
	virtual void OnStart() override;
	virtual void StartGameInstance() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bPlayerStentil{ false };
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	int32 MaxPlayersOnServer{0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	int32 FinalConnectionCount{0};

	UPROPERTY(VisibleAnywhere)
	FString UniqueNetIDName{};
	
	UPROPERTY(VisibleAnywhere)
	TMap<FString, FCharacterDetails> FinalPlayerDetails{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	bool bTeams{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	int32 FinalTeamACount{0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	int32 FinalTeamBCount{0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	EColours FinalTeamAColour{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	EColours FinalTeamBColour{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MusicVolume{ 0.5f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AmbienceVolume{ 0.5f };

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SFXVolume{ 0.5f };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString TeamOneName;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FString TeamTwoName;

public:
	// Game
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	TArray<UMaterialInstance*> PlayerMaterials{{},{},{},{}};
	UPROPERTY(VisibleAnywhere, meta = (AllowPrivateAccess))
	TArray<UMaterialInstanceDynamic*> PlayerMaterialsDynamic{};
	UPROPERTY(EditDefaultsOnly)
	TArray<class UAnimationData*> PlayerModels{{},{},{},{}};
	//

	// Lobby
	UPROPERTY(EditDefaultsOnly)
	UClass* LobbyAnimBP;
	UPROPERTY(EditDefaultsOnly)
	TArray<class USkeletalMesh*> LobbyPlayerModels{{},{},{},{}};
	//
};
