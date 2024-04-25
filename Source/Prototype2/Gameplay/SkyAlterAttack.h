

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"

#include "GameFramework/Actor.h"
#include "SkyAlterAttack.generated.h"

UCLASS()
class PROTOTYPE2_API ASkyAlterAttack : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASkyAlterAttack();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnPlayerTouchAltar(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
	void Attack(class APrototype2Character* _PlayerToNotSmite, int32 _StarValueOfPlant);

	UFUNCTION(BlueprintImplementableEvent)
	void OnSacrificed();

	UFUNCTION(Server, Reliable)
	void Server_SacrificePoof();

	UFUNCTION(NetMulticast, Reliable)
	void Multi_SacrificePoof();
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class TSubclassOf<AActor> PoofSystem;
	
public:	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMesh;
	
	UPROPERTY(EditDefaultsOnly)
	class UWeaponData* SmiteWeaponData;

	/* When smite hits player the knockback will be multiplied by this value so bigger map can have more knockback */
	UPROPERTY(EditAnywhere)
	float KnockBackMultiplier = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* BoxComponent1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* BoxComponent2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* BoxComponent3;

	UPROPERTY(EditAnywhere, Category="Debug")
	bool DEBUG_AttackTeamMates = false;
};

