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
	COW,
	PIG,
	CHICKEN,
	DUCK
};

USTRUCT()
struct FCharacterDetails
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	ECharacters Character{ECharacters::COW};

	UPROPERTY(VisibleAnywhere)
	FVector4 CharacterColour{0.428690,0.102242,0.102242,1.000000};
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
