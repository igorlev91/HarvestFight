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
	WHITE= 7
};

USTRUCT(BlueprintType)
struct FCharacterDetails
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacters Character{ECharacters::COW};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EColours Colour{EColours::RED};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector4 CharacterColour{228.0f/255.0f,154.0f/255.0f,154.0f/255.0f,1.000000};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector4 CharacterSubColour{175.0f/255.0f,90.0f/255.0f,90.0f/255.0f,1.000000};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector4 PureToneColour{1,0,0,1};
};

UCLASS()
class PROTOTYPE2_API UPrototypeGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UPrototypeGameInstance();
	virtual void StartGameInstance() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	int32 MaxPlayersOnServer{0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	int32 FinalConnectionCount{0};

	UPROPERTY(VisibleAnywhere)
	FString UniqueNetIDName{};
	
	UPROPERTY(VisibleAnywhere)
	TMap<FString, FCharacterDetails> FinalPlayerDetails{};
};
