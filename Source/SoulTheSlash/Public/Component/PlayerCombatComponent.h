// FXnRXn copyright notice

#pragma once

#include "Characters/Player/SlashPlayerCharacter.h"
#include "Components/ActorComponent.h"
#include "CoreMinimal.h"
#include "Data/Enums/SoulPlayerEnum.h"
#include "PlayerCombatComponent.generated.h"


class ASlashEquippableItemMaster;

// USTRUCT(BlueprintType)
// struct FWeaponEquippableStruct
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	EWeaponTypeEnum WeaponType;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	TSubclassOf<ASlashEquippableItemMaster> ItemActor;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	ESocketEnum AttachSocket;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	EEquipHandEnum EquipHand;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	AActor* ActorRef;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite)
// 	bool Equipped;
// };

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SOULTHESLASH_API UPlayerCombatComponent : public UActorComponent {
  GENERATED_BODY()

  //------------------------------------------------------------------------------------------------------------------
public:
  UPlayerCombatComponent();

  // --============================== Public Equip/UnEquip Functions
  // ==============================--

  //--> FXnRXn: These functions can be called from outside the component

  void SetWeapon(ASlashEquippableItemMaster *NewWeapon);
  void DropWeapon(ASlashEquippableItemMaster *WeaponToDrop);
  bool HasWeaponOfType(EWeaponTypeEnum WeaponType) const;

  void AttachEquippableWeapon(EWeaponTypeEnum WeaponType);
  void AttachItemSocket(FEquippableStruct EquippableStruct, AActor *Actor);

  void EquipWeapon(FEquippableStruct *Item);
  void UnequipWeapon(EWeaponTypeEnum ItemType);
  void SetEquipStatus(FEquippableStruct *Item, bool bEquipped);

#pragma region Equippable
  UPROPERTY(EditAnywhere, Category = "---Slash---|EquippableData")
  TMap<EWeaponTypeEnum, FEquippableStruct> WeaponEquippableSetup;

  UPROPERTY(EditAnywhere, Category = "---Slash---|EquippableData|Equippable")
  ASlashEquippableItemMaster *MainWeaponItem;

#pragma endregion

#pragma region Animation

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
            Category = "---Slash---|EquippableData|Animation")
  USlashCharacteAnimInstanceBase *PlayerAnimInst;

  UPROPERTY(EditAnywhere, BlueprintReadWrite,
            Category = "---Slash---|EquippableData|Animation|Equip")
  TObjectPtr<UAnimMontage> EquipSwordAnimMontage;

  UPROPERTY(EditAnywhere, BlueprintReadWrite,
            Category = "---Slash---|EquippableData|Animation|Equip")
  TObjectPtr<UAnimMontage> EquipBowAnimMontage;

  UPROPERTY(EditAnywhere, BlueprintReadWrite,
            Category = "---Slash---|EquippableData|Animation|Equip")
  TObjectPtr<UAnimMontage> EquipCrossbowAnimMontage;

  UPROPERTY(EditAnywhere, BlueprintReadWrite,
            Category = "---Slash---|EquippableData|Animation|Equip")
  TObjectPtr<UAnimMontage> EquipKnifeAnimMontage;

  UPROPERTY(EditAnywhere, BlueprintReadWrite,
            Category = "---Slash---|EquippableData|Animation|Unequip")
  TObjectPtr<UAnimMontage> UnequipSwordAnimMontage;

  UPROPERTY(EditAnywhere, BlueprintReadWrite,
            Category = "---Slash---|EquippableData|Animation|Unequip")
  TObjectPtr<UAnimMontage> UnequipBowAnimMontage;

  UPROPERTY(EditAnywhere, BlueprintReadWrite,
            Category = "---Slash---|EquippableData|Animation|Unequip")
  TObjectPtr<UAnimMontage> UnequipCrossbowAnimMontage;

  UPROPERTY(EditAnywhere, BlueprintReadWrite,
            Category = "---Slash---|EquippableData|Animation|Unequip")
  TObjectPtr<UAnimMontage> UnequipKnifeAnimMontage;

#pragma endregion

#pragma endregion

  UFUNCTION()
  bool IsWeaponSlotEmpty() const { return CurrentAttachedWeapon == nullptr; }

  UPROPERTY(VisibleAnywhere, Category = "---Slash---|CombatWeapon")
  ASlashEquippableItemMaster *CurrentAttachedWeapon;

  UPROPERTY(VisibleAnywhere, Category = "---Slash---|CombatWeapon")
  TArray<FWeaponInstanceData> CurrentAttachedWeaponData;

  UPROPERTY(EditAnywhere, Category = "---Slash---|CombatWeapon")
  EWeaponSwapBehavior WeaponSwapBehavior = EWeaponSwapBehavior::WSB_DropToWorld;

  //------------------------------------------------------------------------------------------------------------------
protected:
  virtual void BeginPlay() override;

  // Reference to Owner Character
  UPROPERTY()
  ASlashPlayerCharacter *OwnerCharacter;

  void AttachWeaponInternalInfo(ASlashEquippableItemMaster *Weapon,
                                EWeaponEquipSource Source);
  EWeaponStateTypeEnum
  GetWeaponStateTypeFromWeapon(EWeaponTypeEnum WeaponType) const;

#pragma region Protected Equip/UnEquip Functions

  UAnimMontage *GetEquipMontage(FEquippableStruct EquippableStruct);
  UAnimMontage *GetUnequipMontage(FEquippableStruct EquippableStruct);

#pragma endregion

private:
  ASlashEquippableItemMaster *WeaponItem;
};
