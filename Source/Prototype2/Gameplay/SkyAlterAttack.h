
#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"

#include "GameFramework/Actor.h"
#include "SkyAlterAttack.generated.h"

USTRUCT()
struct FServerSkyAlterData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	float TimeOfSacrifice{};

	UPROPERTY(VisibleAnywhere)
	class APrototype2PlayerState* LastPlayerToSacrifice{nullptr};
};

UCLASS()
class PROTOTYPE2_API ASkyAlterAttack : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASkyAlterAttack();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnPlayerTouchAltar(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);
	void Attack(class APrototype2Character* _PlayerToNotSmite, int32 _StarValueOfPlant);

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess))
	class TSubclassOf<AActor> PoofSystem;

	UFUNCTION()
	void OnRep_OnSacrifice();
	
	UPROPERTY(ReplicatedUsing=OnRep_OnSacrifice, VisibleAnywhere)
	FServerSkyAlterData ServerSkyAlterData{};
	
public:	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMesh;
	
	UPROPERTY(EditDefaultsOnly)
	class UWeaponData* SmiteWeaponData;

	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* AlterOffer_VFX{nullptr};
	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* AlterOffer_VFX2{nullptr};
	UPROPERTY(VisibleAnywhere)
	class UNiagaraComponent* AlterOffer_VFX_Static{nullptr};

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* AlterOffer_System{nullptr};
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* AlterOffer_System2{nullptr};
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* AlterOffer_System_StaticRay{nullptr};
	
	/* When smite hits player the knockback will be multiplied by this value so bigger map can have more knockback */
	UPROPERTY(EditAnywhere)
	float KnockBackMultiplier = 1.0f;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* AlterCollisionBox{nullptr};

	UPROPERTY(EditAnywhere, Category="Debug")
	bool DEBUG_AttackTeamMates = false;
	
	UPROPERTY(EditAnywhere, Category= SFX)
	class USoundCue* SacrificeCue{nullptr};
	UPROPERTY(EditAnywhere, Category= SFX)
	class USoundCue* EnemySacrificeCue{nullptr};
	UPROPERTY(EditAnywhere, Category= SFX)
	class USoundAttenuation* SacrificeAttenuation{nullptr};
};

