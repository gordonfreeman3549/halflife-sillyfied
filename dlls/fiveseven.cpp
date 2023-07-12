// HALF-LIFE: SILLYFIED
// FN Five-Seven
// Stats			[cal 5.7]	[dmg 9] [mag 20] [max 150] [acc 3] [rpm 480] [recoil 1.4 +1] 
// Unbalanced Stats [cal 5.56]	[dmg 9] [mag 20] [max 180] [acc 5] [rpm 420] [recoil 1.4]
// Changes----------------------------------------------------------------
// ammo 5.56 > 5.7, max 180 > 150, acc 5 > 3, rpm 0.15 > 0.125 (03/06/23)


#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"
#include "UserMessages.h"

LINK_ENTITY_TO_CLASS(weapon_fs, CFS);

void CFS::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_57.mdl");
	m_iId = WEAPON_FS;
	m_iDefaultAmmo = 60; // How much ammo this weapon has on spawn
	FallInit();			// get ready to fall down.
	
}

void CFS::Precache()
{
	PRECACHE_MODEL("models/v_57.mdl");
	PRECACHE_MODEL("models/w_57.mdl");
	PRECACHE_MODEL("models/p_57.mdl");

	m_iShell = PRECACHE_MODEL("models/rshell.mdl"); // brass shell

	PRECACHE_SOUND("weapons/57.wav");
}

bool CFS::GetItemInfo(ItemInfo* p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "57"; // 5.56 > 5.7
	p->iMaxAmmo1 = 150;	 //180 > 150
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = NULL;
	p->iMaxClip = 20;  // How many ammo this weapon's clip or magazine has
	p->iSlot = 1;	  // Which "slot" (column) in the HUD this weapon is located (2 = same slot as HL1 MP5, shotgun, crossbow)
	p->iPosition = 2; // Which "position" (row) in the HUD this weapon is located (3 = after HL1 crossbow)
	p->iFlags = 0;	  // Special flags this weapon has
	p->iId = m_iId = WEAPON_FS;
	p->iWeight = MP5_WEIGHT; // How much "priority" this weapon has when auto-switch is triggered

	return true;
}

bool CFS::Deploy()
{
	//  The last parameter is the animation set for the player model in thirdperson to use
	return DefaultDeploy("models/v_57.mdl", "models/p_57.mdl", FS_DRAW, "mp5");
	
}

void CFS::PrimaryAttack()
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
	Vector vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_3DEGREES, 8192, BULLET_PLAYER_MP5,
		1, 9, m_pPlayer->pev, m_pPlayer->random_seed);

	// Play view model animation and firing sound
	SendWeaponAnim(FS_SHOOT1 + RANDOM_LONG(0, 2));
	EMIT_SOUND(edict(), CHAN_AUTO, "weapons/57.wav", 1, ATTN_NORM);

	// Eject the brass
	Vector vecShellVelocity = m_pPlayer->pev->velocity + gpGlobals->v_right * RANDOM_FLOAT(100, 200) +
							  gpGlobals->v_up * RANDOM_FLOAT(100, 150) + gpGlobals->v_forward * 25;
	EjectBrass(pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_up * -12 + gpGlobals->v_forward * 9 +
				   gpGlobals->v_right * 6,
		vecShellVelocity, pev->angles.y, m_iShell, TE_BOUNCE_SHELL);

	// Punch the camera to simulate recoil
	m_pPlayer->pev->punchangle.x -= 1.4;
	// Remove a bullet
	m_iClip--;
	// Next time for attack and weapon idling
	m_flNextPrimaryAttack = 0.125;
	m_flTimeWeaponIdle = 2;
}

void CFS::Reload()
{
	// Reload 30 bullets, play the AK47_RELOAD animation, reload duration is 2.5 seconds
	DefaultReload(20, FS_RELOAD, 3.3);
}


void CFS::WeaponIdle()
{
	ResetEmptySound();

	// Not the time to idle
	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	// Play idle animation
	SendWeaponAnim(FS_IDLE);
}
