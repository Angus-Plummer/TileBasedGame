// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Unit.generated.h"

UCLASS()
class TILEBASEDGAME_API AUnit : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AUnit();

private:

	UPROPERTY(EditAnywhere) 
	int32 Team; // team that the units belongs to

	UPROPERTY(EditAnywhere)
	int32 ClassID; // id of the unit's class

	// ---------- unit's stats ---------- //

	// current HP and AP
	UPROPERTY(EditAnywhere)
	int32 HitPoints;
	UPROPERTY(EditAnywhere)
	int32 AbilityPoints;

	// Base stats (before potential modification by buffs/debuffs)
	// These should not be accessed directly, but accessed from their respective Get functions which will take any modifiers into account
	UPROPERTY(EditAnywhere)
	int32 MaxHitPoints; // starting number of hitpoints of the unit
	
	UPROPERTY(EditAnywhere)
	int32 MaxAbilityPoints; // maximum number of ability points
	UPROPERTY(EditAnywhere)
	int32 AbilityPointRate; // rate of ability point generation per turn
	
	UPROPERTY(EditAnywhere)
	int32 Movement; // the maximum number of tiles this unit can move on their turn

	UPROPERTY(EditAnywhere)
	int32 Armour; // the damage reduction applied to any physical damage taken
	UPROPERTY(EditAnywhere)
	int32 MagicResist; // the damage reduction applied to any magical damage taken

	// ---------- buffs and abilities ---------- //

	//TArray<ABuff> Buffs;

	//AAbility DefaultAbility;

	//TArray<AAbility> Abilities;

	// --------- flags for handling actions ---------- //
	UPROPERTY(EditAnywhere)
	bool CanAttack; // whether the unit can use their default ability
	UPROPERTY(EditAnywhere)
	bool CanMove; // whether the unit can move
	UPROPERTY(EditAnywhere)
	bool CanUseAbilities; // whether the unit can use their abilities


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	int32 GetTeam() const;
	
	// ---------- Start and end of turn handlers ---------- //

	// in these functions the unit is set up for the start of a turn ans shut down correctly at end of turn
	// also loop through buffs and apply any effects they have: Buff.OnTurnStart(this);
	void OnTurnStart();
	void OnTurnEnd();

	// ---------- Actions that the unit can take ---------- //
	
	// check if the unit has the given ability in its abilities array and also has enough AP to use it
	//bool CanUseAbility(Ability* Ability) 
	
	// use an ability, with the given target and reduce AP appropriately. 
		// actually use ability by Ability.Cast( caster (this), targetposition)  
	//void UseAbility(Ability* Ability, FIntAbility TargetPosition) 

	// ---------- Applying actions that can be done to the unit ---------- //

	// apply damage to the unit
	void ApplyDamage(const int32& RawDamage, const bool MagicDamage);

	// apply a heal to the unit, limiting the new HP value to the max HP value
	void ApplyHeal(const int32& RawHeal);

	// performs the necessary action required when a unit dies
	void TriggerDeath();

	// void ApplyBuff(ABuff buff);

	// ---------- Getters for modified stat values ---------- //

	int32 GetMaxHitPoints();

	int32 GetMaxAbilityPoints();

	int32 GetAbilityPointRate();

	int32 GetMovement();

	int32 GetArmour();

	int32 GetMagicResist();


	// ---------- get buff effects which don't effect base stats --------- //

	int32 DamageModifier();

	bool IsTargetable();
	
};
