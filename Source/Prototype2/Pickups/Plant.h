#pragma once

#include "CoreMinimal.h"
#include "PickUpItem.h"
#include "Plant.generated.h"

UCLASS()
class PROTOTYPE2_API APlant : public APickUpItem, public IInteractInterface
{
	GENERATED_BODY()
public:
	APlant();
	virtual void BeginPlay() override;
	virtual void Interact(APrototype2Character* _Player) override;
	virtual void ClientInteract(APrototype2Character* _Player) override;
	virtual void OnDisplayInteractText(class UWidget_PlayerHUD* _InvokingWidget, class APrototype2Character* _Owner, int _PlayerID) override;
	virtual bool IsInteractable(APrototype2PlayerState* _Player) override;

	/* makes the plant gold */
	void MakeGold();

	bool bGrown = false;
	UPROPERTY(EditAnywhere)
	// ReSharper disable once CppUE4ProbableMemoryIssuesWithUObject
	UMaterialInterface* GoldMaterial;
	UPROPERTY(EditAnywhere)
	// ReSharper disable once CppUE4ProbableMemoryIssuesWithUObject
	UMaterialInterface* GoldMaterial2;

	UPROPERTY(EditAnywhere)
	// ReSharper disable once CppUE4ProbableMemoryIssuesWithUObject
	UMaterialInterface* GoldMaterial3;
	int32 ChanceOfGold = 5; // x/100
	int32 GoldMultiplier = 3;
	//UPROPERTY(EditAnywhere)
	//float value; // Moved to ItemComponent

	UFUNCTION(Server, Reliable)
	void Server_ToggleGold();
	void Server_ToggleGold_Implementation();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_ToggleGold();
	void Multi_ToggleGold_Implementation();
	
};
