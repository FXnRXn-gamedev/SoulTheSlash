// FXnRXn copyright notice


#include "Item/SlashEquippableItemMaster.h"
#include "Component/PlayerCombatComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/SphereComponent.h"


// Sets default values
ASlashEquippableItemMaster::ASlashEquippableItemMaster()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SlashCharacter = nullptr;
	// Default values
	WeaponState = EWeaponState::Dropped;
}

void ASlashEquippableItemMaster::BeginPlay()
{
	Super::BeginPlay();

	
	InnerSphereColl = Cast<USphereComponent>(GetDefaultSubobjectByName(TEXT("InnerSphereCollision")));
	OuterSphereColl = Cast<USphereComponent>(GetDefaultSubobjectByName(TEXT("OuterSphereCollision")));
	InnerInteractionIcon = Cast<UBillboardComponent>(GetDefaultSubobjectByName(TEXT("InnerInteractIcon")));
	OuterInteractionIcon = Cast<UBillboardComponent>(GetDefaultSubobjectByName(TEXT("OuterInteractIcon")));
	
	InnerSphereColl->OnComponentBeginOverlap.AddDynamic(this, &ASlashEquippableItemMaster::OnInnerSphereOverlapBegin);
	InnerSphereColl->OnComponentEndOverlap.AddDynamic(this, &ASlashEquippableItemMaster::OnInnerSphereOverlapEnd);
	OuterSphereColl->OnComponentBeginOverlap.AddDynamic(this, &ASlashEquippableItemMaster::OnOuterSphereOverlapBegin);
	OuterSphereColl->OnComponentEndOverlap.AddDynamic(this, &ASlashEquippableItemMaster::OnOuterSphereOverlapEnd);

	//
	ItemMesh = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("Item")));
	MainMesh = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("Main")));
	// RootComponent = Main;
	if (MainMesh && ItemMesh)
	{
		MainMesh->SetVisibility(true);
		ItemMesh->SetVisibility(true);
	}
	if (MainMesh && WeaponState == EWeaponState::Dropped)
	{
		MainMesh->SetSimulatePhysics(true);
		MainMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	}
	
	// Cache reference to controlled character
	SlashCharacter = Cast<ASlashPlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	
}

void ASlashEquippableItemMaster::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	if (InnerSphereColl && OuterSphereColl)
	{
		InnerSphereColl->SetSphereRadius(InnerInteractionRadius);
		OuterSphereColl->SetSphereRadius(OuterInteractionRadius);
	}
}

void ASlashEquippableItemMaster::SetWeaponState(EWeaponState _WeaponState)
{
	WeaponState = _WeaponState;
}


void ASlashEquippableItemMaster::Interact_Implementation(AActor* Actor)
{
	IInteractInterface::Interact_Implementation(Actor);
	
	
	ASlashPlayerCharacter* Character = Cast<ASlashPlayerCharacter>(Actor);
	if (Character && bCanBePickedUp && !bIsEquipped && WeaponState == EWeaponState::Dropped)
	{
		CollectWeapon();
		bIsEquipped = true;
		bCanBePickedUp = false;
		if (InnerInteractionIcon)
		{
			InnerInteractionIcon->SetVisibility(false);
		}
		if (OuterInteractionIcon)
		{
			OuterInteractionIcon->SetVisibility(false);
		}
		if (MainMesh && ItemMesh)
		{
			MainMesh->SetVisibility(false);
			ItemMesh->SetVisibility(false);
		}
	}
}

bool ASlashEquippableItemMaster::CanInteract_Implementation(AActor* Actor)
{
	return WeaponState == EWeaponState::Dropped && bCanBePickedUp;
}

void ASlashEquippableItemMaster::CollectWeapon()
{
	CombatComponentRef = SlashCharacter->FindComponentByClass<UPlayerCombatComponent>();
	if (CombatComponentRef)
	{
		// Disable physics and collision
		MainMesh->SetSimulatePhysics(false);
		MainMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		InnerSphereColl->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		OuterSphereColl->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		
		CombatComponentRef->SetWeapon(this);
	}
	
}



#pragma region Overlap Handlers

void ASlashEquippableItemMaster::OnInnerSphereOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == SlashCharacter && !bIsEquipped && InnerInteractionIcon)
	{
		InnerInteractionIcon->SetVisibility(true);
		OuterInteractionIcon->SetVisibility(false);
		bCanBePickedUp = true;
	}
}

void ASlashEquippableItemMaster::OnInnerSphereOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == SlashCharacter && InnerInteractionIcon)
	{
		InnerInteractionIcon->SetVisibility(false);
		OuterInteractionIcon->SetVisibility(true);
		bCanBePickedUp = false;
	}
}

void ASlashEquippableItemMaster::OnOuterSphereOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == SlashCharacter && !bIsEquipped && OuterInteractionIcon)
	{
		OuterInteractionIcon->SetVisibility(true);
	}
}

void ASlashEquippableItemMaster::OnOuterSphereOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == SlashCharacter && OuterInteractionIcon)
	{
		OuterInteractionIcon->SetVisibility(false);
	}
}

#pragma endregion 






