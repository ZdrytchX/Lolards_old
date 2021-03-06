/*
   ===========================================================================
   Copyright (C) 2007 Amine Haddad

   This file is part of Tremulous.

   The original works of vcxzet (lamebot3) were used a guide to create TremBot.
   
   The works of Amine (TremBot) and Sex (PBot) where used/modified to create COW Bot

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

#include "g_local.h"
#include "g_bot.h"

#ifndef RAND_MAX
#define RAND_MAX 32768
#endif

//uncomment to enable debuging messages
//#define BOT_DEBUG 1

botMemory_t g_botMind[MAX_CLIENTS];

void G_BotAdd( char *name, int team, int skill ) {
    int i;
    int clientNum;
    char userinfo[MAX_INFO_STRING];
    int reservedSlots = 0;
    gentity_t *bot;
    //char buffer [33];
    reservedSlots = trap_Cvar_VariableIntegerValue( "sv_privateclients" );

    // find what clientNum to use for bot
    clientNum = -1;
    for( i = 0; i < reservedSlots; i++ ) {
        if( !g_entities[i].inuse ) {
            clientNum = i;
            break;
        }
    }

    if(clientNum < 0) {
        trap_Printf("no more slots for bot\n");
        return;
    }
    bot = &g_entities[ clientNum ];
    bot->r.svFlags |= SVF_BOT;
    bot->inuse = qtrue;


    //default bot data
    bot->botMind = &g_botMind[clientNum];
    bot->botMind->enemyLastSeen = 0;
    bot->botMind->command = BOT_AUTO;
    bot->botMind->botTeam = team;
    bot->botMind->spawnItem = WP_HBUILD;
    bot->botMind->state = FINDNEWNODE;
    bot->botMind->timeFoundEnemy = 0;
    bot->botMind->followingRoute = qfalse;
    
    setSkill(bot, skill);

    

    // register user information
    userinfo[0] = '\0';
    Info_SetValueForKey( userinfo, "name", name );
    Info_SetValueForKey( userinfo, "rate", "25000" );
    Info_SetValueForKey( userinfo, "snaps", "20" );
    
    //so we can connect if server is password protected
    if(g_needpass.integer == 1)
      Info_SetValueForKey( userinfo, "password", g_password.string);
    
    trap_SetUserinfo( clientNum, userinfo );

    // have it connect to the game as a normal client
    if(ClientConnect(clientNum, qtrue) != NULL )
        // won't let us join
        return;

    ClientBegin( clientNum );
    G_ChangeTeam( bot, team );
}

void G_BotDel( int clientNum ) {
    gentity_t *bot;

    bot = &g_entities[clientNum];
    if( !( bot->r.svFlags & SVF_BOT ) ) {
        trap_Printf( va("'^7%s^7' is not a bot\n", bot->client->pers.netname) );
        return;
    }

    ClientDisconnect(clientNum);
    //trap_BotFreeClient(clientNum);
}

void G_BotCmd( gentity_t *master, int clientNum, char *command) {
    gentity_t *bot;

    bot = &g_entities[clientNum];
    if( !( bot->r.svFlags & SVF_BOT ) )
        return;

    if( !Q_stricmp( command, "auto" ) )
        bot->botMind->command = BOT_AUTO;
    else if( !Q_stricmp( command, "attack" ) )
        bot->botMind->command = BOT_ATTACK;
    else if( !Q_stricmp( command, "idle" ) )
        bot->botMind->command = BOT_IDLE;
    else if( !Q_stricmp( command, "repair" ) ) {
        if(BG_InventoryContainsWeapon(WP_HBUILD, bot->client->ps.stats)) {
            bot->botMind->command = BOT_REPAIR;
            G_ForceWeaponChange( bot, WP_HBUILD );
        }
    } else if( !Q_stricmp( command, "spawnrifle" ) )
        bot->botMind->spawnItem = WP_MACHINEGUN;
    else if( !Q_stricmp( command, "spawnckit" ) )
        bot->botMind->spawnItem = WP_HBUILD;
    else {
        bot->botMind->command = BOT_AUTO;
    }
    return;
}

qboolean botShouldJump(gentity_t *self) {
    trace_t trace;
    gentity_t *traceEnt;
    vec3_t mins,maxs;
    vec3_t end;
    vec3_t forward,right,up;
    vec3_t muzzle;
    
    
    BG_FindBBoxForClass(self->client->ps.stats[STAT_PCLASS], mins, maxs, NULL, NULL, NULL);
    
    AngleVectors(self->client->ps.viewangles,forward,right,up);
    CalcMuzzlePoint(self,forward,right,up,muzzle);
    VectorMA(muzzle, 10, forward, end);
    
    trap_Trace(&trace, self->s.origin, mins, maxs, end, self->s.number,MASK_SHOT);
    
    traceEnt = &g_entities[trace.entityNum];
    
    if(traceEnt->s.eType == ET_BUILDABLE)
        return qtrue;
    else
        return qfalse;

}
int getStrafeDirection(gentity_t *self) {
    
    trace_t traceRight,traceLeft;
    vec3_t traceHeight;
    vec3_t startRight,startLeft;
    vec3_t mins,maxs;
    vec3_t forward,right;
    vec3_t endRight,endLeft;
    
    int strafe;
    BG_FindBBoxForClass( self->client->ps.stats[STAT_PCLASS], mins, maxs, NULL, NULL, NULL);
    AngleVectors( self->client->ps.viewangles,forward , right, NULL);
    
    
    VectorScale(right, maxs[1], right);
    
    VectorAdd( self->s.origin, right, startRight );
    VectorSubtract( self->s.origin, right, startLeft );
    
    forward[2] = 0.0f;
    VectorMA(startRight, maxs[0] + 30, forward, endRight);
    VectorMA(startLeft, maxs[0] + 30, forward, endLeft);
    
    startRight[2] += mins[2];
    startLeft[2] += mins[2];
    
    VectorSet(traceHeight, 0.0f, 1.0f, (maxs[2] - mins[2]));
    
    trap_Trace( &traceRight, startRight, NULL, traceHeight, endRight, self->s.number, MASK_SHOT);
    trap_Trace( &traceLeft, startLeft, NULL, traceHeight, endLeft, self->s.number, MASK_SHOT);
    
    if( traceRight.fraction == 1.0f && traceLeft.fraction != 1.0f ) {
        strafe = 127;
    } else if( traceRight.fraction != 1.0f && traceLeft.fraction == 1.0f ) {
        strafe = -127;
        
        //we dont know which direction to strafe, so strafe randomly
    } else {
        if((self->client->time10000 % 5000) > 2500)
            strafe = 127;
        else
            strafe = -127;
    }
        
    
    return strafe;
}
qboolean botPathIsBlocked(gentity_t *self) {
    vec3_t forward,start, end;
    vec3_t mins, maxs;
    trace_t trace;
    gentity_t *traceEnt;
    int blockerTeam;
    
    if( !self->client )
        return qfalse;
    
    BG_FindBBoxForClass( self->client->ps.stats[ STAT_PCLASS ], mins, maxs, NULL, NULL, NULL );
    
    AngleVectors( self->client->ps.viewangles, forward, NULL, NULL);
    forward[ 2 ] = 0.0f; //make vector 2D by getting rid of z component

    //scaling the vector
    VectorMA( self->client->ps.origin, maxs[0], forward, start );
    VectorMA(start, 30, forward,end);
    
    trap_Trace( &trace, self->client->ps.origin, mins, maxs, end, self->client->ps.clientNum, MASK_PLAYERSOLID );
    traceEnt = &g_entities[trace.entityNum];
    if(traceEnt) {
        if(traceEnt->s.eType == ET_BUILDABLE)
            blockerTeam = traceEnt->biteam;
        else if(traceEnt->client)
            blockerTeam = traceEnt->client->ps.stats[STAT_PTEAM];
        else
            blockerTeam = PTE_NONE;
    }
    if( trace.fraction == 1.0f || trace.entityNum == ENTITYNUM_WORLD || blockerTeam != self->client->ps.stats[STAT_PTEAM] )//hitting nothing? (world doesnt count)
            return qfalse;
    else
        return qtrue;
}
/**G_BotThink
 * Does Misc bot actions
 * Calls G_BotModusManager to decide which Modus the bot should be in
 * Executes the different bot functions based on the bot's mode
*/
void G_BotThink( gentity_t *self) {
    usercmd_t  botCmdBuffer = self->client->pers.cmd;
    botCmdBuffer.buttons = 0;
    botCmdBuffer.forwardmove = 0;
    botCmdBuffer.rightmove = 0;
    
    //use medkit when hp is low
    if(self->health < BOT_USEMEDKIT_HP && BG_InventoryContainsUpgrade(UP_MEDKIT,self->client->ps.stats))
        BG_ActivateUpgrade(UP_MEDKIT,self->client->ps.stats);
    
    //try to evolve every so often (aliens only)
    if(g_bot_evolve.integer > 0 && self->client->ps.stats[STAT_PTEAM] == PTE_ALIENS && self->client->ps.persistant[PERS_CREDIT] > 0)
        G_BotEvolve(self,&botCmdBuffer);
    
    //infinite funds cvar
    if(g_bot_infinite_funds.integer == 1)
        G_AddCreditToClient(self->client, HUMAN_MAX_CREDITS, qtrue);

    //hacky ping fix
    self->client->ps.ping = rand() % 100 + 100; //not really a ping fix, but it gives a delay for reaction. Handy when you're a basi.
    
    G_BotModusManager(self);
    switch(self->botMind->currentModus) {
        case ATTACK:
            G_BotAttack(self, &botCmdBuffer);
            break;
        case BUILD:
           // G_BotBuild(self, &botCmdBuffer);
           break;
        case BUY:
            G_BotBuy(self, &botCmdBuffer);
            break;
        case HEAL:
            G_BotHeal(self, &botCmdBuffer);
            break;
        case REPAIR:
            G_BotRepair(self, &botCmdBuffer);
            break;
        case ROAM:
            G_BotRoam(self, &botCmdBuffer);
            break;
        case IDLE:
            break;
    }
    self->client->pers.cmd =botCmdBuffer;
}
/**G_BotModusManager
 * Changes the bot's current Modus based on the surrounding conditions
 * Decides when a bot will Build,Attack,Roam,etc
 */
