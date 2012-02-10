/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2000-2006 Tim Angus

This file is part of Tremulous.

Tremulous is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Tremulous is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Tremulous; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/


/*
 * ALIEN weapons
 *
 * _REPEAT  - time in msec until the weapon can be used again
 * _DMG     - amount of damage the weapon does
 *
 * ALIEN_WDMG_MODIFIER - overall damage modifier for coarse tuning
 *
 */

#define ALIEN_WDMG_MODIFIER         1.0f
#define ADM(d)                      ((int)((float)d*ALIEN_WDMG_MODIFIER))

#define ABUILDER_BUILD_REPEAT       500
#define ABUILDER_CLAW_DMG           ADM(24) //20)
#define ABUILDER_CLAW_RANGE         64.0f
#define ABUILDER_CLAW_WIDTH         4.0f
#define ABUILDER_CLAW_REPEAT        800
#define ABUILDER_BASE_CLAW_REPEAT   1000
#define ABUILDER_CLAW_K_SCALE       1.0f
#define ABUILDER_BASE_DELAY         12000 //17000
#define ABUILDER_ADV_DELAY          8000 //12000

#define ABUILDER_BLOB_DMG           ADM(6)//4
#define ABUILDER_BLOB_RADIUS        75     //explosion radius
#define ABUILDER_BLOB_REPEAT        500
#define ABUILDER_BLOB_SPEED         800.0f //800
#define ABUILDER_BLOB_SPEED_MOD     0.6f
#define ABUILDER_BLOB_TIME          5000 //5000 how long a guy gets slowed down

#define LEVEL0_BITE_DMG             ADM(36)
#define LEVEL0_BITE_RANGE           64.0f
#define LEVEL0_BITE_WIDTH           6.0f
#define LEVEL0_BITE_REPEAT          333 //500
#define LEVEL0_BITE_K_SCALE         1.0f
#define LEVEL0_SCRATCH_DMG          4 //dispite this, it is actually almost overpowered.
#define LEVEL0_SCRATCH_REPEAT       100 //Main pupous of this attack is to drain health slowly and gain it just like mara zap.
#define LEVEL0_SCRATCH_RANGE        80

#define LEVEL1_CLAW_DMG             ADM(48)
#define LEVEL1_CLAW_RANGE           96.0f //96
#define LEVEL1_CLAW_WIDTH           10.0f
#define LEVEL1_CLAW_REPEAT          666
#define LEVEL1_CLAW_U_REPEAT        500
#define LEVEL1_CLAW_K_SCALE         -1.0f
#define LEVEL1_CLAW_U_K_SCALE       -1.0f
#define LEVEL1_GRAB_RANGE           96.0f//64
#define LEVEL1_GRAB_TIME            300
#define LEVEL1_GRAB_U_TIME          450
#define LEVEL1_PCLOUD_DMG           ADM(8)
#define LEVEL1_PCLOUD_RANGE         600.0f //300
#define LEVEL1_PCLOUD_REPEAT        1200 //2000
#define LEVEL1_PCLOUD_TIME          10000 //

#define LEVEL2_CLAW_DMG             ADM(42)
#define LEVEL2_CLAW_RANGE           128.0f //96
#define LEVEL2_CLAW_WIDTH           18.0f //12f
#define LEVEL2_CLAW_REPEAT          500
#define LEVEL2_CLAW_K_SCALE         1.0f//1
#define LEVEL2_CLAW_U_REPEAT        333 //Okay, i admit, it is overpowered. So i decided to change it back to before. (original Lolards qvm had 250 here and 333 in normal mar)
#define LEVEL2_CLAW_U_K_SCALE       1.0f//1
#define LEVEL2_AREAZAP_DMG          ADM(120) //default 80 //but remember, this is dealt over 3 seconds.
#define LEVEL2_AREAZAP_RANGE        360.0f // default 300 //Overpowered, but its good practice to stay withing a 360 area first because its easier.
#define LEVEL2_AREAZAP_WIDTH        30.0f //30 //keep small or it will 'hit a wall'
#define LEVEL2_AREAZAP_REPEAT       750 //1500 //max of 5 can overlap in 3 seconds
#define LEVEL2_AREAZAP_TIME         3000 //1000
#define LEVEL2_AREAZAP_MAX_TARGETS  7 //whole shit here. But it is to help againts groups of humans and base rushes.
#define LEVEL2_WALLJUMP_MAXSPEED    1290.0f //129

