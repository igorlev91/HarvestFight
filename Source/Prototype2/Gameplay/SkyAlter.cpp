


#include "Prototype2/Gameplay/SkyAlter.h"

#include "Smite.h"
#include "Kismet/GameplayStatics.h"
#include "Prototype2/Characters/Prototype2Character.h"
#include "Prototype2/Gamestates/Prototype2Gamestate.h"
#include "Prototype2/Pickups/Plant.h"
#include "SymsLib/syms/core/generated/syms_meta_serial_ext.h"

// Sets default values
ASkyAlter::ASkyAlter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemComponent = CreateDefaultSubobject<UItemComponent>(TEXT("ItemComponent"));
	ItemComponent->Mesh->SetIsReplicated(false);

}

// Called when the game starts or when spawned
void ASkyAlter::BeginPlay()
{
	Super::BeginPlay();
	ItemComponent->Mesh->OnComponentHit.AddDynamic(this, &ASkyAlter::OnPlayerTouchAltar);
	BoxComponent1->OnComponentHit.AddDynamic(this, &ASkyAlter::OnPlayerTouchAltar);
	BoxComponent2->OnComponentHit.AddDynamic(this, &ASkyAlter::OnPlayerTouchAltar);
	BoxComponent3->OnComponentHit.AddDynamic(this, &ASkyAlter::OnPlayerTouchAltar);
}

void ASkyAlter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& _OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(_OutLifetimeProps);
}

// Called every frame
void ASkyAlter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ASkyAlter::IsInteractable(APrototype2PlayerState* _Player)
{
	return false;
}

bool ASkyAlter::IsInteractable_Unprotected(APrototype2PlayerState* _Player, bool _LookOutForConcrete)
{
	return false;
}

void ASkyAlter::ClientInteract(APrototype2Character* _Player)
{
	IInteractInterface::ClientInteract(_Player);
}

void ASkyAlter::Interact(APrototype2Character* _Player)
{
}

void ASkyAlter::HoldInteract(APrototype2Character* _Player)
{
}

void ASkyAlter::OnDisplayInteractText(UWidget_PlayerHUD* _InvokingWidget, APrototype2Character* _Owner, int32 _PlayerID)
{
}

void ASkyAlter::Client_ClearItem_Implementation(APrototype2Character* _Player)
{
	if (_Player->PlayerHUDRef)
		_Player->PlayerHUDRef->ClearPickupUI();
}

void ASkyAlter::OnPlayerTouchAltar(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor->HasNetOwner())
		return;
	
	if (!OtherActor)
		return;
	APrototype2Character* SomePlayer = Cast<APrototype2Character>(OtherActor);
	if (!SomePlayer)
		return;
	APrototype2Gamestate* GameStateCast = Cast<APrototype2Gamestate>(UGameplayStatics::GetGameState(GetWorld()));
	if (!GameStateCast)
		return;
	
	UE_LOG(LogTemp, Warning, TEXT("Attempted to offer something!"));
	if (!SomePlayer->HeldItem)
		return;

	if (auto Plant = Cast<APlant>(SomePlayer->HeldItem))
	{
		if (Plant->ServerData.SeedData->Name != "Lamb")
		{
			SomePlayer->smite->IncreaseTime(Plant->ServerData.SeedData->BabyStarValue * 10);
			SomePlayer->PlayerStateRef->AddCoins(SomePlayer->HeldItem->ServerData.SeedData->BabyStarValue);
		}
		else
		{
			SomePlayer->smite->SetSmiteTime(SomePlayer->smite->TimerStartTime);
			SomePlayer->PlayerStateRef->AddCoins(SomePlayer->HeldItem->ServerData.SeedData->BabyStarValue);
		}

		Client_ClearItem(SomePlayer);
			
		// Audio
		if (!HasAuthority())
			return;

		if (SomePlayer->SellCue)
		{
			SomePlayer->PlaySoundAtLocation(GetActorLocation(), SomePlayer->SellCue, nullptr);
		}
			
		SomePlayer->Multi_SocketItem(SomePlayer->WeaponMesh, FName("Base-HumanWeapon"));

		APickUpItem* item = SomePlayer->HeldItem;
		SomePlayer->DropItem();
		// Destroy the crop the player is holding
		item->Destroy();

		SomePlayer->Client_RefreshCurrentMaxSpeed();
	}
}