void G_BotModusManager( gentity_t *self ) {
    
    int enemyIndex = ENTITYNUM_NONE;
    
    int damagedBuildingIndex = botFindDamagedFriendlyStructure(self);
    int medistatIndex = botFindBuilding(self, BA_H_MEDISTAT, BOT_MEDI_RANGE);
    int armouryIndex = botFindBuilding(self, BA_H_ARMOURY, BOT_ARM_RANGE);
    
    //search for a new enemy every so often
   if(self->client->time10000 % BOT_ENEMYSEARCH_INTERVAL == 0) 
        enemyIndex = botFindClosestEnemy(self, qfalse);
    
    //if we are in attackmode, we have an enemy, continue chasing him for a while even if he goes out of sight/range unless a new enemy is closer
    if(level.time - self->botMind->enemyLastSeen < BOT_ENEMY_CHASETIME && self->botMind->currentModus == ATTACK && g_entities[getTargetEntityNumber(self->botMind->goal)].health > 0 && enemyIndex == ENTITYNUM_NONE)
        enemyIndex = getTargetEntityNumber(self->botMind->goal);
    
    //dont do anything if given the idle command
    if(self->botMind->command == BOT_IDLE) {
        self->botMind->currentModus = IDLE;
        return;
    }
   
    
    if(enemyIndex != ENTITYNUM_NONE && self->botMind->command != BOT_REPAIR) {
        self->botMind->currentModus = ATTACK;
        if(enemyIndex != getTargetEntityNumber(self->botMind->goal)) {
            setGoalEntity(self, &g_entities[enemyIndex]);
        }
        self->botMind->state = FINDNEWNODE;
    } else if(damagedBuildingIndex != ENTITYNUM_NONE && BG_InventoryContainsWeapon(WP_HBUILD,self->client->ps.stats) && self->botMind->command != BOT_ATTACK) {
        self->botMind->currentModus = REPAIR;
        if(damagedBuildingIndex != getTargetEntityNumber(self->botMind->goal)) {
            setGoalEntity(self, &g_entities[damagedBuildingIndex]);
        }
        self->botMind->state = FINDNEWNODE;
    } else if(medistatIndex != ENTITYNUM_NONE && self->health < BOT_LOW_HP && !BG_InventoryContainsUpgrade(UP_MEDKIT, self->client->ps.stats) 
    && self->client->ps.stats[STAT_PTEAM] == PTE_HUMANS && self->botMind->command != BOT_REPAIR) {
        self->botMind->currentModus = HEAL;
        if(medistatIndex != getTargetEntityNumber(self->botMind->goal)) {
            setGoalEntity(self, &g_entities[medistatIndex]);
        }
        self->botMind->state = FINDNEWNODE;
    } else if(armouryIndex != ENTITYNUM_NONE && botNeedsItem(self) && g_bot_buy.integer > 0 && self->client->ps.stats[STAT_PTEAM] == PTE_HUMANS 
    && self->botMind->command != BOT_REPAIR) {
        self->botMind->currentModus = BUY;
        if(armouryIndex != getTargetEntityNumber(self->botMind->goal)) {
            setGoalEntity(self, &g_entities[armouryIndex]);
        }
        self->botMind->state = FINDNEWNODE;
    } else if(g_bot_roam.integer > 0 && self->botMind->command != BOT_REPAIR){
        self->botMind->currentModus = ROAM;
    }
    
}
/**G_BotMoveDirectlyToGoal
*Used whenever we need to go directly to the goal
*Uses the route to get to the closest node to the goal
*Then goes from the last node to the goal if the goal is seen
*If the goal is not seen, then it computes a new route to the goal and follows it
*/
void G_BotMoveDirectlyToGoal( gentity_t *self, usercmd_t *botCmdBuffer ) {
    
    vec3_t targetPos;
    getTargetPos(self->botMind->goal, &targetPos);
    if(self->botMind->followingRoute && self->botMind->targetNodeID != -1) {
        setTargetCoordinate(&self->botMind->targetNode, level.nodes[self->botMind->targetNodeID].coord);
        G_BotGoto( self, self->botMind->targetNode, botCmdBuffer );
        doLastNodeAction(self, botCmdBuffer);
        
        //apparently, we are stuck, so find a new route to the goal and use that
        if(level.time - self->botMind->timeFoundNode > level.nodes[self->botMind->lastNodeID].timeout)
        {
            findRouteToTarget(self, self->botMind->goal);
            setNewRoute(self);
        }
        
        else if( level.time - self->botMind->timeFoundNode > 10000 )
        {
            findRouteToTarget(self, self->botMind->goal);
            setNewRoute(self);
        }
        if(distanceToTargetNode(self) < 70)
        {
            self->botMind->lastNodeID = self->botMind->targetNodeID;
            self->botMind->targetNodeID = self->botMind->routeToTarget[self->botMind->targetNodeID];
            self->botMind->timeFoundNode = level.time;
            
        }
        //our route has ended or we have been told to quit following it 
        //can we see the goal? Then go directly to it. Else find a new route and folow it
    } else if(botTargetInRange(self, self->botMind->goal, MASK_DEADSOLID)){
        G_BotGoto(self,self->botMind->goal,botCmdBuffer);
    } else {
        findRouteToTarget(self, self->botMind->goal);
        setNewRoute(self);
    }
    
}


//using PBot Code for now..
void G_BotSearchForGoal(gentity_t *self, usercmd_t *botCmdBuffer) {
    switch(self->botMind->state) {
        case FINDNEWNODE: findNewNode(self, botCmdBuffer); break;
        case FINDNEXTNODE: findNextNode(self); break;
        case TARGETNODE:break; //basically used as a flag that is checked elsewhere
        case LOST: findNewNode(self, botCmdBuffer);break; //This should never happen unless there are 0 waypoints on the map
        case TARGETOBJECTIVE: break;
        default: break;
    }
    if(self->botMind->state == TARGETNODE) {
        #ifdef BOT_DEBUG
        trap_SendServerCommand(-1,va("print \"Now Targeting Node %d\n\"", self->botMind->targetNode));
        #endif
        setTargetCoordinate(&self->botMind->targetNode, level.nodes[self->botMind->targetNodeID].coord);
        G_BotGoto(self, self->botMind->targetNode, botCmdBuffer);
        
        if(self->botMind->lastNodeID >= 0 ) {
            doLastNodeAction(self, botCmdBuffer);
            if(level.time - self->botMind->timeFoundNode > level.nodes[self->botMind->lastNodeID].timeout) {
                self->botMind->state = FINDNEWNODE;
                self->botMind->timeFoundNode = level.time;
            }
        }
        else if( level.time - self->botMind->timeFoundNode > 10000 ) {
            self->botMind->state = FINDNEWNODE;
            self->botMind->timeFoundNode = level.time;
        }
        if(distanceToTargetNode(self) < 70) {
            self->botMind->state = FINDNEXTNODE;
            self->botMind->timeFoundNode = level.time;
        }
    }
}