//Goon modified heavily to get close to gpp values.
#define LEVEL3_CLAW_DMG             ADM(82)
#define LEVEL3_CLAW_RANGE           72.0f //96
#define LEVEL3_CLAW_WIDTH           16.0f
#define LEVEL3_CLAW_REPEAT          700 //usually 700 //900 for gpp-related
#define LEVEL3_CLAW_K_SCALE         1.0f
#define LEVEL3_CLAW_U_REPEAT        600 //usuaULLY 600 //800 ^^what i said
#define LEVEL3_CLAW_U_K_SCALE       1.0f
#define LEVEL3_POUNCE_DMG           ADM(180) //default 100, but i increased it to relate to gpp values of killing armoured.
#define LEVEL3_POUNCE_RANGE         48.0f //default 72. Remember 0 is at the centre, it won't hit anything. Want 36, but can't hit because it still isn't long enough :( Gpp's value is probably 48, cause thats what it says in TremX's source code.
#define LEVEL3_POUNCE_WIDTH         32.0f //default 16. Also remember that the attacking stuff is in a form of a boundry box Any wider will cause it to hit the sides first. 32 for a more 'realistic' pounce so adv goons can get humans without aiming down. Works almost perfectly at this setting. It is basically the width of the goon/2.
#define LEVEL3_POUNCE_SPEED         700 //pounce speed usually 700
#define LEVEL3_POUNCE_UPG_SPEED     800 //pounce speed for adv goon usually 800
#define LEVEL3_POUNCE_SPEED_MOD     0.75f //speed when charging pounce default 0.75
#define LEVEL3_POUNCE_CHARGE_TIME   700 //charge time, default 700
#define LEVEL3_POUNCE_TIME          400 //delay after touching ground?!? default 400
#define LEVEL3_BOUNCEBALL_DMG       ADM(135) //110 default. Not to be increased higher than 160 as it can kill reactor easily. //180 //decreased as it is now explosive like gpp.

//I've added these to g_missile.c and bg_misc.c already for you.
#define LEVEL3_BOUNCEBALL_AMMO      3
#define LEVEL3_BOUNCEBALL_REPEAT    600 //default 1000 //deal with long range turrets
#define LEVEL3_BOUNCEBALL_SPEED     1000.0f //default 1000 //1200 for longer range
#define LEVEL3_BOUNCEBALL_SPLASH_DMG  110 //Explosion dmg.
#define LEVEL3_BOUNCEBALL_SPLASH_RADIUS 92 //Splash radius. Helps with sniping. //Rememeber that reload time is 10 seconds. You don't want it overpowered.

#define LEVEL4_CLAW_DMG             ADM(110)
#define LEVEL4_CLAW_RANGE           128.0f
#define LEVEL4_CLAW_WIDTH           20.0f
#define LEVEL4_CLAW_REPEAT          750 //750
#define LEVEL4_CLAW_K_SCALE         1.0f
#define LEVEL4_REGEN_RANGE          200.0f
#define LEVEL4_REGEN_MOD            1.5f //2 //cannot benifit from booster's x3 but i think tyrant's normal healing is already enough (~18hp/s)
#define LEVEL4_CHARGE_SPEED         3.0f
#define LEVEL4_CHARGE_TIME          3000 //2000
#define LEVEL4_CHARGE_CHARGE_TIME   1500
#define LEVEL4_MIN_CHARGE_TIME      750
#define LEVEL4_CHARGE_CHARGE_RATIO  (LEVEL4_CHARGE_TIME/LEVEL4_CHARGE_CHARGE_TIME)
#define LEVEL4_CHARGE_REPEAT        1000
#define LEVEL4_CHARGE_DMG           ADM(209) //100 (to help with armoured)

//Note: tyrant spit bomb is a trapper bomb, so it shares the facts of a trapper.
//Change trapper facts if you want to change the stats of the spit.



/*
 * ALIEN classes
 *
 * _SPEED   - fraction of Q3A run speed the class can move
 * _REGEN   - health per second regained
 *
 * ALIEN_HLTH_MODIFIER - overall health modifier for coarse tuning
 *
 */

#define ALIEN_HLTH_MODIFIER         1.0f
#define AHM(h)                      ((int)((float)h*ALIEN_HLTH_MODIFIER))

#define ALIEN_VALUE_MODIFIER        1.0f
#define AVM(h)                      ((int)((float)h*ALIEN_VALUE_MODIFIER))

#define ABUILDER_SPEED              0.8f
#define ABUILDER_VALUE              AVM(170)
#define ABUILDER_HEALTH             AHM(60)
#define ABUILDER_REGEN              2
#define ABUILDER_COST               0

#define ABUILDER_UPG_SPEED          1.0f
#define ABUILDER_UPG_VALUE          AVM(220)
#define ABUILDER_UPG_HEALTH         AHM(90)
#define ABUILDER_UPG_REGEN          3
#define ABUILDER_UPG_COST           0

#define LEVEL0_SPEED                1.4f
#define LEVEL0_VALUE                AVM(160) //default 175, 180 for gpp. I forgot the values for the basilisks and mars.
#define LEVEL0_HEALTH               AHM(30)
#define LEVEL0_REGEN                1
#define LEVEL0_COST                 0

