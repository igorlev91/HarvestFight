#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Prototype2Gamestate.generated.h"

UCLASS()
class PROTOTYPE2_API APrototype2Gamestate : public AGameStateBase
{
	GENERATED_BODY()

public:
	APrototype2Gamestate();
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShouldServerTravel{true};
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
	int MatchLengthMinutes{5};
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
	float MatchLengthSeconds{0};

private:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool IsCountingDown{};
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool PreviousServerTravel{};
};