/**G_BotGoto
 * Used to make the bot travel between waypoints or to the target from the last waypoint
 * Also can be used to make the bot travel other short distances
*/
void G_BotGoto(gentity_t *self, botTarget_t target, usercmd_t *botCmdBuffer) {
    
    vec3_t tmpVec;
    
    //aim at the destination
    botGetAimLocation(target, &tmpVec);
    
    if(!targetIsEntity(target))
        botSlowAim(self, tmpVec, 0.5f, &tmpVec);
    else
        botSlowAim(self, tmpVec, self->botMind->botSkill.aimSlowness, &tmpVec);
    
    if(getTargetType(target) != ET_BUILDABLE && targetIsEntity(target)) {
        botShakeAim(self, &tmpVec);
    }
    
        
    botAimAtLocation(self, tmpVec, botCmdBuffer);
    
    //humans should not move if they are targetting, and can hit, a building
    if(botTargetInAttackRange(self, target) && getTargetType(target) == ET_BUILDABLE && self->client->ps.stats[STAT_PTEAM] == PTE_HUMANS && getTargetTeam(target) == PTE_ALIENS)
        return;
    
    //move forward
    botCmdBuffer->forwardmove = 127;
    
    //dodge if going toward enemy
    if(self->client->ps.stats[STAT_PTEAM] != getTargetTeam(target) && getTargetTeam(target) != PTE_NONE) {
        G_BotDodge(self, botCmdBuffer);
    }
    
    //this is here so we dont run out of stamina..
    //basically, just me being too lazy to make bots stop and regain stamina
    self->client->ps.stats[ STAT_STAMINA ] = MAX_STAMINA;
    
    //we have stopped moving forward, try to get around whatever is blocking us
    if( botPathIsBlocked(self) ) {
        botCmdBuffer->rightmove = getStrafeDirection(self);
        if(botShouldJump(self))
            botCmdBuffer->upmove = 127;
        //dont move forward as quickly to allow use to strafe correctly
        botCmdBuffer->forwardmove = 30;
    }
    
    //need to periodically reset upmove to 0 for jump to work
    if( self->client->time10000 % 1000)
        botCmdBuffer->upmove = 0;
    
    // enable wallwalk for classes that can wallwalk
    if( BG_ClassHasAbility( self->client->ps.stats[STAT_PCLASS], SCA_WALLCLIMBER ) )
        botCmdBuffer->upmove = -1;
    
    //stay away from enemy as human
        getTargetPos(target, &tmpVec);
        if(self->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS && 
        DistanceSquared(self->s.pos.trBase,tmpVec) < Square(350) && botTargetInAttackRange(self, target) && self->s.weapon != WP_PAIN_SAW
        && getTargetTeam(target) == PTE_ALIENS)
        {
            botCmdBuffer->forwardmove = -100;
        }
    
}
/**
 * G_BotAttack
 * The AI for attacking an enemy
 * Is called in G_BotThink
 * Decided when to be called in G_BotModusManager
 */
void G_BotAttack(gentity_t *self, usercmd_t *botCmdBuffer) {
    
    //switch to blaster
	//stopped ckit - blaster switching remove the "self->client->ps.weapon == WP_HBUILD"
    if((BG_WeaponIsEmpty(self->client->ps.weapon, self->client->ps.ammo, self->client->ps.powerups)
        || self->client->ps.weapon == WP_HBUILD) && self->client->ps.stats[STAT_PTEAM] == PTE_HUMANS)
        G_ForceWeaponChange( self, WP_BLASTER );
    
    //aliens have radar so they will always 'see' the enemy if they are in radar range
    if(self->client->ps.stats[STAT_PTEAM] == PTE_ALIENS) {
        self->botMind->enemyLastSeen = level.time;
    }
    if(botTargetInAttackRange(self, self->botMind->goal)) {
        
        self->botMind->followingRoute = qfalse;
        G_BotMoveDirectlyToGoal(self, botCmdBuffer);
        botFireWeapon(self, botCmdBuffer);
        self->botMind->enemyLastSeen = level.time;
    } else if(botTargetInRange(self, self->botMind->goal, MASK_SHOT)) {
        G_BotMoveDirectlyToGoal(self, botCmdBuffer);
        G_BotReactToEnemy(self, botCmdBuffer);
        self->botMind->enemyLastSeen = level.time;
    } else if(!self->botMind->followingRoute || self->botMind->targetNodeID == -1){
        findRouteToTarget(self, self->botMind->goal);
        setNewRoute(self);
        G_BotMoveDirectlyToGoal(self, botCmdBuffer);
    } else {
        G_BotMoveDirectlyToGoal(self, botCmdBuffer);
    }
    
    
}
/**
 * G_BotRepair
 * The AI for repairing a structure
 * Is called in G_BotThink
 * Decided when to be called in G_BotModusManager
 */
void G_BotRepair(gentity_t *self, usercmd_t *botCmdBuffer) {
    if(self->client->ps.weapon != WP_HBUILD)
        G_ForceWeaponChange( self, WP_HBUILD );
    if(botTargetInAttackRange(self, self->botMind->goal) && botGetAimEntityNumber(self) == getTargetEntityNumber(self->botMind->goal) ) {
        self->botMind->followingRoute = qfalse;
        botFireWeapon( self, botCmdBuffer );
    } else
        G_BotMoveDirectlyToGoal(self, botCmdBuffer);
}
/**
 * G_BotHeal
 * The AI for making the bot go to a medistation to heal
 * Is called in G_BotThink
 * Decided when to be called in G_BotModusManager
 */
void G_BotHeal(gentity_t *self, usercmd_t *botCmdBuffer) {
    
    vec3_t targetPos;
    getTargetPos(self->botMind->goal, &targetPos);
    if(DistanceSquared(self->s.origin, targetPos) > Square(50))
        G_BotMoveDirectlyToGoal(self, botCmdBuffer);
    
}
/**
 * G_BotBuy
 * The AI for making the bot go to an armoury and buying something
 * Is called in G_BotThink
 * Decided when to be called in G_BotModusManager
 */
void G_BotBuy(gentity_t *self, usercmd_t *botCmdBuffer) {
    vec3_t targetPos;
    int i;
    getTargetPos(self->botMind->goal, &targetPos);
    if(DistanceSquared(self->s.pos.trBase, targetPos) > Square(100))
        G_BotMoveDirectlyToGoal(self, botCmdBuffer);
    else {
        // sell current weapon
        for( i = WP_NONE + 1; i < WP_NUM_WEAPONS; i++ )
        {
            if( BG_InventoryContainsWeapon( i, self->client->ps.stats ) &&
                BG_FindPurchasableForWeapon( i ) )
            {
                BG_RemoveWeaponFromInventory( i, self->client->ps.stats );
                
                //add to funds
                G_AddCreditToClient( self->client, (short)BG_FindPriceForWeapon( i ), qfalse );
            }
            
            //if we have this weapon selected, force a new selection
            if( i == self->client->ps.weapon )
                G_ForceWeaponChange( self, WP_NONE );
        }
        //try to buy helmet/lightarmour /not bsuit because humans glitch
        G_BotBuyUpgrade( self, UP_HELMET);
        G_BotBuyUpgrade( self, UP_LIGHTARMOUR);
        
        // buy most expensive first, then one cheaper, etc, dirty but working way
      if( !G_BotBuyWeapon( self, WP_LOCKBLOB_LAUNCHER ) )
        if( !G_BotBuyWeapon( self, WP_LUCIFER_CANNON ) )
            if( !G_BotBuyWeapon( self, WP_PULSE_RIFLE ) )
                if( !G_BotBuyWeapon( self, WP_FLAMER ) )
                    if( !G_BotBuyWeapon( self, WP_MASS_DRIVER ) )
                        if( !G_BotBuyWeapon( self, WP_CHAINGUN ) )
                            if( !G_BotBuyWeapon( self, WP_SHOTGUN ) )
                                if( !G_BotBuyWeapon( self, WP_LAS_GUN ) )
                                    if( !G_BotBuyWeapon( self, WP_PAIN_SAW ) )
                                        G_BotBuyWeapon( self, WP_MACHINEGUN );
                                    
        //buy ammo/batpack
        if( BG_FindUsesEnergyForWeapon( self->client->ps.weapon )) {
            G_BotBuyUpgrade( self, UP_BATTPACK );
        }else {
            G_BotBuyUpgrade( self, UP_AMMO );
        }
    }
}
/**
 * G_BotEvolve
 * Used to make an alien evolve
 * Is called in G_BotThink
 */
