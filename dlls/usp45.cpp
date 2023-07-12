#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"
#include "UserMessages.h"

LINK_ENTITY_TO_CLASS(weapon_usp, CUsp);

void CUsp::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_usp.mdl");
	m_iId = WEAPON_USP;
	m_iDefaultAmmo = 50; // ammo on spawn
	FallInit();
}

void CUsp::Precache()
{
	PRECACHE_MODEL("models/v_usp.mdl");
	PRECACHE_MODEL("models/w_usp.mdl");
	PRECACHE_SOUND("weapons/usp.wav");
	PRECACHE_MODEL("models/p_usp.mdl");
	PRECACHE_MODEL("models/v_agun.mdl");
	PRECACHE_MODEL("models/w_57.mdl");
	PRECACHE_SOUND("weapons/agun.wav");
	m_iShell = PRECACHE_MODEL("models/shell.mdl"); // brass shell
}

bool CUsp::GetItemInfo(ItemInfo* p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "45acp";		   // 4 barrelled shotgun that uses slug shells (shared with pepshot and spas-12)
	p->iMaxAmmo1 = 192; // 125
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = NULL;
	p->iMaxClip = 13;			  // 4 barrels
	p->iSlot = 1;				  // 2 = smg, shotgun, crossbow (BUG! sprite doesn't show in game? gotta ask for help)
	p->iPosition = 4;			  // below m40a1 scopeless		(BUG! same here)
	p->iFlags = 0;				  // no idea
	p->iId = m_iId = WEAPON_USP; // errm errm also no idea but must be important
	p->iWeight = MP5_WEIGHT;

	return true;
}

bool CUsp::Deploy()
{
	return DefaultDeploy("models/v_usp.mdl", "models/p_usp.mdl", USP_DRAW, "python");
}

void CUsp::PrimaryAttack()
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
	Vector vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_3DEGREES, 8192, BULLET_PLAYER_MP5,
		1, 12, m_pPlayer->pev, m_pPlayer->random_seed);

	// play view model animation and firing sound
	SendWeaponAnim(USP_SHOOT1 + RANDOM_LONG(0, 2));
	EMIT_SOUND(edict(), CHAN_AUTO, "weapons/usp.wav", 1, ATTN_NORM);

	// Eject the brass
	Vector vecShellVelocity = m_pPlayer->pev->velocity + gpGlobals->v_right * RANDOM_FLOAT(100, 200) +
							  gpGlobals->v_up * RANDOM_FLOAT(100, 150) + gpGlobals->v_forward * 25;
	EjectBrass(pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_up * -5 + gpGlobals->v_forward * 15 +
				   gpGlobals->v_right * 5,
		vecShellVelocity, pev->angles.y, m_iShell, TE_BOUNCE_SHELL);


	// recoil
	m_pPlayer->pev->punchangle.x -= 1.2;
	m_iClip--;
	m_flNextPrimaryAttack = 0.17; //353 rpm
	m_flTimeWeaponIdle = 1.85;
}

void CUsp::Reload()
{
	DefaultReload(13, USP_RELOAD, 2.7);
}


void CUsp::WeaponIdle()
{
	ResetEmptySound();


	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;


	SendWeaponAnim(USP_IDLE);
}
