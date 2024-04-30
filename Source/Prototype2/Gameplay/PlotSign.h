
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Prototype2/InteractInterface.h"
#include "PlotSign.generated.h"

USTRUCT(BlueprintType)
struct FPlotSignData
{
	GENERATED_BODY()

		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bHasBeenClaimed{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector4 AssignedColour{};

	UPROPERTY(VisibleAnywhere)
	class APrototype2Character* CharacterWhoClaimed{nullptr};
};

UCLASS()
class PROTOTYPE2_API APlotSign : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	APlotSign();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;
	
	class UStaticMeshComponent* GetMesh();
	void SetMeshGrey();
	
public:	
	virtual void Interact(APrototype2Character* _Player) override;
	virtual void OnDisplayInteractText(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner, int _PlayerID) override;
	virtual EInteractMode IsInteractable(APrototype2PlayerState* _Player, EInteractMode _ForcedMode = INVALID) override;
	virtual void ClientInteract(APrototype2Character* _Player) override;

protected:
	void ClaimPlot(APrototype2Character* _Player);
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UItemComponent* ItemComponent;

	UPROPERTY(EditAnywhere)
	class USquashAndStretch* SSComponent;

	UFUNCTION()
	void OnRep_bClaimed();

	UPROPERTY(ReplicatedUsing=OnRep_bClaimed, EditAnywhere, BlueprintReadOnly)
	FPlotSignData PlotSignData;

	UPROPERTY(EditDefaultsOnly)
	class UMaterialInstance* UnclaimableMaterial{nullptr};
};
