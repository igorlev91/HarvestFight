
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Prototype2/InteractInterface.h"
#include "PlotSign.generated.h"

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
	
public:	
	virtual void Interact(APrototype2Character* _Player) override;
	virtual void HoldInteract(APrototype2Character* _Player) override;
	virtual void OnDisplayInteractText(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner, int _PlayerID) override;
	virtual bool IsInteractable(APrototype2PlayerState* _Player) override;
	virtual void ClientInteract(APrototype2Character* _Player) override;

protected:
	void ClaimPlot(APrototype2Character* _Player);
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class UItemComponent* ItemComponent;

	UPROPERTY(EditAnywhere)
	class USquashAndStretch* SSComponent;
	
	UPROPERTY(Replicated, VisibleAnywhere)
	bool bHasBeenClaimed{};
		
	float HoldInteractTotalDuration = 1.0f;
	float HoldInteractTimer = 0.0f;
	APrototype2Character* CurrentPlayerClaimingPlot = nullptr;
};