#define LEVEL1_SPEED                1.2f
#define LEVEL1_VALUE                AVM(280)
#define LEVEL1_HEALTH               AHM(80)
#define LEVEL1_REGEN                2
#define LEVEL1_COST                 1

//head on fighting 1.1 advbasi! Note that human killing one of these will get a lot more credits than normal
#define LEVEL1_UPG_SPEED            1.2f
#define LEVEL1_UPG_VALUE            AVM(475) //275
#define LEVEL1_UPG_HEALTH           AHM(125)
#define LEVEL1_UPG_REGEN            4
#define LEVEL1_UPG_COST             2

#define LEVEL2_SPEED                1.2f
#define LEVEL2_VALUE                AVM(320)
#define LEVEL2_HEALTH               AHM(100)
#define LEVEL2_REGEN                3
#define LEVEL2_COST                 1

#define LEVEL2_UPG_SPEED            1.3f
#define LEVEL2_UPG_VALUE            AVM(575)
#define LEVEL2_UPG_HEALTH           AHM(192)
#define LEVEL2_UPG_REGEN            6
#define LEVEL2_UPG_COST             3

#define LEVEL3_SPEED                1.1f
#define LEVEL3_VALUE                AVM(400)
#define LEVEL3_HEALTH               AHM(236)
#define LEVEL3_REGEN                7
#define LEVEL3_COST                 2

#define LEVEL3_UPG_SPEED            1.1f
#define LEVEL3_UPG_VALUE            AVM(730)
#define LEVEL3_UPG_HEALTH           AHM(275)
#define LEVEL3_UPG_REGEN            8
#define LEVEL3_UPG_COST             2

#define LEVEL4_SPEED                1.1f //1.2f
#define LEVEL4_VALUE                AVM(880)
#define LEVEL4_HEALTH               AHM(480)
#define LEVEL4_REGEN                (0.025f * LEVEL4_HEALTH)
#define LEVEL4_COST                 3

//Basi-Suit (hidden extra, costing 9 evos, or 11 if tyrant. Somehow this is a wasted class as humans don't 'morph' into a battlesuit, so i can use it as a seperate class unlike gpp =D) Values change for bg_misc.c only.
#define SPY_SPEED                   1.0f
#define SPY_HEALTH                  300
#define SPY_VALUE                   720
#define SPY_COST                    5
#define SPY_REGEN                   1

/*
 * ALIEN buildables
 *
 * _BP            - build points required for this buildable
 * _BT            - build time required for this buildable
 * _REGEN         - the amount of health per second regained
 * _SPLASHDAMGE   - the amount of damage caused by this buildable when melting
 * _SPLASHRADIUS  - the radius around which it does this damage
 *
 * CREEP_BASESIZE - the maximum distance a buildable can be from an egg/overmind
 * ALIEN_BHLTH_MODIFIER - overall health modifier for coarse tuning
 *
 */

#define ALIEN_BHLTH_MODIFIER        1.0f
#define ABHM(h)                     ((int)((float)h*ALIEN_BHLTH_MODIFIER))

#define CREEP_BASESIZE              900 //700
#define CREEP_TIMEOUT               1000
#define CREEP_MODIFIER              0.5f
#define CREEP_ARMOUR_MODIFIER       0.75f
#define CREEP_SCALEDOWN_TIME        3000

#define ASPAWN_BP                   10
#define ASPAWN_BT                   12000
#define ASPAWN_HEALTH               ABHM(350) //250
#define ASPAWN_REGEN                8
#define ASPAWN_SPLASHDAMAGE         60
#define ASPAWN_SPLASHRADIUS         50
#define ASPAWN_CREEPSIZE            120
#define ASPAWN_VALUE                180 //default 150

#define BARRICADE_BP                8
#define BARRICADE_BT                12000
#define BARRICADE_HEALTH            ABHM(800)//default 200, yes i know thats hell weak. gpp is 300, but i put 600 to deal with luci spammers.
#define BARRICADE_REGEN             24
#define BARRICADE_SPLASHDAMAGE      150
#define BARRICADE_SPLASHRADIUS      150
#define BARRICADE_CREEPSIZE         240

#define BOOSTER_BP                  12
#define BOOSTER_BT                  15000
#define BOOSTER_HEALTH              ABHM(250) //default 150
#define BOOSTER_REGEN               18
#define BOOSTER_SPLASHDAMAGE        50
#define BOOSTER_SPLASHRADIUS        50
#define BOOSTER_CREEPSIZE           120 //also the size of the area it heals players
#define BOOSTER_INTERVAL            30000 //time in msec between uses (per player) //30000
#define BOOSTER_REGEN_MOD           3.0f //regen multipliyer, default is 2
#define BOOST_TIME                  60000 //poison reload/poison last in alien Default is 30000, gpp should be 20000

