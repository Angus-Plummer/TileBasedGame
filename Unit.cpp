// Fill out your copyright notice in the Description page of Project Settings.

#include "Unit.h"


// Sets default values
AUnit::AUnit()
	: ClassID(0)
	, Team(0)
	, MaxHitPoints(10)
	, MaxAbilityPoints(6)
	, AbilityPoints(0)
	, AbilityPointRate(1)
	, Movement(2)
	, Armour(0)
	, MagicResist(0)
{
	// set starting hit points to max hit points
	HitPoints = MaxHitPoints;

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AUnit::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AUnit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

int32 AUnit::GetTeam() const
{
	return Team;
}

void AUnit::OnTurnStart()
{
	// set up action flags as unused (these may be modified later in OnTurnStart() by buffs such as stun, root, silence, etc)
	CanMove = true;
	CanAttack = true;
	CanUseAbilities = true;

	// increase the ability points of the unit by their current ability point rate (potentially modified by buffs)
	AbilityPoints += GetAbilityPointRate();
	// cap the ability points between 0 and the maximum ability point value
	if (AbilityPoints > MaxAbilityPoints)
	{
		AbilityPoints = MaxAbilityPoints;
	}
	else if (AbilityPoints < 0)
	{
		AbilityPoints = 0;
	}
}

void AUnit::OnTurnEnd()
{
	// set the action flags as used
	CanMove = false;
	CanAttack = false;
	CanUseAbilities = false;

	// apply any end of turn effects on the buffs
	// loop through buffs and use buff.OnTurnEnd(this)

}

void AUnit::ApplyDamage(const int32& RawDamage, const bool MagicDamage)
{
	
	int32 DamageTaken;

	// if the incoming damage was magic damage then reduce by magic resist. If physical then reduce by armour.
	if (MagicDamage)
	{
		DamageTaken = RawDamage - GetMagicResist();
	}
	else 
	{
		DamageTaken = RawDamage - GetArmour();
	}

	// cap the damage taken as cannot be negative
	if (DamageTaken < 0)
	{
		DamageTaken = 0;
	}

	// reduce the unit's HP by the damage taken
	HitPoints -= DamageTaken;

	// if the hit points have fallen to 0 or below then kill the unit
	if (HitPoints <= 0)
	{
		HitPoints = 0; // ensure ui doesn't display negative hp

		TriggerDeath();
	}
}

void AUnit::ApplyHeal(const int32& RawHeal)
{
	// increase HP by the heal amount
	HitPoints += RawHeal;

	// if the units HP is now above the maximum then set it to the maximum
	if (HitPoints > MaxHitPoints)
	{
		HitPoints = MaxHitPoints;
	}
}

void AUnit::TriggerDeath()
{
	// start any death animation
	// remove the unit from the map and game manager
	// delete all the ability and buffs the unit has

	// trigger destruction of the unit
	Destroy();
}

int32 AUnit::GetMaxHitPoints()
{
	// check buffs for max ap modifiers
	int32 BuffModifier = 0;

	int32 TotalMaxHitPoints = MaxHitPoints + BuffModifier;

	return TotalMaxHitPoints;
	
}

int32 AUnit::GetMaxAbilityPoints()
{
	// check buffs for max ap modifiers
	int32 BuffModifier = 0;

	int32 TotalMaxAbilityPoints = MaxAbilityPoints + BuffModifier;

	return TotalMaxAbilityPoints;
}

int32 AUnit::GetAbilityPointRate()
{
	// check buffs for ap rate modifiers
	int32 BuffModifier = 0;

	// apply to the base stat
	int32 TotalAbilityPointRate = AbilityPointRate + BuffModifier;

	return TotalAbilityPointRate;
}

int32 AUnit::GetMovement()
{

	// check buffs for movement modifiers
	int32 BuffModifier = 0;

	// apply to the base stat
	int32 TotalMovement = Movement + BuffModifier;

	return TotalMovement;
}

int32 AUnit::GetArmour()
{
	// check buffs for armour modifiers
	int32 BuffModifier = 0;

	// apply to the base stat
	int32 TotalArmour = Armour + BuffModifier;

	return TotalArmour;
}

int32 AUnit::GetMagicResist()
{
	// check buffs for magic resist modifiers
	int32 BuffModifier = 0;

	// apply to the base stat
	int32 TotalMagicResist = MagicResist + BuffModifier;

	return TotalMagicResist;
}

int32 AUnit::DamageModifier()
{	
	// check buffs for damage modifiers
	int32 BuffModifier = 0;

	return BuffModifier;
}

bool AUnit::IsTargetable()
{
	bool Targetable = true;

	// check if any buffs make the unit untargetable

	return Targetable;
}

