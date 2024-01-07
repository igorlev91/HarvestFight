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
#include "PrototypeGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FSearchResults
{
	GENERATED_BODY()
};

UENUM(BlueprintType)
enum class ECharacters : uint8
{
	COW,
	PIG,
	CHICKEN,
	DUCK
};

UENUM(BlueprintType)
enum class ECharacterColours : uint8
{
	RED,
	BLUE,
	GREEN,
	YELLOW
};

UCLASS()
class PROTOTYPE2_API UPrototypeGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UPrototypeGameInstance();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	int32 MaxPlayersOnServer{0};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	int32 FinalConnectionCount{0};

	TArray<FString> FinalPlayerNames;
	TArray<ECharacters> FinalCharacters;
	TArray<ECharacterColours> FinalColours;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	ECharacters Character{ECharacters::COW};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	ECharacterColours CharacterColour{ECharacterColours::RED};
};