#define ACIDTUBE_BP                 6
#define ACIDTUBE_BT                 8000 //15000
#define ACIDTUBE_HEALTH             ABHM(137) //default 125
#define ACIDTUBE_REGEN              9
#define ACIDTUBE_SPLASHDAMAGE       13 //default 4, i'd rather 13 for balance, but 18 is to deal with those damn bsuits. Don't get confused this with other splash damages, the others apply to exploding...
#define ACIDTUBE_SPLASHRADIUS       380
#define ACIDTUBE_CREEPSIZE          120
#define ACIDTUBE_RANGE              380.0f //200
#define ACIDTUBE_REPEAT             2000
#define ACIDTUBE_K_SCALE            0.1f//1f

#define HIVE_BP                     14
#define HIVE_LIFETIME               15000
#define HIVE_BT                     20000
#define HIVE_HEALTH                 ABHM(252) //default 125, same as acidtube. //252 = rifle's damage per clip
#define HIVE_REGEN                  12
#define HIVE_SPLASHDAMAGE           30 //unknown default
#define HIVE_SPLASHRADIUS           200
#define HIVE_CREEPSIZE              120
#define HIVE_RANGE                  900.0f //300 //600
#define HIVE_REPEAT                 2500
#define HIVE_K_SCALE                -1.0f
#define HIVE_DMG                    97 //60 //120
#define HIVE_SPEED                  380.0f //280 //deal with jettards
#define HIVE_DIR_CHANGE_PERIOD      500 //500

#define TRAPPER_BP                  8
#define TRAPPER_BT                  6000 //12000
#define TRAPPER_HEALTH              ABHM(60) //default 50
#define TRAPPER_REGEN               6
#define TRAPPER_SPLASHDAMAGE        97
#define TRAPPER_SPLASHRADIUS        90
#define TRAPPER_CREEPSIZE           30
#define TRAPPER_RANGE               600 //default 300 or 200
#define TRAPPER_REPEAT              1000  //Wrong. Unused. Real value is 500.
#define TRAPPER_K_SCALE             1.0f //explosion. Keep positive for trapper/lockblob jumping. Negative value in g_missile.c
#define LOCKBLOB_SPEED              1250.0f //default 650
#define LOCKBLOB_LOCKTIME           20 //default 6000 //8000
//the REAL lifetime of lock because aliens now get locked as well
//refer to g_missile.c's blob code
#define LOCKBLOB_LIFETIME           6000
#define LOCKBLOB_DOT                0.85f // max angle = acos( LOCKBLOB_DOT )
#define LOCKBLOB_K_SCALE            -1.0f //1.0f
#define LOCKBLOB_DMG                97

#define OVERMIND_BP                 0
#define OVERMIND_BT                 20000
#define OVERMIND_HEALTH             ABHM(920)
#define OVERMIND_REGEN              32 //16
#define OVERMIND_SPLASHDAMAGE       24 //18 //36 //must be saw-able, use in-between values
#define OVERMIND_SPLASHRADIUS       300
#define OVERMIND_CREEPSIZE          120
#define OVERMIND_ATTACK_RANGE       150.0f //100 //200f too overpowered
#define OVERMIND_ATTACK_REPEAT      800 //1000 //500 - can't even get behind om
#define OVERMIND_VALUE              800

#define HOVEL_BP                    0
#define HOVEL_BT                    15000
#define HOVEL_HEALTH                ABHM(537) //370
#define HOVEL_REGEN                 30 //20
#define HOVEL_SPLASHDAMAGE          20
#define HOVEL_SPLASHRADIUS          200
#define HOVEL_CREEPSIZE             120



/*
 * ALIEN misc
 *
 * ALIENSENSE_RANGE - the distance alien sense is useful for
 *
 */

#define ALIENSENSE_RANGE            1000.0f //look up ^^ usually 1000, want 2000 but can't evolve in some areas 

#define ALIEN_POISON_TIME           12000 //miliseconds how long poison lasts for, usually 8000
#define ALIEN_POISON_DMG            7 //default 5
#define ALIEN_POISON_DIVIDER        (1.0f/1.32f) //about 1.0/(time`th root of damage)[1.0f/1.32f]

#define ALIEN_SPAWN_REPEAT_TIME     6000 // spawn repeat, default 12000, larger than hspawn

#define ALIEN_REGEN_DAMAGE_TIME     3000 //msec since damage that regen starts again //2000

/*
 * HUMAN weapons
 *
 * _REPEAT  - time between firings
 * _RELOAD  - time needed to reload
 * _PRICE   - amount in credits weapon costs
 * _K_SCALE - Knockback scale
 *
 * HUMAN_WDMG_MODIFIER - overall damage modifier for coarse tuning
 *
 */