void G_BotEvolve ( gentity_t *self, usercmd_t *botCmdBuffer )
{
    // very not-clean code, but hey, it works and I'm lazy 
    int res;
//    if(!G_BotEvolveToClass(self, "human_bsuit", botCmdBuffer))
//^^fuck them suits - they just waste their evos. We need proper human players. I'm sorry.
//Ehem. Remember to re-disable this after LAN-use because it is useless againts humans. Well almost.
    if(!G_BotEvolveToClass(self, "level4", botCmdBuffer))
        if(!G_BotEvolveToClass(self, "level3upg", botCmdBuffer)) {
            res = (random()>0.7) ? G_BotEvolveToClass(self, "level3", botCmdBuffer) : G_BotEvolveToClass(self, "level2upg", botCmdBuffer);
            if(!res) {
                res = (random()>0.5) ? G_BotEvolveToClass(self, "level2", botCmdBuffer) : G_BotEvolveToClass(self, "level1upg", botCmdBuffer);
                if(!res)
                    if(!G_BotEvolveToClass(self, "level1", botCmdBuffer))
                       if (!G_BotEvolveToClass(self, "level0", botCmdBuffer))
                       		G_BotEvolveToClass(self, "builderupg", botCmdBuffer);
            }
        }
}
void G_BotRoam(gentity_t *self, usercmd_t *botCmdBuffer) {
    int buildingIndex;
    qboolean teamRush;
    if(self->client->ps.stats[STAT_PTEAM] == PTE_HUMANS) {
        buildingIndex = botFindBuilding(self, BA_A_OVERMIND, -1);
        if(buildingIndex == ENTITYNUM_NONE) {
            buildingIndex = botFindBuilding(self, BA_A_SPAWN, -1);
        }
        teamRush = (level.time % 300000 < 150000) ? qtrue : qfalse;
    } else {
        buildingIndex = botFindBuilding(self, BA_H_REACTOR, -1);
        if(buildingIndex == ENTITYNUM_NONE) {
            buildingIndex = botFindBuilding(self, BA_H_SPAWN, -1);
        }
        teamRush = (level.time % 300000 > 150000) ? qtrue : qfalse;
    }
    if(buildingIndex != ENTITYNUM_NONE && teamRush ) {
        if(buildingIndex != getTargetEntityNumber(self->botMind->goal))
            setGoalEntity(self,&g_entities[buildingIndex]);
        else
            G_BotMoveDirectlyToGoal(self, botCmdBuffer);
    }else {
        G_BotSearchForGoal(self, botCmdBuffer); 
    }
}
/**
 * G_BotReactToEnemy
 * Class specific movement upon seeing an enemy
 * We need this to prevent some alien classes from being an easy target by walking in a straight line
 * ,but also prevent others from being unable to get to the enemy
 * please only use if the bot can SEE his enemy (botTargetinRange == qtrue)
*/
void G_BotReactToEnemy(gentity_t *self, usercmd_t *botCmdBuffer) {
    vec3_t forward,right,up,muzzle, targetPos;
    AngleVectors(self->client->ps.viewangles, forward, right, up);
    CalcMuzzlePoint(self, forward, right, up, muzzle);
    switch(self->client->ps.stats[STAT_PCLASS]) {
        case PCL_ALIEN_BUILDER0_UPG:
        case PCL_ALIEN_LEVEL0:
        case PCL_ALIEN_LEVEL1:
        case PCL_ALIEN_LEVEL1_UPG:
            //stop following a route since we can wall walk
            self->botMind->followingRoute = qfalse;
            //dodge
            G_BotDodge(self,botCmdBuffer);
            break;
        case PCL_ALIEN_LEVEL2:
        case PCL_ALIEN_LEVEL2_UPG:
            if(DistanceSquared(self->s.pos.trBase, level.nodes[self->botMind->targetNodeID].coord) > Square(400))
                botCmdBuffer->upmove = 20;
            break;
        case PCL_ALIEN_LEVEL3:
        case PCL_ALIEN_LEVEL3_UPG:
            self->botMind->followingRoute = qfalse;
            //dodge: stop dodging within a 150-400 range for a good pounce
	    //also helps get around corners when stuck
            if(DistanceSquared( muzzle, targetPos ) > 400)
		{
			G_BotDodge(self,botCmdBuffer);
		}
            else if(DistanceSquared( muzzle, targetPos ) < 150)
		{
			G_BotDodge(self,botCmdBuffer);
		}
            /*
            getTargetPos(self->botMind->goal,&targetPos);
            //pounce to the target
            if(DistanceSquared( muzzle, targetPos ) > Square(LEVEL3_CLAW_RANGE) && 
                self->client->ps.stats[ STAT_MISC ] < LEVEL3_POUNCE_UPG_SPEED) {
                //look up a bit more
                botCmdBuffer->angles[PITCH] -= 3000.0f;
                botCmdBuffer->buttons |= BUTTON_ATTACK2;
            }*/
            break;
        case PCL_ALIEN_LEVEL4:
            getTargetPos(self->botMind->goal, &targetPos);
            //use charge to approach more quickly
            if (DistanceSquared( muzzle, targetPos) > Square(LEVEL4_CLAW_RANGE))
                botCmdBuffer->buttons |= BUTTON_ATTACK2;
            break;
        case PCL_HUMAN:
        case PCL_HUMAN_BSUIT:
            if(self->s.weapon == WP_PAIN_SAW) //we ALWAYS want psaw users to fire, otherwise they only fire intermittently
	{
                botFireWeapon(self, botCmdBuffer);
		//dodge while going head-on from a far distance
            if(DistanceSquared( muzzle, targetPos ) > 100)
		{
			G_BotDodge(self,botCmdBuffer);
		}
	}
            break;
        default: break;
    }
        
}
/**
 * G_BotDodge
 * Makes the bot dodge :P
 */
void G_BotDodge(gentity_t *self, usercmd_t *botCmdBuffer) {
    if(self->client->time1000 >= 800) //>= 500
        botCmdBuffer->rightmove = 127;
    else
        botCmdBuffer->rightmove = -127;
    
    if((self->client->time10000 % 2000) < 1000)
        botCmdBuffer->rightmove *= -1;
    
    if((self->client->time1000 % 300) >= 100 && (self->client->time10000 % 3000) > 2000)
        botCmdBuffer->rightmove = 0;
}
/**
 * botGetAimEntityNumber
 * Returns the entity number of the entity that the bot is currently aiming at
 */
int botGetAimEntityNumber(gentity_t *self) {
    vec3_t forward;
    vec3_t end;
    trace_t trace;
    AngleVectors( self->client->ps.viewangles, forward, NULL,NULL);
    
    
    VectorMA(self->client->ps.origin, 4092, forward, end);
    
    trap_Trace(&trace, self->client->ps.origin, NULL, NULL, end, self->s.number, MASK_SHOT);
    return trace.entityNum;
}
/**botTargetInAttackRange
 * Tells if the bot is in range of attacking a target
 */
qboolean botTargetInAttackRange(gentity_t *self, botTarget_t target) {
    float range,secondaryRange;
    vec3_t forward,right,up;
    vec3_t muzzle, targetPos;
    vec3_t myMaxs, targetMaxs;
    trace_t trace;
    int distance, myMax, targetMax;
    AngleVectors( self->client->ps.viewangles, forward, right, up);
    
    CalcMuzzlePoint( self, forward, right, up , muzzle);
    BG_FindBBoxForClass(self->client->ps.stats[STAT_PCLASS], NULL, myMaxs, NULL, NULL, NULL);
    
    if(targetIsEntity(target) && target.ent->client)
        BG_FindBBoxForClass(target.ent->client->ps.stats[STAT_PCLASS], NULL,targetMaxs, NULL, NULL, NULL);
    else if(targetIsEntity(target) && getTargetType(target) == ET_BUILDABLE)
        BG_FindBBoxForBuildable(target.ent->s.modelindex, NULL, targetMaxs);
    else 
        VectorSet(targetMaxs, 0, 0, 0);
    targetMax = VectorLengthSquared(targetMaxs);
    myMax = VectorLengthSquared(myMaxs);
    
    switch(self->s.weapon) {
        case WP_ABUILD:
            range = 0; //poor granger :(
            secondaryRange = 0;
            break;
        case WP_ABUILD2:
            range = ABUILDER_CLAW_RANGE;
            secondaryRange = 350; //An arbitrary value for the blob launcher, has nothing to do with actual range
            break;
        case WP_ALEVEL0:
            range = LEVEL0_BITE_RANGE;
            secondaryRange = 0;
            break;
        case WP_ALEVEL1:
            range = LEVEL1_CLAW_RANGE;
            secondaryRange = 0;
            break;
        case WP_ALEVEL1_UPG:
            range = LEVEL1_CLAW_RANGE * 3;
            secondaryRange = LEVEL1_PCLOUD_RANGE;
            break;
        case WP_ALEVEL2:
            range = LEVEL2_CLAW_RANGE;
            secondaryRange = 0;
            break;
        case WP_ALEVEL2_UPG:
            range = LEVEL2_CLAW_RANGE;
            secondaryRange = LEVEL2_AREAZAP_RANGE;
            break;
        case WP_ALEVEL3:
            range = LEVEL3_CLAW_RANGE;
            secondaryRange = 900; //An arbitrary value for pounce, has nothing to do with actual range
            break;
        case WP_ALEVEL3_UPG:
            range = LEVEL3_CLAW_RANGE;
            secondaryRange = 1200; //An arbitrary value for pounce and barbs, has nothing to do with actual range
            break;
        case WP_ALEVEL4:
            range = LEVEL4_CLAW_RANGE;
            secondaryRange = 0; //Using 0 since tyrant charge is already defined above
            break;
        case WP_HBUILD:
            range = 100; //heal range
            secondaryRange = 0;
            break;
        case WP_PAIN_SAW:
            range = PAINSAW_RANGE;
            secondaryRange = 0;
            break;
        case WP_FLAMER:
            range = FLAMER_SPEED * 1.6; //takes some speed from the user right?
            secondaryRange = 0;
            break;
        case WP_LAS_GUN:
            range = (100 * 8192)/RIFLE_SPREAD; //Use rifle's range
            secondaryRange = 0;
            break;
        case WP_SHOTGUN:
            range = (100 * 8192)/SHOTGUN_SPREAD; //100 is the maximum radius we want the spread to be
            secondaryRange = 0;
            break;
        case WP_MACHINEGUN:
            range = (100 * 8192)/RIFLE_SPREAD; //100 is the maximum radius we want the spread to be
            secondaryRange = 0;
            break;
        case WP_CHAINGUN:
            range = 600;
//            range = (100 * 8192)/CHAINGUN_SPREAD; //100 is the maximum radius we want the spread to be
            secondaryRange = (100 * 8192)/RIFLE_SPREAD; //secondary uses rifle sread so yeah
	    break;

        case WP_LUCIFER_CANNON:
            secondaryRange = 150; //suprise?!? :D - shoots primary as it loses its charge then shoots a secondary straight after- a combo attack
            range = (100 * 8192)/CHAINGUN_SPREAD; //not too far
            break;
        default:
            range = 4098 * 4; //large range for guns because guns have large ranges :)
            secondaryRange = 0; //no secondary attack
    }
    getTargetPos(target, &targetPos);
    botGetAimLocation(target, &targetPos);
    trap_Trace(&trace,muzzle,NULL,NULL,targetPos,self->s.number,MASK_SHOT);
    distance = DistanceSquared(self->s.pos.trBase, targetPos);
    distance = (int) distance - myMax/2 - targetMax/2;
    
    if((distance <= Square(range) || distance <= Square(secondaryRange))
    &&(trace.entityNum == getTargetEntityNumber(target) || trace.fraction == 1.0f) && !trace.startsolid)
        return qtrue;
    else
        return qfalse;
}
/**botFireWeapon
 * Makes the bot attack/fire his weapon
 */
