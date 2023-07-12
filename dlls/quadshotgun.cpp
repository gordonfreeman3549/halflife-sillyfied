#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"
#include "UserMessages.h"

LINK_ENTITY_TO_CLASS(weapon_quad, CQuad);

void CQuad::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_quad.mdl");
	m_iId = WEAPON_QUAD;
	m_iDefaultAmmo = 6; // ammo on spawn
	FallInit();			
}

void CQuad::Precache()
{
	PRECACHE_MODEL("models/v_quad.mdl");
	PRECACHE_MODEL("models/w_quad.mdl");
	PRECACHE_MODEL("models/p_quad.mdl");
	PRECACHE_SOUND("weapons/quad.wav");
}

bool CQuad::GetItemInfo(ItemInfo* p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "buckshot";		   // 4 barrelled shotgun that uses slug shells (shared with pepshot and spas-12)
	p->iMaxAmmo1 = BUCKSHOT_MAX_CARRY; // 125
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = NULL;
	p->iMaxClip = 2;  // 4 barrels
	p->iSlot = 4;	  // 2 = smg, shotgun, crossbow (BUG! sprite doesn't show in game? gotta ask for help)
	p->iPosition = 1; // below m40a1 scopeless		(BUG! same here)
	p->iFlags = 0;	  //no idea
	p->iId = m_iId = WEAPON_QUAD; //errm errm also no idea but must be important
	p->iWeight = MP5_WEIGHT; 

	return true;
}

bool CQuad::Deploy()
{
	return DefaultDeploy("models/v_quad.mdl", "models/p_quad.mdl", QUAD_DRAW, "shotgun");
}

void CQuad::PrimaryAttack()
{
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	// don't fire if empty
	if (m_iClip <= 0)
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	// nuzzle flash and sound volume
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	// player "shoot" animation
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);
	Vector vecDir = m_pPlayer->FireBulletsPlayer(24, vecSrc, vecAiming, VECTOR_CONE_20DEGREES, 2048, BULLET_PLAYER_BUCKSHOT,
		1, 8, m_pPlayer->pev, m_pPlayer->random_seed);

	// play view model animation and firing sound
	SendWeaponAnim(	QUAD_SHOOT1 + RANDOM_LONG(0, 2));
	EMIT_SOUND(edict(), CHAN_AUTO, "weapons/quad.wav", 1, ATTN_NORM);


	// recoil
	m_pPlayer->pev->punchangle.x -= 7;
	m_iClip--;
	m_iClip--;
	m_flNextPrimaryAttack = 1.5;
	m_flTimeWeaponIdle = 1.85;
}

void CQuad::Reload()
{
	DefaultReload(2, QUAD_RELOAD, 1.7);
}


void CQuad::WeaponIdle()
{
	ResetEmptySound();

	
	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	
	SendWeaponAnim(QUAD_IDLE);
}