#define HUMAN_WDMG_MODIFIER         1.0f
#define HDM(d)                      ((int)((float)d*HUMAN_WDMG_MODIFIER))

#define BLASTER_REPEAT              380 //500
#define BLASTER_K_SCALE             5.0f //knockback now noticable
#define BLASTER_SPREAD              200 //doesn't work, soon to be fixed
#define BLASTER_SPEED               1400 //1400 //2400
#define BLASTER_DMG                 HDM(8) //9 //10+ to help againts dretches //18 if 18limited ammo with a slow repeat //36 - strong with ammo but ammo gets used up fast //8 because faster shoot
#define BLASTER_CLIPSIZE            6
#define BLASTER_MAXCLIPS            9
#define BLASTER_MELEE               17
#define BLASTER_MELEE_RANGE         80

#define RIFLE_CLIPSIZE              42 //30 //36 //42
#define RIFLE_MAXCLIPS              9 //6
#define RIFLE_REPEAT                110 //100 //118 //70
#define RIFLE_K_SCALE               1.0f
#define RIFLE_RELOAD                1800
#define RIFLE_PRICE                 0
#define RIFLE_SPREAD                170
#define RIFLE_DMG                   HDM(6) //5 //6

#define PAINSAW_PRICE               100
#define PAINSAW_REPEAT              65 //enough to touch below feet of a normal human
#define PAINSAW_K_SCALE             1.0f
#define PAINSAW_DAMAGE              HDM(13)
#define PAINSAW_RANGE               60.0f //40f

//Super Grenade Throw!
#define GRENADE_PRICE               600 //200 //mininuke
#define GRENADE_REPEAT              0 //Default 0. Surely doesn't stop nade spams. (because you're discarding the weapon/upgrade as you use it, duh!)
#define GRENADE_K_SCALE             2.0f
#define GRENADE_DAMAGE              HDM(380) //210 //strong enough to kill egg or anything weaker.
#define GRENADE_RANGE               332.0f //192 //mininuke == 500
#define GRENADE_SPEED               780.0f //Throwing speed. Default 400

//reverse pump shotgun "semi-semi-auto"
#define SHOTGUN_PRICE               250
#define SHOTGUN_SHELLS              10
#define SHOTGUN_PELLETS             14 //used to sync server and client side //spray of defeat -18 //long range combat shot - 12
#define SHOTGUN_MAXCLIPS            7 //default 3
#define SHOTGUN_REPEAT              700 //1000
#define SHOTGUN_K_SCALE             2.0f
#define SHOTGUN_RELOAD              5000 //spray of defeat - 6000 //clip based - 3000
#define SHOTGUN_SPREAD              1320 //900 //1320 spray of defeat
#define SHOTGUN_DMG                 HDM(6) //spray of defeat - 6 //long range combat shot 6
#define SHOTGUN_BLAST               168 //damage for secondary. Bleedingly powerful,
#define SHOTGUN_BLAST_RANGE         350 //350 but this value must be small.
#define SHOTGUN_BLAST_REPEAT        2000

#define LASGUN_PRICE                150
#define LASGUN_AMMO                 370
#define LASGUN_REPEAT               180 //200
#define LASGUN_MAXCLIPS             1
#define LASGUN_K_SCALE              2.0f
#define LASGUN_RELOAD               3000
#define LASGUN_SPREAD               100 //works now =P (ripped from mgun)
#define LASGUN_DAMAGE               HDM(12) //9

//Mass Driver now a Sniper Rifle with a large cooldown
#define MDRIVER_PRICE               380
#define MDRIVER_CLIPSIZE            5
#define MDRIVER_MAXCLIPS            7
#define MDRIVER_DMG                 HDM(72) //(random( ) * 38 + 100)
#define MDRIVER_RADIUS              100    
#define MDRIVER_REPEAT              1320 //2000 - wearer must remember to put ammo in his blaster to deal with dretches safely unless blaster is unlimited ammo
#define MDRIVER_K_SCALE             3.0f
#define MDRIVER_RELOAD              4000

//Chaingun actually fires faster
#define CHAINGUN_PRICE              400
#define CHAINGUN_BULLETS            300
#define CHAINGUN_MAXCLIPS           2
#define CHAINGUN_REPEAT             90 //without speedy barrel function
#define CHAINGUN_REPEAT2             70 //with speedy barrel function
#define CHAINGUN_K_SCALE            1.0f
#define CHAINGUN_SPREAD             500 //700
#define CHAINGUN_DMG                HDM(7) //6

//Prifle - slow spam, doing max 112 dmg/s

