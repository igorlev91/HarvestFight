


#include "Prototype2/Gameplay/SkyAlter.h"

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

