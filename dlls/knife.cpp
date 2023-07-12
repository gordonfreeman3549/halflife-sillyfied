#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"
#include "UserMessages.h"

LINK_ENTITY_TO_CLASS(weapon_knife, CKnife);

void CKnife::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_knife.mdl");
	m_iId = WEAPON_KNIFE;
	m_iDefaultAmmo = -1; // How much ammo this weapon has on spawn
	FallInit();			 // get ready to fall down.
}

void CKnife::Precache()
{
	PRECACHE_MODEL("models/v_knife.mdl");
	PRECACHE_MODEL("models/w_knife.mdl");
	PRECACHE_MODEL("models/p_knife.mdl");



	PRECACHE_SOUND("weapons/knife.wav");
}

bool CKnife::GetItemInfo(ItemInfo* p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = NULL;	 // Which ammo type this weapon use
	p->iMaxAmmo1 = NULL; // What's the max ammo quantity for that kind of ammo
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = NULL;
	p->iMaxClip = -1; // How many ammo this weapon's clip or magazine has
	p->iSlot = 0;	  // Which "slot" (column) in the HUD this weapon is located (2 = same slot as HL1 MP5, shotgun, crossbow)
	p->iPosition = 1; // Which "position" (row) in the HUD this weapon is located (3 = after HL1 crossbow)
	p->iFlags = 0;	  // Special flags this weapon has
	p->iId = m_iId = WEAPON_KNIFE;
	p->iWeight = MP5_WEIGHT; // How much "priority" this weapon has when auto-switch is triggered

	return true;
}

bool CKnife::Deploy()
{
	//  The last parameter is the animation set for the player model in thirdperson to use
	return DefaultDeploy("models/v_knife.mdl", "models/p_knife.mdl", KNIFE_DRAW, "crowbar");
}

void CKnife::PrimaryAttack()
{


	// Size of the muzzle flash and how much volume in the world the firing sound produces
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_LIGHT;

	// player "shoot" animation
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = m_pPlayer->GetAutoaimVector(AUTOAIM_CAMERA);
	Vector vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, 64, BULLET_PLAYER_MP5,
		1, 30, m_pPlayer->pev, m_pPlayer->random_seed);

	// Play view model animation and firing sound
	SendWeaponAnim(KNIFE_SHOOT1);
	EMIT_SOUND(edict(), CHAN_AUTO, "weapons/knife.wav", 1, ATTN_NORM);


	// Next time for attack and weapon idling
	m_flNextPrimaryAttack = 0.80;//75 rpm
	m_flTimeWeaponIdle = 3;
}

void CKnife::Reload()
{
	return;
}


void CKnife::WeaponIdle()
{
	ResetEmptySound();

	// Not the time to idle
	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	// Play idle animation
	SendWeaponAnim(KNIFE_IDLE);
}
