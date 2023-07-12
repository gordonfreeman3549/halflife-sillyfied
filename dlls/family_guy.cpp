#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"
#include "UserMessages.h"

LINK_ENTITY_TO_CLASS(weapon_fg, CFG);

void CFG::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_crowbar.mdl");
	m_iId = WEAPON_FG;
	m_iDefaultAmmo = -1; // How much ammo this weapon has on spawn
	FallInit();			// get ready to fall down.
}

void CFG::Precache()
{
	PRECACHE_MODEL("models/v_fg.mdl");

}

bool CFG::GetItemInfo(ItemInfo* p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL; // Which ammo type this weapon use
	p->iMaxAmmo1 = NULL;	 // What's the max ammo quantity for that kind of ammo
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = NULL;
	p->iMaxClip = NULL;  // How many ammo this weapon's clip or magazine has
	p->iSlot = 0;	  // Which "slot" (column) in the HUD this weapon is located (2 = same slot as HL1 MP5, shotgun, crossbow)
	p->iPosition = 5; // Which "position" (row) in the HUD this weapon is located (3 = after HL1 crossbow)
	p->iFlags = 0;	  // Special flags this weapon has
	p->iId = m_iId = WEAPON_FG;
	p->iWeight = MP5_WEIGHT; // How much "priority" this weapon has when auto-switch is triggered

	return true;
}

bool CFG::Deploy()
{
	//  The last parameter is the animation set for the player model in thirdperson to use
	return DefaultDeploy("models/v_fg.mdl", "models/p_crowbar.mdl", FG_DRAW, "mp5");
}

void CFG::PrimaryAttack()
{
	return;
}

void CFG::Reload()
{
	return;
}


void CFG::WeaponIdle()
{
	ResetEmptySound();

	// Not the time to idle
	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	// Play idle animation
	SendWeaponAnim(FG_IDLE);
}