void botFireWeapon(gentity_t *self, usercmd_t *botCmdBuffer) {
    vec3_t forward,right,up;
    vec3_t muzzle, targetPos;
    vec3_t myMaxs,targetMaxs;
    int distance, myMax,targetMax;
    BG_FindBBoxForClass(self->client->ps.stats[STAT_PCLASS], NULL, myMaxs, NULL, NULL, NULL);
    
    if(targetIsEntity(self->botMind->goal) && self->botMind->goal.ent->client)
        BG_FindBBoxForClass(self->botMind->goal.ent->client->ps.stats[STAT_PCLASS], NULL,targetMaxs, NULL, NULL, NULL);
    else if(targetIsEntity(self->botMind->goal) && getTargetType(self->botMind->goal) == ET_BUILDABLE)
        BG_FindBBoxForBuildable(self->botMind->goal.ent->s.modelindex, NULL, targetMaxs);
    else 
        VectorSet(targetMaxs, 0, 0, 0);
    targetMax = VectorLengthSquared(targetMaxs);
    myMax = VectorLengthSquared(myMaxs);
    getTargetPos(self->botMind->goal,&targetPos);
    distance = DistanceSquared(self->s.pos.trBase, targetPos);
    distance = (int) distance - myMax/2 - targetMax/2;
    
    AngleVectors(self->client->ps.viewangles, forward,right,up);
    CalcMuzzlePoint(self,forward,right,up,muzzle);
    if( self->client->ps.stats[STAT_PTEAM] == PTE_ALIENS ) {
        switch(self->client->ps.stats[STAT_PCLASS]) {
            case PCL_ALIEN_BUILDER0:
                botCmdBuffer->buttons |= BUTTON_GESTURE; //poor  grangie; taunt like you mean it!
                break;
            case PCL_ALIEN_BUILDER0_UPG:
                if(distance < Square(ABUILDER_CLAW_RANGE))
                    botCmdBuffer->buttons |= BUTTON_ATTACK2;
                else
                    botCmdBuffer->buttons |= BUTTON_USE_HOLDABLE;
                break;
            case PCL_ALIEN_LEVEL0:
                if((distance < Square(50)) && (distance > Square(350)) && (self->client->time1000 % 300 == 0))
                    botCmdBuffer->upmove = 20; //jump when getting close
                break; //nothing, auto hit =D
            case PCL_ALIEN_LEVEL1:
                botCmdBuffer->buttons |= BUTTON_ATTACK;
                break;
            case PCL_ALIEN_LEVEL1_UPG:
                if(distance <= Square(LEVEL1_CLAW_RANGE))
                    botCmdBuffer->buttons |= BUTTON_ATTACK;
                else
                    botCmdBuffer->buttons |= BUTTON_ATTACK2; //gas
                break;
            case PCL_HUMAN_BSUIT:
                if(distance > Square(250) && (distance < Square(600)) && (self->client->time1000 % 300 == 0))
                    botCmdBuffer->upmove = 20; //jump when getting close
                if(distance <= Square(LEVEL1_CLAW_RANGE*2))
                    botCmdBuffer->buttons |= BUTTON_ATTACK;
                else
                    botCmdBuffer->buttons |= BUTTON_ATTACK2; //gas
                break;
            case PCL_ALIEN_LEVEL2:
                if(self->client->time1000 % 300 == 0)
                    botCmdBuffer->upmove = 20; //jump when getting close
                botCmdBuffer->buttons |= BUTTON_ATTACK;
                break;
            case PCL_ALIEN_LEVEL2_UPG:
                if(self->client->time1000 % 300 == 0)
                    botCmdBuffer->upmove = 20; //jump
                if(distance <= Square(LEVEL2_CLAW_RANGE))
                    botCmdBuffer->buttons |= BUTTON_ATTACK;
                else
                    botCmdBuffer->buttons |= BUTTON_ATTACK2; //zap
                break;
            case PCL_ALIEN_LEVEL3:
                if(distance > Square(LEVEL3_CLAW_RANGE + LEVEL3_CLAW_RANGE/2) && 
                    self->client->ps.stats[ STAT_MISC ] < LEVEL3_POUNCE_SPEED) {
                    botCmdBuffer->angles[PITCH] -= Distance(self->s.pos.trBase,targetPos) * 6 - self->client->ps.delta_angles[PITCH]; //look up a bit more //*10 too high
                    botCmdBuffer->buttons |= BUTTON_ATTACK2; //pounce
                } else
                    botCmdBuffer->buttons |= BUTTON_ATTACK;
                break;
            case PCL_ALIEN_LEVEL3_UPG:
                if(self->client->ps.ammo[WP_ALEVEL3_UPG] > 0 && 
                    distance > Square(LEVEL3_CLAW_RANGE) ) {
                    botCmdBuffer->angles[PITCH] -= Distance(self->s.pos.trBase,targetPos) * 6 - self->client->ps.delta_angles[PITCH]; //look up a bit more
                    botCmdBuffer->buttons |= BUTTON_USE_HOLDABLE; //barb
                } else {       
                    if(distance > Square(LEVEL3_CLAW_RANGE + LEVEL3_CLAW_RANGE/2) && 
                    self->client->ps.stats[ STAT_MISC ] < LEVEL3_POUNCE_UPG_SPEED) {
                        botCmdBuffer->angles[PITCH] -= Distance(self->s.pos.trBase,targetPos) * 7 - self->client->ps.delta_angles[PITCH];; //look up a bit more
                        botCmdBuffer->buttons |= BUTTON_ATTACK2; //pounce
                    }else
                        botCmdBuffer->buttons |= BUTTON_ATTACK;
                }
                break;
            case PCL_ALIEN_LEVEL4:
                if (distance > Square(LEVEL4_CLAW_RANGE))
                    botCmdBuffer->buttons |= BUTTON_ATTACK2; //charge
                else
                    botCmdBuffer->buttons |= BUTTON_ATTACK;
                break;
            default: break; //nothing
        }
        
    } else if( self->client->ps.stats[STAT_PTEAM] == PTE_HUMANS ) {
        if(self->client->ps.weapon == WP_FLAMER)
        {
                botCmdBuffer->buttons |= BUTTON_ATTACK;
            
        } else if( self->client->ps.weapon == WP_LUCIFER_CANNON ) {
            /*if(self->client->ps.ammo[WP_LUCIFER_CANNON] < 10){
		botCmdBuffer->buttons |= BUTTON_ATTACK2; } //use secondary when low on ammo
            else */if( self->client->time10000 % 2700 ) {
                botCmdBuffer->buttons |= BUTTON_ATTACK;
                self->botMind->isFireing = qtrue;
            }
        } else if(self->client->ps.weapon == WP_HBUILD || self->client->ps.weapon == WP_HBUILD2) {
            botCmdBuffer->buttons |= BUTTON_ATTACK2;
        } else
            botCmdBuffer->buttons |= BUTTON_ATTACK; //just fire the damn gun!
            
    }
}
/**
 * Helper functions for managing botTarget_t structures
 * Please use these when you need to access something in a botTarget structure
 * If you use the structure directly, you may make the program crash if the member is undefined
 */
