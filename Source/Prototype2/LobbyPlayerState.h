#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "LobbyPlayerState.generated.h"

UCLASS()
class PROTOTYPE2_API ALobbyPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	bool IsReady{};
};
