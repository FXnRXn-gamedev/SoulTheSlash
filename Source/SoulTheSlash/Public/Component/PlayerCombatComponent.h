// FXnRXn copyright notice

#pragma once

#include "CoreMinimal.h"
#include "Characters/Player/SlashPlayerCharacter.h"
#include "Components/ActorComponent.h"
#include "Data/Enums/SoulPlayerEnum.h"
#include "PlayerCombatComponent.generated.h"

class ASlashEquippableItemMaster;


USTRUCT(BlueprintType)
struct FWeaponEquippableStruct
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponTypeEnum WeaponType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ASlashEquippableItemMaster> ItemActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESocketEnum AttachSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEquipHandEnum EquipHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* ActorRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Equipped;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULTHESLASH_API UPlayerCombatComponent : public UActorComponent
{
	GENERATED_BODY()

	//------------------------------------------------------------------------------------------------------------------
public:	
	UPlayerCombatComponent();

#pragma region Public Equip/UnEquip Functions
	
	//--> FXnRXn: These functions can be called from outside the component
	
	void SetWeapon(ASlashEquippableItemMaster* NewWeapon);
	void DropWeapon(ASlashEquippableItemMaster* WeaponToDrop);
	bool HasWeaponOfType(EWeaponTypeEnum WeaponType) const;

	
	void AttachEquippableWeapon(EWeaponTypeEnum WeaponType);
	void AttachItemSocket(FWeaponEquippableStruct EquippableStruct, AActor* Actor);

	void EquipWeapon(EWeaponTypeEnum ItemType);
	void UnequipWeapon(EWeaponTypeEnum ItemType);
	void SetEquipStatus(EWeaponTypeEnum ItemType, bool bEquipped);

#pragma region Equippable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|EquippableData")
	TMap<EWeaponTypeEnum, FWeaponEquippableStruct> WeaponEquippableSetup;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|EquippableData|Equippable")
	ASlashEquippableItemMaster* MainWeaponItem;

#pragma endregion

#pragma region Animation

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "---Slash---|EquippableData|Animation")
	USlashCharacteAnimInstanceBase* PlayerAnimInst;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|EquippableData|Animation|Equip")
	TObjectPtr<UAnimMontage> EquipSwordAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|EquippableData|Animation|Equip")
	TObjectPtr<UAnimMontage> EquipBowAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|EquippableData|Animation|Equip")
	TObjectPtr<UAnimMontage> EquipCrossbowAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|EquippableData|Animation|Equip")
	TObjectPtr<UAnimMontage> EquipKnifeAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|EquippableData|Animation|Unequip")
	TObjectPtr<UAnimMontage> UnequipSwordAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|EquippableData|Animation|Unequip")
	TObjectPtr<UAnimMontage> UnequipBowAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|EquippableData|Animation|Unequip")
	TObjectPtr<UAnimMontage> UnequipCrossbowAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "---Slash---|EquippableData|Animation|Unequip")
	TObjectPtr<UAnimMontage> UnequipKnifeAnimMontage;
	
#pragma endregion


#pragma endregion


	//------------------------------------------------------------------------------------------------------------------
protected:
	virtual void BeginPlay() override;
	
#pragma region Protected Equip/UnEquip Functions
	
	UAnimMontage* GetEquipMontage(FWeaponEquippableStruct EquippableStruct);
	UAnimMontage* GetUnequipMontage(FWeaponEquippableStruct EquippableStruct);

#pragma endregion
	
	// Reference to Owner Character
	UPROPERTY()
	ASlashPlayerCharacter* OwnerCharacter;

		
};


