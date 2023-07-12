#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"
#include "UserMessages.h"

LINK_ENTITY_TO_CLASS(weapon_agun, CAgun);

void CAgun::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_57.mdl");
	m_iId = WEAPON_AGUN;
	m_iDefaultAmmo = 250; // How much ammo this weapon has on spawn
	FallInit();			  // get ready to fall down.
	
}

void CAgun::Precache()
{
	return;
}

bool CAgun::GetItemInfo(ItemInfo* p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = ".22"; // Which ammo type this weapon use
	p->iMaxAmmo1 = 250;	 // What's the max ammo quantity for that kind of ammo
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = NULL;
	p->iMaxClip = 15; // How many ammo this weapon's clip or magazine has
	p->iSlot = 1;	   // Which "slot" (column) in the HUD this weapon is located (2 = same slot as HL1 MP5, shotgun, crossbow)
	p->iPosition = 5;  // Which "position" (row) in the HUD this weapon is located (3 = after HL1 crossbow)
	p->iFlags = 0;	   // Special flags this weapon has
	p->iId = m_iId = WEAPON_AGUN;
	p->iWeight = MP5_WEIGHT; // How much "priority" this weapon has when auto-switch is triggered

	return true;
}

bool CAgun::Deploy()
{
	//  The last parameter is the animation set for the player model in thirdperson to use
	return DefaultDeploy("models/v_agun.mdl", "models/p_crowbar.mdl", AGUN_DRAW, "mp5");
}

void CAgun::PrimaryAttack()
{
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	// Don't fire if empty
	if (m_iClip <= 0)
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	// Size of the muzzle flash and how much volume in the world the firing sound produces
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	// player "shoot" animation
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);
	Vector vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, 8192, BULLET_PLAYER_MP5,
		1, 5, m_pPlayer->pev, m_pPlayer->random_seed);

	// Play view model animation and firing sound
	SendWeaponAnim(AGUN_SHOOT1);
	EMIT_SOUND(edict(), CHAN_AUTO, "weapons/agun.wav", 1, ATTN_NORM);

	// Punch the camera to simulate recoil
	m_pPlayer->pev->punchangle.x -= 1.2;
	// Remove a bullet
	m_iClip--;
	// Next time for attack and weapon idling
	m_flNextPrimaryAttack = 0.25;
	m_flTimeWeaponIdle = 2;
}

void CAgun::Reload()
{
	// Reload 30 bullets, play the AK47_RELOAD animation, reload duration is 2.5 seconds
	DefaultReload(15, AGUN_RELOAD, 2.2);
}


void CAgun::WeaponIdle()
{
	ResetEmptySound();

	// Not the time to idle
	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	// Play idle animation
	SendWeaponAnim(AGUN_IDLE);
}