#define PRIFLE_PRICE                580 //cost much? Its like a really strong lasgun in my opinion.
#define PRIFLE_CLIPS                58
#define PRIFLE_MAXCLIPS             5
#define PRIFLE_REPEAT               125
#define PRIFLE_K_SCALE              1.5f //overpowered againts adv marauder (if the pulse is a bot or a good human player)
#define PRIFLE_RELOAD               2000
#define PRIFLE_DMG                  HDM(14)
#define PRIFLE_SPEED                3000 //1000
//#define PRIFLE_SPREAD               300 //cancelled - usage deleted

//pulse rifle from movie 'alien'
/*#define PRIFLE_PRICE                480
#define PRIFLE_CLIPS                70
#define PRIFLE_MAXCLIPS             0
#define PRIFLE_REPEAT               136
#define PRIFLE_K_SCALE              1.0f
#define PRIFLE_RELOAD               2000
#define PRIFLE_DMG                  HDM(38)
#define PRIFLE_SPEED                6000
*/

//Flamer has a realistic shot by halving the repeat rate (2x speed) and halving the dmg
#define FLAMER_PRICE                520
#define FLAMER_GAS                  380 //150
#define FLAMER_REPEAT               100 //200
#define FLAMER_K_SCALE              1.0f //1f
#define FLAMER_DMG                  HDM(11) //20
#define FLAMER_RADIUS               80 //splash radius //50
#define FLAMER_LIFETIME             800.0f
#define FLAMER_SPEED                360.0f //300
#define FLAMER_LAG                  0.65f  //the amount of player velocity that is added to the fireball

//Easier to get dretches, and lucijump. Also improved to match tyrant's and other alien's new health.
#define LCANNON_PRICE               700
#define LCANNON_AMMO                200
#define LCANNON_REPEAT              1250 //500 //400
#define LCANNON_K_SCALE             3.27f//1f
#define LCANNON_CHARGEREPEAT        500 //800
#define LCANNON_RELOAD              5000
#define LCANNON_DAMAGE              HDM(372) //265 //395
#define LCANNON_RADIUS              120 //splash radius. Default 150, wanted 180, but too overpowered
#define LCANNON_SECONDARY_DAMAGE    HDM(132) //default 27, prefered 30, but gets overpowered if rapid //now direct hit
#define LCANNON_SECONDARY_RADIUS    55 //75
#define LCANNON_SPEED               400 //330 //Noob Tube! //780 //changed back, see g_missile.c for reason.
/*OR you can just read this: It is porportional to how much you power up.
 *What you put here is the MINIMUM speed, and max speed is roughly... uhh lets say around 2.6x this value.
*/

#define LCANNON_CHARGE_TIME         2800 //2000
#define LCANNON_TOTAL_CHARGE        355 //255
#define LCANNON_MIN_CHARGE          36 //50
#define LCANNON_MAXCLIPS	    1

#define HBUILD_PRICE                0
#define HBUILD_REPEAT               1000 //1000
#define HBUILD_DELAY                17500 //17500
#define HBUILD_HEALRATE             28 //18

#define HBUILD2_PRICE               0
#define HBUILD2_REPEAT              800 //1000
#define HBUILD2_DELAY               10000 //15000



/*
 * HUMAN upgrades
 */

#define LIGHTARMOUR_PRICE           70
#define LIGHTARMOUR_POISON_PROTECTION 2

#define HELMET_PRICE                130 //helm + larmour = 200 = 1 evo
#define HELMET_RANGE                1000.0f //1000.0f
#define HELMET_POISON_PROTECTION    2 

//medkit price ?!?
#define MEDKIT_PRICE                0

#define BATTPACK_PRICE              100
#define BATTPACK_MODIFIER           1.6f //modifier for extra energy storage available 1.5f

#define JETPACK_PRICE               120
#define JETPACK_FLOAT_SPEED         228.0f //up movement speed //128
#define JETPACK_SINK_SPEED          292.0f //down movement speed //192
#define JETPACK_DISABLE_TIME        1600 //time to disable the jetpack when player damaged
#define JETPACK_DISABLE_CHANCE      0.3f //? - never worked

#define BSUIT_PRICE                 500
#define BSUIT_POISON_PROTECTION     5
//#define HUMAN_REGEN                 (client->ps.stats[ STAT_MAX_HEALTH ] / 25) //human regeneration. Once every 10 seconds. //Fuck it. I need to use a variable that is declared somewhere else. I can't be bothered directing the file.

//physical ammo prices, all default 0
#define MGCLIP_PRICE                0

#define CGAMMO_PRICE                0

#define GAS_PRICE                   0

#define MEDKIT_POISON_IMMUNITY_TIME 30000 //default 0
#define MEDKIT_STARTUP_TIME         12000 //4000
#define MEDKIT_STARTUP_SPEED        15 //5


