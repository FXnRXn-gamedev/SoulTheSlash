// FXnRXn copyright notice

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Characters/Player/SlashPlayerCharacter.h"
#include "Interface/InteractInterface.h"
#include "SlashEquippableItemMaster.generated.h"

class UPhysicsConstraintComponent;
class UPlayerCombatComponent;
class USphereComponent;
class UBillboardComponent;
class UStaticMeshComponent;


UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	Dropped UMETA(DisplayName = "Dropped"),
	Equipped UMETA(DisplayName = "Equipped"),
	Holstered UMETA(DisplayName = "Holstered")
};

UCLASS()
class SOULTHESLASH_API ASlashEquippableItemMaster : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	
	ASlashEquippableItemMaster();
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	void SetWeaponState(EWeaponState WeaponState);
	// Toggles the physics simulation on the item's mesh
	UFUNCTION(BlueprintImplementableEvent)
	void TogglePhysics(bool bEnable);

	// Sets a pivot point, possibly for animation or attachment
	UFUNCTION(BlueprintImplementableEvent)
	void SetPivot(bool bHeld);
	
#pragma region Item Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Item")
	UPlayerCombatComponent* CombatComponentRef;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Item")
	UStaticMesh* EquipmentMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Item")
	EWeaponStateTypeEnum WeaponType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Item|Constraint Settings")
	float Swing = 5.0f;  // Default Swing angle

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Item|Constraint Settings")
	float Twist = 10.0f;  // Default Twist angle

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Item|Constraint Settings")
	bool FlipOnEquipped = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Item|Constraint Settings")
	bool ClosedFist = false;

#pragma endregion

#pragma region Interaction
	UPROPERTY()
	USphereComponent* InnerSphereColl;
	UPROPERTY()
	USphereComponent* OuterSphereColl;

	UPROPERTY()
	UBillboardComponent* InnerInteractionIcon;
	UPROPERTY()
	UBillboardComponent* OuterInteractionIcon;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Interaction")
	float InnerInteractionRadius = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Interaction")
	float OuterInteractionRadius = 270.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Interaction")
	EWeaponTypeEnum Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Interaction")
	FText ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Interaction")
	EWeaponState WeaponState;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Interaction")
	int32 ItemValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Interaction")
	int32 ItemWeight = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Interaction")
	bool bCanBePickedUp = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Interaction")
	bool bIsInInventory = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Interaction")
	bool bIsEquipped = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Interaction")
	EEquipHandEnum EquipHand;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Interaction")
	FName EquipSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|Interaction")
	FName UnequipSocketName;

#pragma endregion

#pragma region components
	UPROPERTY()
	UStaticMeshComponent* ItemMesh;
	
	UPROPERTY()
	UStaticMeshComponent* MainMesh;
#pragma endregion

	virtual void Interact_Implementation(AActor* Actor) override;
	virtual bool CanInteract_Implementation(AActor* Actor) override;

protected:
	void CollectWeapon();

	//Overlap event functions
	UFUNCTION()
	void OnInnerSphereOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                               const FHitResult& SweepResult);

	UFUNCTION()
	void OnInnerSphereOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnOuterSphereOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                               const FHitResult& SweepResult);

	UFUNCTION()
	void OnOuterSphereOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY()
	ASlashPlayerCharacter* SlashCharacter;
};
