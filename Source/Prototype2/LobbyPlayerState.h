#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "LobbyPlayerState.generated.h"

UCLASS()
class PROTOTYPE2_API ALobbyPlayerState : public APlayerState
{
	GENERATED_BODY()
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	UPROPERTY(Replicated, VisibleAnywhere)
	bool IsReady{};

	UPROPERTY(Replicated, VisibleAnywhere)
	int Player_ID{};
	
	void SetIsReady(bool _isReady);
};