int getTargetEntityNumber(botTarget_t target) {
    if(target.ent) 
        return target.ent->s.number;
    else
        return ENTITYNUM_NONE;
}
void getTargetPos(botTarget_t target, vec3_t *rVec) {
    if(target.ent)
        VectorCopy( target.ent->s.origin, *rVec);
    else
        VectorCopy(target.coord, *rVec);
}
int getTargetTeam( botTarget_t target) {
    if(target.ent) {
        if(target.ent->client)
            return target.ent->client->ps.stats[STAT_PTEAM];
        else
            return target.ent->biteam;
    } else
        return PTE_NONE;
}
int getTargetType( botTarget_t target) {
    if(target.ent)
        return target.ent->s.eType;
    else
        return -1;
}
qboolean targetIsEntity( botTarget_t target) {
    if(target.ent)
        return qtrue;
    else
        return qfalse;
}
/**setGoalEntity
 * Used to set a new goal for the bot
 *The goal is an entity
 */
void setGoalEntity(gentity_t *self, gentity_t *goal ){
    self->botMind->goal.ent = goal;
    findRouteToTarget(self, self->botMind->goal);
    setNewRoute(self);
}

/**setGoalCoordinate
 * Used to set a new goal for the bot 
 * The goal is a coordinate
 */
void setGoalCoordinate(gentity_t *self, vec3_t goal ) {
    VectorCopy(goal,self->botMind->goal.coord);
    self->botMind->goal.ent = NULL;
    findRouteToTarget(self, self->botMind->goal);
    setNewRoute(self);
}
/**setTargetEntity
 * Generic function for setting a botTarget that is not our current goal
 * Note, this function does not compute a route to the specified target
 */
void setTargetEntity(botTarget_t *target, gentity_t *goal ){
    target->ent = goal;
}
/**setTargetCoordinate
 * Generic function for setting a botTarget that is not our current goal
 * Note, this function does not compute a route to the specified target
 */
void setTargetCoordinate(botTarget_t *target, vec3_t goal ) {
    VectorCopy(goal, target->coord);
    target->ent = NULL;
}

int botFindBuilding(gentity_t *self, int buildingType, int range) {
    // The range of our scanning field.
    //int vectorRange = MGTURRET_RANGE * 5;
    // vectorRange converted to a vector
    vec3_t vectorRange;
    // Lower bound vector
    vec3_t mins;
    // Upper bound vector
    vec3_t maxs;
    // Indexing field
    int total_entities;
    int entityList[ MAX_GENTITIES ];
    int i;
    float minDistance = -1;
    int closestBuilding = ENTITYNUM_NONE;
    float newDistance;
    gentity_t *target;
    if(range != -1) {
        VectorSet( vectorRange, range, range, range );
        VectorAdd( self->client->ps.origin, vectorRange, maxs );
        VectorSubtract( self->client->ps.origin, vectorRange, mins );
        total_entities = trap_EntitiesInBox(mins, maxs, entityList, MAX_GENTITIES);
        for( i = 0; i < total_entities; ++i ) {
            target = &g_entities[entityList[ i ] ];
            
            if( target->s.eType == ET_BUILDABLE && target->s.modelindex == buildingType && (target->biteam == PTE_ALIENS || target->powered)) {
                newDistance = DistanceSquared( self->s.pos.trBase, target->s.pos.trBase );
                if( newDistance < minDistance|| minDistance == -1) {
                    minDistance = newDistance;
                    closestBuilding = entityList[i];
                }
            }
            
        }
    } else {
        for( i = 0; i < MAX_GENTITIES; ++i ) {
            target = &g_entities[i];
            
            if( target->s.eType == ET_BUILDABLE && target->s.modelindex == buildingType && (target->biteam == PTE_ALIENS || target->powered)) {
                newDistance = DistanceSquared(self->s.pos.trBase,target->s.pos.trBase);
                if( newDistance < minDistance|| minDistance == -1) {
                    minDistance = newDistance;
                    closestBuilding = i;
                }
            }
            
        }
    }
    
    return closestBuilding;
}
    
    
void G_BotSpectatorThink( gentity_t *self ) {
    if( self->client->ps.pm_flags & PMF_QUEUED) {
        //we're queued to spawn, all good
        //check for humans in the spawn que
        {
            spawnQueue_t *sq;
            if(self->client->pers.teamSelection == PTE_HUMANS)
                sq = &level.humanSpawnQueue;
            else if(self->client->pers.teamSelection == PTE_ALIENS)
                sq = &level.alienSpawnQueue;
            else
                sq = NULL;
            
            if( sq && G_BotCheckForSpawningPlayers( self )) {
                G_RemoveFromSpawnQueue( sq, self->s.number );
                G_PushSpawnQueue( sq, self->s.number );
            }
        }
        return;
    }
    
    //reset stuff
    self->botMind->followingRoute = qfalse;
    setTargetEntity(&self->botMind->goal, NULL);
    self->botMind->state = FINDNEWNODE;
    self->botMind->targetNodeID = -1;
    self->botMind->lastNodeID = -1;
    
    if( self->client->sess.sessionTeam == TEAM_SPECTATOR ) {
        int teamnum = self->client->pers.teamSelection;
        int clientNum = self->client->ps.clientNum;

        if( teamnum == PTE_HUMANS ) {
            self->client->pers.classSelection = PCL_HUMAN;
            self->client->ps.stats[STAT_PCLASS] = PCL_HUMAN;

            self->client->pers.humanItemSelection = self->botMind->spawnItem;

            G_PushSpawnQueue( &level.humanSpawnQueue, clientNum );
        } else if( teamnum == PTE_ALIENS) {
            self->client->pers.classSelection = PCL_ALIEN_LEVEL0;//PCL_ALIEN_LEVEL0
            self->client->ps.stats[STAT_PCLASS] = PCL_ALIEN_LEVEL0; //PCL_ALIEN_LEVEL0 then PCL_ALIEN_BUILDER0_UPG for grangie
            G_PushSpawnQueue( &level.alienSpawnQueue, clientNum );
        }
    }
}
qboolean G_BotCheckForSpawningPlayers( gentity_t *self ) 
{
    //this function only checks if there are Humans in the SpawnQueue
    //which are behind the bot
    int i;
    int botPos = 0, lastPlayerPos = 0;
    spawnQueue_t *sq;
    
    if(self->client->pers.teamSelection == PTE_HUMANS)
        sq = &level.humanSpawnQueue;
    
    else if(self->client->pers.teamSelection == PTE_ALIENS)
        sq = &level.alienSpawnQueue;
    else
        return qfalse;
    
    i = sq->front;
    
    if( G_GetSpawnQueueLength( sq ) ) {
        do {
            if( !(g_entities[ sq->clients[ i ] ].r.svFlags & SVF_BOT)) {
                if( i < sq->front )
                    lastPlayerPos = i + MAX_CLIENTS - sq->front;
                else
                    lastPlayerPos = i - sq->front;
            }
            
            if( sq->clients[ i ] == self->s.number ) {
                if( i < sq->front )
                    botPos = i + MAX_CLIENTS - sq->front;
                else
                    botPos = i - sq->front;
            }
            
            i = QUEUE_PLUS1( i );
        } while( i != QUEUE_PLUS1( sq->back ) );
    }
    
    if(botPos < lastPlayerPos)
        return qtrue;
    else
        return qfalse;
}
/*
 * Called when we are in intermission.
 * Just flag that we are ready to proceed.
 */
void G_BotIntermissionThink( gclient_t *client )
{
    client->readyToExit = qtrue;
}
void botGetAimLocation( botTarget_t target, vec3_t *aimLocation) {
    //get the position of the enemy
    getTargetPos(target, aimLocation);
    //gentity_t *targetEnt = &g_entities[getTargetEntityNumber(target)];
    
    if(getTargetType(target) != ET_BUILDABLE && getTargetTeam(target) == PTE_HUMANS && getTargetEntityNumber(target) != ENTITYNUM_NONE)
        (*aimLocation)[2] += g_entities[getTargetEntityNumber(target)].r.maxs[2] * 0.85;
    if(getTargetType(target) == ET_BUILDABLE) {
        VectorCopy( g_entities[getTargetEntityNumber(target)].s.origin, *aimLocation );
    }
}

