#pragma once
#pragma pack(push,1)
#ifndef CHAR_H
#define CHAR_H
#include "Blam\Cache\DataTypes\DataTypes.h"
#include "Blam\Cache\TagGroups.hpp"
#include "Blam\Cache\TagBlocks\TagBlock.h"

using namespace Blam::Cache::DataTypes;

struct character_tag_group
{
	int CharacterFlags; // pad fuck it.
	//BitField32 CharacterFlags;
	//[TagRefence("char")]
	tagRef ParentCharacter;
	//[TagRefence("unit")]
	tagRef Unit;
	//[TagRefence("crea")]
	tagRef Creature;
	//[TagRefence("styl")]
	tagRef Style;
	//[TagRefence("char")]
	tagRef MajorCharacter;
	
	__int64 Variants;
	//Reflexive Variants;
	__int64 GeneralProperties;
	//Reflexive GeneralProperties;
	__int64 VitalityProperties;
	//Reflexive VitalityProperties;
	__int64 PlacementProperties;
	//Reflexive PlacementProperties;
	__int64 PerceptionProperties;
	//Reflexive PerceptionProperties;
	__int64 LookProperties;
	//Reflexive LookProperties;
	__int64 MovementProperties;
	//Reflexive MovementProperties;
	tag_block<character_swarm_block> SwarmProperties;
	__int64 ReadyProperties;
	//Reflexive ReadyProperties;
	__int64 EngageProperties;
	//Reflexive EngageProperties;
	__int64 ChargeProperties;
	//Reflexive ChargeProperties;
	__int64 EvasionProperties;
	//Reflexive EvasionProperties;
	__int64 CoverProperties;
	//Reflexive CoverProperties;
	__int64 RetreatProperties;
	//Reflexive RetreatProperties;
	__int64 SearchProperties;
	//Reflexive SearchProperties;
	__int64 PreSearchProperties;
	//Reflexive PreSearchProperties;
	__int64 IdleProperties;
	//Reflexive IdleProperties;
	__int64 VocalizationProperties;
	//Reflexive VocalizationProperties;
	__int64 BoardingProperties;
	//Reflexive BoardingProperties;
	__int64 BossProperties;
	//Reflexive BossProperties;
	__int64 WeaponsProperties;
	//Reflexive WeaponsProperties;
	__int64 FiringPatternProperties;
	//Reflexive FiringPatternProperties;
	__int64 GrenadeProperties;
	//Reflexive GrenadeProperties;
	__int64 VehicleProperties;
	//Reflexive VehicleProperties;
};
#endif
#pragma pack(pop)
