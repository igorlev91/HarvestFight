.
* File Name : Plant.h
* Description : Class for plant objects, used for all blueprint plant objects
.
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
	virtual void OnDisplayInteractText(class UWidget_PlayerHUD* InvokingWidget, class APrototype2Character* Owner, int _PlayerID) override;
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
	int ChanceOfGold = 5; // x/100
	int GoldMultiplier = 3;
	//UPROPERTY(EditAnywhere)
	//float value; // Moved to ItemComponent

	UFUNCTION(Server, Reliable)
	void Server_ToggleGold();
	void Server_ToggleGold_Implementation();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multi_ToggleGold();
	void Multi_ToggleGold_Implementation();
	
};
