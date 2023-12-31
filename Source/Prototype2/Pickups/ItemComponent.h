﻿#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Prototype2/Widgets/Widget_PlayerHUD.h"
#include "ItemComponent.generated.h"

class APrototype2Character;
class APickUpItem;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROTOTYPE2_API UItemComponent : public UActorComponent
{
	GENERATED_BODY()
	
	UItemComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float _DeltaTime, ELevelTick _TickType, FActorComponentTickFunction* _ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const override;
	
public:
	void Interact(APrototype2Character* _Player, APickUpItem* _ItemPickedUp);
	
	/* Spawning from PlantData Data Asset */
	void InitializeSeed(UMaterialInstance* _InMaterial);
	void InitializePlant(UStaticMesh* _InMesh);
	void InitializeWeapon(UStaticMesh* _InMesh);
	
public:
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh;
	
	UPROPERTY(Replicated , EditAnywhere)
	bool bGold{};
};