/*
 * HUMAN buildables
 *
 * _BP            - build points required for this buildable
 * _BT            - build time required for this buildable
 * _SPLASHDAMGE   - the amount of damage caused by this buildable when it blows up
 * _SPLASHRADIUS  - the radius around which it does this damage
 *
 * REACTOR_BASESIZE - the maximum distance a buildable can be from an reactor
 * REPEATER_BASESIZE - the maximum distance a buildable can be from a repeaterabout:startpage
 * HUMAN_BHLTH_MODIFIER - overall health modifier for coarse tuning
 *
 */

#define HUMAN_BHLTH_MODIFIER        1.0f
#define HBHM(h)                     ((int)((float)h*HUMAN_BHLTH_MODIFIER))

#define REACTOR_BASESIZE            1200 //1000
#define REPEATER_BASESIZE           600 //500
#define HUMAN_DETONATION_DELAY      3000

#define HSPAWN_BP                   10
#define HSPAWN_BT                   10000
#define HSPAWN_HEALTH               HBHM(310) //210
#define HSPAWN_SPLASHDAMAGE         120 //60
#define HSPAWN_SPLASHRADIUS         100
#define HSPAWN_VALUE                1

#define MEDISTAT_BP                 6
#define MEDISTAT_BT                 8000 //default 10000
#define MEDISTAT_HEALTH             HBHM(190) //190
#define MEDISTAT_SPLASHDAMAGE       50
#define MEDISTAT_SPLASHRADIUS       100


//NOTE: MASSIVE CHANGE TO THE MACHINE GUN TURRET!
//no spinup required!
//IMPORTANT: You should change MGTURRET_FIRE_SPEED to 1 if you want the turret to be 'normal'.
//It slows down the turning speed of the turret when it fires.
#define MGTURRET_FIRE_SPEED         0.5
// IF it is directly aiming at it, the following speed multiplier will be used instead.
//It gives a more realistic feel.
#define MGTURRET_FIRE_DIRECT_SPEED  0.2

#define MGTURRET_BP                 8
#define MGTURRET_BT                 10000
#define MGTURRET_HEALTH             HBHM(210) //default 190. //increased to cope with tyrant's charge and pounce
#define MGTURRET_SPLASHDAMAGE       60 //50
#define MGTURRET_SPLASHRADIUS       100
#define MGTURRET_ANGULARSPEED       5  //degrees/think 8 ~= 200deg/sec //3 //now 5 because of added MGTURRET_FIRE_SPEED
#define MGTURRET_ACCURACYTOLERANCE  MGTURRET_ANGULARSPEED / 1.5f //1.5 angular difference for turret to fire
#define MGTURRET_VERTICALCAP        30  // +/- maximum pitch //45 default, measured in degrees of depression (google it)
#define MGTURRET_REPEAT             80 //100
#define MGTURRET_K_SCALE            1.0f
#define MGTURRET_RANGE              580.0f //default 300, same goes for atube
#define MGTURRET_SPREAD             1200 //200
#define MGTURRET_DMG                HDM(6) //4
#define MGTURRET_DCC_ANGULARSPEED       13
#define MGTURRET_DCC_ACCURACYTOLERANCE  MGTURRET_DCC_ANGULARSPEED / 1.5f //1.5f
#define MGTURRET_GRAB_ANGULARSPEED      3
#define MGTURRET_GRAB_ACCURACYTOLERANCE MGTURRET_GRAB_ANGULARSPEED / 1.5f
//sort of a 'proper' version of accuracy tolerance as the pervious one screws up the turret if you raise it.
//This 'range/spread' is a multiplier for the accuracy tolerance before it fires.
#define MGTURRET_ACCURACY_SPREAD        8
//The same thing, but the 'smaller' version of it.
#define MGTURRET_DIRECT_ACCURACY_SPREAD 3
//this is a mutliplier for the ratio of how fast the pitch goes compare to normal. "Normal" is the same value as for yaw. Yaw will stay as MGTURRET_ANGULARSPEED or whatever if it is grabbed. For example, normal turning speed is 8 without grab or dcc, therefore:
//*1 = normal = 8
//*0.5 = half = 4
//*2 = Fast rise = 16
//*0 = no rise
#define MGTURRET_ACCURACY_PITCH         0.8


#define TESLAGEN_BP                 10
#define TESLAGEN_BT                 15000
#define TESLAGEN_HEALTH             HBHM(280) //210
#define TESLAGEN_SPLASHDAMAGE       60 //default 50
#define TESLAGEN_SPLASHRADIUS       100
#define TESLAGEN_REPEAT             100 //default 250
#define TESLAGEN_K_SCALE            4.0f //4f
#define TESLAGEN_RANGE              300 //default 250
#define TESLAGEN_DMG                HDM(7) //default 9

#define DC_BP                       8
#define DC_BT                       10000
#define DC_HEALTH                   HBHM(220) //190
#define DC_SPLASHDAMAGE             100 //50
#define DC_SPLASHRADIUS             120 //100