void botAimAtLocation( gentity_t *self, vec3_t target, usercmd_t *rAngles)
{
        vec3_t aimVec, oldAimVec, aimAngles;
        vec3_t refNormal, grapplePoint, xNormal, viewBase;
        //vec3_t highPoint;
        float turnAngle;
        int i;
        vec3_t forward;

        if( ! (self && self->client) )
                return;
        VectorCopy( self->s.pos.trBase, viewBase );
        viewBase[2] += self->client->ps.viewheight;
        
        VectorSubtract( target, viewBase, aimVec );
        VectorCopy(aimVec, oldAimVec);

        {
        VectorSet(refNormal, 0.0f, 0.0f, 1.0f);
        VectorCopy( self->client->ps.grapplePoint, grapplePoint);
        //cross the reference normal and the surface normal to get the rotation axis
        CrossProduct( refNormal, grapplePoint, xNormal );
        VectorNormalize( xNormal );
        turnAngle = RAD2DEG( acos( DotProduct( refNormal, grapplePoint ) ) );
        //G_Printf("turn angle: %f\n", turnAngle);
        }

        if(self->client->ps.stats[ STAT_STATE ] & SS_WALLCLIMBINGCEILING ) {
                //NOTE: the grapplePoint is here not an inverted refNormal :(
                RotatePointAroundVector( aimVec, grapplePoint, oldAimVec, -180.0);
        }
        else if( turnAngle != 0.0f)
                RotatePointAroundVector( aimVec, xNormal, oldAimVec, -turnAngle);

        vectoangles( aimVec, aimAngles );

        VectorSet(self->client->ps.delta_angles, 0.0f, 0.0f, 0.0f);

        for( i = 0; i < 3; i++ ) {
                aimAngles[i] = ANGLE2SHORT(aimAngles[i]);
        }

        AngleVectors( self->client->ps.viewangles, forward, NULL, NULL);
        //save bandwidth
        SnapVector(aimAngles);
        rAngles->angles[0] = aimAngles[0];
        rAngles->angles[1] = aimAngles[1];
        rAngles->angles[2] = aimAngles[2];
}
//blatently ripped from ShotgunPattern() in g_weapon.c :)
void botShakeAim( gentity_t *self, vec3_t *rVec ){
    vec3_t forward, right, up, diffVec;
    int seed;
    float r,u, length, speedAngle;
    AngleVectors(self->client->ps.viewangles, forward, right, up);
    //seed crandom
    seed = (int) rand() & 255;
    //Distance(self->s.origin, *rVec)/8192;
    //int shakeMag = 8192 / self->botMind->botSkill.aimShake
    VectorSubtract(*rVec,self->s.origin, diffVec);
    length = (float) VectorLength(diffVec)/1000;
    VectorNormalize(diffVec);
    speedAngle=RAD2DEG(acos(DotProduct(forward,diffVec)))/100;
    r = crandom() * self->botMind->botSkill.aimShake * length * speedAngle;
    u = crandom() * self->botMind->botSkill.aimShake * length * speedAngle;
    VectorMA(*rVec, r, right, *rVec);
    VectorMA(*rVec,u,up,*rVec);
}
int botFindClosestEnemy( gentity_t *self, qboolean includeTeam ) {
    // return enemy entity index, or -1
    int vectorRange = MGTURRET_RANGE * 3;
    int i;
    int total_entities;
    int entityList[ MAX_GENTITIES ];
    float minDistance = (float) Square(MGTURRET_RANGE * 3);
    int closestTarget = ENTITYNUM_NONE;
    float newDistance;
    vec3_t range;
    vec3_t mins, maxs;
    gentity_t *target;
    botTarget_t botTarget;
    
    VectorSet( range, vectorRange, vectorRange, vectorRange );
    VectorAdd( self->client->ps.origin, range, maxs );
    VectorSubtract( self->client->ps.origin, range, mins );
    SnapVector(mins);
    SnapVector(maxs);
    total_entities = trap_EntitiesInBox( mins, maxs, entityList, MAX_GENTITIES );
    
    for( i = 0; i < total_entities; ++i ) {
        target = &g_entities[entityList[ i ] ];
        setTargetEntity(&botTarget, target);
        //DistanceSquared for performance reasons (doing sqrt constantly is bad and keeping it squared does not change result)
        newDistance = (float) DistanceSquared( self->s.pos.trBase, target->s.pos.trBase );
        //if entity is closer than previous stored one and the target is alive
        if( newDistance < minDistance && target->health > 0) {
            
            //if we can see the entity OR we are on aliens (who dont care about LOS because they have radar)
            if( (self->client->ps.stats[STAT_PTEAM] == PTE_ALIENS ) || botTargetInRange(self, botTarget, MASK_SHOT) ){
                
                //if the entity is a building and we can attack structures and we are not a normal granger
                if(target->s.eType == ET_BUILDABLE && g_bot_attackStruct.integer && self->client->ps.stats[STAT_PCLASS] != PCL_ALIEN_BUILDER0) {
                    
                    //if the building is not on our team (unless we can attack teamates)
                    if( target->biteam != self->client->ps.stats[STAT_PTEAM] || includeTeam ) {
                        
                        //store the new distance and the index of the entity
                        minDistance = newDistance;
                        closestTarget = entityList[i];
                    }
                    //if the entity is a player and not us
                } else if( target->client && self != target) {
                    //if we are not on the same team (unless we can attack teamates)
                    if( target->client->ps.stats[STAT_PTEAM] != self->client->ps.stats[STAT_PTEAM] || includeTeam ) {
                        
                        //store the new distance and the index of the enemy
                        minDistance = newDistance;
                        closestTarget = entityList[i];
                    }
                }
            }
        }

    }
        return closestTarget;
}

qboolean botTargetInRange( gentity_t *self, botTarget_t target, int mask ) {
    trace_t trace;
    vec3_t  muzzle, targetPos;
    vec3_t  forward, right, up;

    // set aiming directions
    AngleVectors( self->client->ps.viewangles, forward, right, up );

    CalcMuzzlePoint( self, forward, right, up, muzzle );
    getTargetPos(target, &targetPos);
    trap_Trace( &trace, muzzle, NULL, NULL,targetPos, self->s.number, mask);

    if( trace.surfaceFlags & SURF_NOIMPACT )
        return qfalse;

    //traceEnt = &g_entities[ trace.entityNum ];
        
    //target is in range
    if( (trace.entityNum == getTargetEntityNumber(target) || trace.fraction == 1.0f) && !trace.startsolid )
        return qtrue;
    return qfalse;
}

//Begin node/waypoint/path functions
int distanceToTargetNode( gentity_t *self ) {
        return (int) Distance(level.nodes[self->botMind->targetNodeID].coord, self->s.pos.trBase);
}
void botSlowAim( gentity_t *self, vec3_t target, float slow, vec3_t *rVec) {
        vec3_t viewBase;
        vec3_t aimVec, forward;
        vec3_t skilledVec;
        float slowness;
        
        if( !(self && self->client) )
                return;
        
        //get the point of where the bot is aiming from
        VectorCopy( self->s.pos.trBase, viewBase );
        viewBase[2] += self->client->ps.viewheight;
        
        //random targetVec manipulation (point of enemy)
        
        //get the Vector from the bot to the enemy (aim Vector)
        VectorSubtract( target, viewBase, aimVec );
        //take the current aim Vector
        AngleVectors( self->client->ps.viewangles, forward, NULL, NULL);
        
        //make the aim slow by not going the full difference
        //between the current aim Vector and the new one
        slowness = slow*(15/1000.0);
        if(slowness > 1.0) slowness = 1.0;
        
        VectorLerp( slowness, forward, aimVec, skilledVec);
        
        VectorAdd(viewBase, skilledVec, *rVec);
}

int findClosestNode( botTarget_t target ) {
        trace_t trace;
        int i,k,n = 0;
        float distance = 0;
        float closestNodeDistances[10] = {-1,-1,-1,-1, -1, -1, -1, -1, -1, -1};
        int closestNodes[10];
        vec3_t start;
        getTargetPos(target, &start);
        for(i = 0; i < level.numNodes; i++) {
            distance = DistanceSquared(start,level.nodes[i].coord);
            //check if new distance is shorter than one of the 4 we have
            for(k=0;k<10;k++) {
                if(distance < closestNodeDistances[k] || closestNodeDistances[k] == -1) {
                    //need to move the other elements up 1 index
                    for(n=k;n<9;n++) {
                        closestNodeDistances[n+1] = closestNodeDistances[n];
                        closestNodes[n+1] = closestNodes[n];
                    }
                    closestNodeDistances[k] = distance;
                    closestNodes[k] = i;
                    //get out of inner loop
                    break;
                } else {
                    continue;
                }
            }
        }
        //now loop through the closestnodes and find the closest node that is in LOS
        //note that they are sorted by distance in the array
        for(i = 0; i < 10; i++) {
            trap_Trace( &trace, start, NULL, NULL, level.nodes[closestNodes[i]].coord, getTargetEntityNumber(target), MASK_DEADSOLID );
            if( trace.fraction == 1.0f ) {
                return closestNodes[i];
            }
        }
        //no closest nodes are in LOS, pick the closest node regardless of LOS
        return closestNodes[0];
}