#define ARMOURY_BP                  10
#define ARMOURY_BT                  17500
#define ARMOURY_HEALTH              HBHM(580) //380
#define ARMOURY_SPLASHDAMAGE        80 //50
#define ARMOURY_SPLASHRADIUS        200 //100

//Tesla Reactor! =P
#define REACTOR_BP                  0
#define REACTOR_BT                  20000 //default 20000
#define REACTOR_HEALTH              HBHM(970) //930
#define REACTOR_SPLASHDAMAGE        380 //200/100
#define REACTOR_SPLASHRADIUS        780
#define REACTOR_ATTACK_RANGE        180.0f //120
#define REACTOR_ATTACK_REPEAT       125 //1000
#define REACTOR_ATTACK_DAMAGE       6 //30 //8 is too strong and can't be hopped
#define REACTOR_VALUE               4

//nuclear repeater
#define REPEATER_BP                 3
#define REPEATER_BT                 10000
#define REPEATER_HEALTH             HBHM(350) //250
#define REPEATER_SPLASHDAMAGE       250 //50
#define REPEATER_SPLASHRADIUS       400 // 100
#define REPEATER_INACTIVE_TIME      300000 //90 seconds default. Give people time to build rc.

/*
 * HUMAN misc
 * Usually this: (in order) - 1.2f  1.0f  0.8f  0.9f | 25  15  8  4
 */

#define HUMAN_SPRINT_MODIFIER       1.3f //sprint multiplyer //1.2
#define HUMAN_JOG_MODIFIER          1.0f // normal human speed
#define HUMAN_BACK_MODIFIER         0.8f //to stop back pedaling
#define HUMAN_SIDE_MODIFIER         0.9f // dodging/straifing sideways

#define STAMINA_STOP_RESTORE        35 //restore stamina by stopping
#define STAMINA_WALK_RESTORE        18 //or walking
#define STAMINA_SPRINT_TAKE         9 //taking stamina away by sprinting //8
#define STAMINA_LARMOUR_TAKE        12 //It replaces sprint_take when having larmour. According to g_active.c

#define HUMAN_SPAWN_REPEAT_TIME     6000 //spawn timing,default 8000

/*
 * Misc
 */

#define MIN_FALL_DISTANCE           30.0f //the fall distance at which fall damage kicks in //20f //18f
#define MAX_FALL_DISTANCE           720.0f //the fall distance at which maximum damage is dealt //240f //change higher to 360 //changed higher to 720 because more damage is delt at maximum damage
#define AVG_FALL_DISTANCE           ((MIN_FALL_DISTANCE+MAX_FALL_DISTANCE)/2.0f)

#define HUMAN_MAXED                 900   //a human with a strong selection of weapons/upgrades // << i dont get this
#define HUMAN_MAX_CREDITS           3000 //max creds, usually 2000
#define ALIEN_MAX_KILLS             13 //max evos, usually 9
#define ALIEN_MAX_SINGLE_KILLS      4 //uh... probably for G_alienstage?threshold?

#define FREEKILL_PERIOD             60000 //msec // What i think it is the free credits recieved after a amount of time. //120000
#define FREEKILL_ALIEN              1 // one evo per 120000 miliseconds or whatever FREEKILL_PERIOD is.
#define FREEKILL_HUMAN              220 // LEVEL0_VALUE is default. This is enough for lasgun and larmour.

//thresholds usually 20, then 40, bp is 100 both, but usually prefered 150 for aliens.
#define DEFAULT_ALIEN_BUILDPOINTS   "230"
#define DEFAULT_ALIEN_STAGE2_THRESH "30"
#define DEFAULT_ALIEN_STAGE3_THRESH "90"
#define DEFAULT_ALIEN_MAX_STAGE     "2"
#define DEFAULT_HUMAN_BUILDPOINTS   "170"
#define DEFAULT_HUMAN_STAGE2_THRESH "30"
#define DEFAULT_HUMAN_STAGE3_THRESH "90"
#define DEFAULT_HUMAN_MAX_STAGE     "2"

#define DAMAGE_FRACTION_FOR_KILL    0.5f //how much damage players (versus structures) need to
                                         //do to increment the stage kill counters. Usually 0.5. I had 0.9 but that made humans come up slowly.

//Vampire mode settings
//See the rest in g_combat.c, line 1762
#define VAMP_EXTRA                  50

// g_suddenDeathMode settings
#define SDMODE_BP                   29 // This is why gpp sudden death sucks, you can't build an armoury. Here, you can build a maximum of one armoury,a reactor and medi, dcc and repeater. 
#define SDMODE_NO_BUILD             0 // 1 for totally out
#define SDMODE_SELECTIVE            0 //2