void doLastNodeAction(gentity_t *self, usercmd_t *botCmdBuffer) {
    vec3_t targetPos;
    getTargetPos(self->botMind->goal,&targetPos);
    switch(level.nodes[self->botMind->lastNodeID].action)
    {
        case BOT_JUMP:  
            
            if( self->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS && 
                self->client->ps.stats[ STAT_STAMINA ] < 0 )
            {break;}
            if( !BG_ClassHasAbility( self->client->ps.stats[ STAT_PCLASS ], SCA_WALLCLIMBER ) )
                
                botCmdBuffer->upmove = 20;
            break;
            //we should not need this now that wallclimb is always enabled in G_BotGoto
        case BOT_WALLCLIMB: if( BG_ClassHasAbility( self->client->ps.stats[ STAT_PCLASS ], SCA_WALLCLIMBER ) ) {
            botCmdBuffer->upmove = -1;
        }
        break;
        case BOT_KNEEL: if(self->client->ps.stats[ STAT_PTEAM ] == PTE_HUMANS)
        {
            botCmdBuffer->upmove = -1;
        }
        break;
        case BOT_POUNCE:if(self->client->ps.stats[STAT_PCLASS] == PCL_ALIEN_LEVEL3 && 
            self->client->ps.stats[ STAT_MISC ] < LEVEL3_POUNCE_SPEED) {
            botCmdBuffer->angles[PITCH] -= Distance(self->s.pos.trBase,targetPos) * 6 - self->client->ps.delta_angles[PITCH];
            botCmdBuffer->buttons |= BUTTON_ATTACK2;
            }else if(self->client->ps.stats[STAT_PCLASS] == PCL_ALIEN_LEVEL3_UPG && 
                self->client->ps.stats[ STAT_MISC ] < LEVEL3_POUNCE_UPG_SPEED) {
            botCmdBuffer->angles[PITCH] -= Distance(self->s.pos.trBase,targetPos) * 6 - self->client->ps.delta_angles[PITCH];
            botCmdBuffer->buttons |= BUTTON_ATTACK2;
                }
                break;
        default: break;
    }
    
}
void setNewRoute(gentity_t *self) { 
    self->botMind->timeFoundNode = level.time;
    self->botMind->targetNodeID = self->botMind->startNodeID;
    self->botMind->followingRoute = qtrue;
}

void findRouteToTarget( gentity_t *self, botTarget_t target ) {
    long shortDist[MAX_NODES];
    short i;
    short k;
    int bestNode;
    int childNode;
    short visited[MAX_NODES];
    //make startNum -1 so if there is no node close to us, we will not use the path
    short startNum = -1;
    short endNum = -1;
    vec3_t start = {0,0,0}; 
    trace_t trace, trace2;
    botTarget_t bot;
    setTargetEntity(&bot, self);
    VectorCopy(self->s.pos.trBase,start);
    //set initial variable values
    for( i=0;i<MAX_NODES;i++) {
        shortDist[i] = 99999999;
        self->botMind->routeToTarget[i] = -1;
        visited[i] = 0;
    }
    startNum = findClosestNode(bot);
    endNum = findClosestNode(target);
    
    
    shortDist[endNum] = 0;
    //NOTE: the algorithm has been reversed so we dont have to go through the final route and reverse it before we use it
    for (k = 0; k < MAX_NODES; ++k) {
        bestNode = -1;
        for (i = 0; i < MAX_NODES; ++i) {
            if (!visited[i] && ((bestNode == -1) || (shortDist[i] < shortDist[bestNode])))
                bestNode = i;
        }

        visited[bestNode] = 1;

        for (i = 0; i < MAX_PATH_NODES; ++i) {
            childNode = level.nodes[bestNode].nextid[i];
            if (childNode != -1 && childNode < 1000 ) {
                if (shortDist[bestNode] + level.distNode[bestNode][childNode] < shortDist[childNode]) {
                    shortDist[childNode] = shortDist[bestNode] + level.distNode[bestNode][childNode];
                    self->botMind->routeToTarget[childNode] = bestNode;
                }
            }
        }
    }
        self->botMind->lastRouteSearch = level.time;
        
        //now we check to see if we can skip the first node
        //we want to do this to avoid peculiar backtracking to the first node in the chain
        //note that we already know that there are at least 2 nodes in the route because of the previous check that startNode != endNode
        
        //can we see the second node?
        trap_Trace(&trace, start, NULL, NULL, level.nodes[self->botMind->routeToTarget[startNum]].coord, self->s.number, MASK_SHOT);
        
        //check if we are blocked from getting there
        trap_Trace(&trace2, self->s.pos.trBase, NULL, NULL, level.nodes[self->botMind->routeToTarget[startNum]].coord,self->s.number, MASK_SHOT);
        
        //we can see the second node and are not blocked? then start with that node
        if(trace.fraction == 1.0f && trace2.fraction == 1.0f && !trace.startsolid && !trace2.startsolid)
            self->botMind->startNodeID = self->botMind->routeToTarget[startNum];
        else //nope, start with the first node
            self->botMind->startNodeID = startNum;
}
void findNewNode( gentity_t *self, usercmd_t *botCmdBuffer) {
    botTarget_t target;
    
    int closestNode;
    setTargetEntity(&target, self);
    closestNode = findClosestNode(target);
    self->botMind->lastNodeID = -1;
    if(closestNode != -1) {
        self->botMind->targetNodeID = closestNode;
        self->botMind->timeFoundNode = level.time;
        self->botMind->state = TARGETNODE;
    } else {
        self->botMind->state = LOST;
        botCmdBuffer->forwardmove = 0;
        botCmdBuffer->upmove = -1;
        botCmdBuffer->rightmove = 0;
        botCmdBuffer->buttons = 0;
        botCmdBuffer->buttons = BUTTON_GESTURE; //|=
    }
}

void findNextNode( gentity_t *self )
{
    int randnum = 0;
    int i,nextNode = 0;
    int possibleNextNode = 0;
    int possibleNodes[5];
    int lasttarget = self->botMind->targetNodeID;
    possibleNodes[0] = possibleNodes[1] = possibleNodes[2] = possibleNodes[3] = possibleNodes[4] = 0;
    for(i = 0; i < 5; i++) {
        if(level.nodes[self->botMind->targetNodeID].nextid[i] < level.numNodes &&
            level.nodes[self->botMind->targetNodeID].nextid[i] >= 0) {
            if(self->botMind->lastNodeID >= 0) {
                if(self->botMind->lastNodeID == level.nodes[self->botMind->targetNodeID].nextid[i]) {
                    continue;
                }
            }
            possibleNodes[possibleNextNode] = level.nodes[self->botMind->targetNodeID].nextid[i];
        possibleNextNode++;
            }
    }
    if(possibleNextNode == 0) {
        self->botMind->state = FINDNEWNODE;
        return;
    }
    else {
        self->botMind->state = TARGETNODE;
        if(level.nodes[self->botMind->targetNodeID].random < 0) {
            nextNode = 0;
        }
        else {
            srand( trap_Milliseconds( ) );
            randnum = (int)(( (double)rand() / ((double)(RAND_MAX)+(double)(1)) ) * possibleNextNode);
            nextNode = randnum;
            //if(nextpath == possiblenextpath)
            //{nextpath = possiblenextpath - 1;}
            }
            self->botMind->lastNodeID = self->botMind->targetNodeID;
            self->botMind->targetNodeID = possibleNodes[nextNode];
            for(i = 0;i < 5;i++) {
                if(level.nodes[self->botMind->targetNodeID].nextid[i] == lasttarget) {
                    i = 5;
                }
            }
            
            self->botMind->timeFoundNode = level.time;
            return;
        }
        
        return;
    }
void setSkill(gentity_t *self, int skill) {
    self->botMind->botSkill.level = skill;
    //different aim for different teams
    if(self->botMind->botTeam == PTE_HUMANS) {
        self->botMind->botSkill.aimSlowness = (float) skill / 50;
        self->botMind->botSkill.aimShake = (int) (20 - (skill * 2) );
    } else {
        self->botMind->botSkill.aimSlowness = (float) skill / 10;
        self->botMind->botSkill.aimShake = (int) (10 - skill);
    }
}
    
