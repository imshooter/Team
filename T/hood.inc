#if defined _INC_T_HOOD
    #endinput
#endif
#define _INC_T_HOOD

#include <T\team>
#include <streamer>

#include <YSI_Data\y_iterate>
#include <YSI_Coding\y_hooks>

/**
 * # Header
 */

#if !defined MAX_HOODS
    #define MAX_HOODS (Hood:128)
#endif

#if !defined HOOD_COLOR_ALPHA
    #define HOOD_COLOR_ALPHA (0x80)
#endif

#if !defined HOOD_STREAMER_IDENTIFIER
    #define HOOD_STREAMER_IDENTIFIER (0)
#endif

#define INVALID_HOOD_ID (Hood:-1)

static enum E_HOOD_DATA {
    Float:E_HOOD_MIN_X,
    Float:E_HOOD_MIN_Y,
    Float:E_HOOD_MAX_X,
    Float:E_HOOD_MAX_Y,

    Team:E_HOOD_TEAM_ID,
    E_HOOD_ZONE_ID,
    STREAMER_TAG_AREA:E_HOOD_AREA_ID
};

static
    gHoodData[MAX_HOODS][E_HOOD_DATA]
;

const static
    HOOD_ITER_SIZE = _:MAX_HOODS
;

new stock
    Iterator:Hood<Hood:HOOD_ITER_SIZE>,
    Iterator:PlayerAtHood<HOOD_ITER_SIZE, MAX_PLAYERS>
;

static
    Hood:gInHoodID[MAX_PLAYERS] = { INVALID_HOOD_ID, ... }
;

/**
 * # Functions
 */

forward Hood:CreateHood(Float:minX, Float:minY, Float:maxX, Float:maxY);
forward bool:IsValidHood(Hood:hoodid);
forward bool:GetHoodPos(Hood:hoodid, &Float:minX, &Float:minY, &Float:maxX, &Float:maxY);
forward bool:SetHoodTeam(Hood:hoodid, Team:teamid);
forward bool:RemoveHoodFromTeam(Hood:hoodid);
forward bool:HasHoodTeam(Hood:hoodid);
forward bool:GetHoodTeam(Hood:hoodid, &Team:teamid);
forward GetHoodZoneID(Hood:hoodid);
forward bool:GetHoodAtPoint(Float:x, Float:y, Float:z, &Hood:hoodid);
forward bool:IsPlayerInAnyHood(playerid);
forward Hood:GetPlayerHoodID(playerid, &Hood:hoodid = INVALID_HOOD_ID);
forward bool:ShowHoodsForPlayer(playerid);

/**
 * # Calls
 */

forward OnPlayerEnterHood(playerid, Hood:hoodid);
forward OnPlayerLeaveHood(playerid, Hood:hoodid);

/**
 * # API
 */

stock Hood:CreateHood(Float:minX, Float:minY, Float:maxX, Float:maxY) {
    new const
        Hood:hoodid = Hood:Iter_Alloc(Hood)
    ;

    if (_:hoodid == INVALID_ITERATOR_SLOT) {
        return INVALID_HOOD_ID;
    }

    gHoodData[hoodid][E_HOOD_MIN_X] = minX;
    gHoodData[hoodid][E_HOOD_MIN_Y] = minY;
    gHoodData[hoodid][E_HOOD_MAX_X] = maxX;
    gHoodData[hoodid][E_HOOD_MAX_Y] = maxY;

    gHoodData[hoodid][E_HOOD_TEAM_ID] = INVALID_TEAM_ID;
    gHoodData[hoodid][E_HOOD_ZONE_ID] = GangZoneCreate(minX, minY, maxX, maxY);
    gHoodData[hoodid][E_HOOD_AREA_ID] = CreateDynamicRectangle(minX, minY, maxX, maxY);

    new
        data[2]
    ;

    data[0] = HOOD_STREAMER_IDENTIFIER;
    data[1] = _:hoodid;

    Streamer_SetArrayData(STREAMER_TYPE_AREA, gHoodData[hoodid][E_HOOD_AREA_ID], E_STREAMER_EXTRA_ID, data);

    return hoodid;
}

stock bool:IsValidHood(Hood:hoodid) {
    if (!(0 <= _:hoodid < HOOD_ITER_SIZE)) {
        return false;
    }

    return Iter_Contains(Hood, hoodid);
}

stock bool:GetHoodPos(Hood:hoodid, &Float:minX, &Float:minY, &Float:maxX, &Float:maxY) {
    if (!IsValidHood(hoodid)) {
        return false;
    }

    minX = gHoodData[hoodid][E_HOOD_MIN_X];
    minY = gHoodData[hoodid][E_HOOD_MIN_Y];
    maxX = gHoodData[hoodid][E_HOOD_MAX_X];
    maxY = gHoodData[hoodid][E_HOOD_MAX_Y];

    return true;
}

stock bool:SetHoodTeam(Hood:hoodid, Team:teamid) {
    if (!IsValidHood(hoodid)) {
        return false;
    }

    if (!IsValidTeam(teamid)) {
        return false;
    }

    gHoodData[hoodid][E_HOOD_TEAM_ID] = teamid;

    return true;
}

stock bool:RemoveHoodFromTeam(Hood:hoodid) {
    if (!IsValidHood(hoodid)) {
        return false;
    }

    if (!IsValidTeam(gHoodData[hoodid][E_HOOD_TEAM_ID])) {
        return false;
    }

    new const
        Team:teamid = gHoodData[hoodid][E_HOOD_TEAM_ID]
    ;

    gHoodData[hoodid][E_HOOD_TEAM_ID] = INVALID_TEAM_ID;

    return true;
}

stock bool:HasHoodTeam(Hood:hoodid) {
    if (!IsValidHood(hoodid)) {
        return false;
    }

    return IsValidTeam(gHoodData[hoodid][E_HOOD_TEAM_ID]);
}

stock bool:GetHoodTeam(Hood:hoodid, &Team:teamid) {
    if (!IsValidHood(hoodid)) {
        return false;
    }

    teamid = gHoodData[hoodid][E_HOOD_TEAM_ID];

    return true;
}

stock GetHoodZoneID(Hood:hoodid) {
    if (!IsValidHood(hoodid)) {
        return INVALID_GANG_ZONE;
    }

    return gHoodData[hoodid][E_HOOD_ZONE_ID];
}

stock bool:GetHoodAtPoint(Float:x, Float:y, Float:z, &Hood:hoodid) {
    new
        STREAMER_TAG_AREA:areas[256],
        data[2]
    ;

    for (new i, size = GetDynamicAreasForPoint(x, y, z, areas); i < size; ++i) {
        Streamer_GetArrayData(STREAMER_TYPE_AREA, areas[i], E_STREAMER_EXTRA_ID, data);

        if (data[0] == HOOD_STREAMER_IDENTIFIER) {
            hoodid = Hood:data[1];

            return true;
        }
    }

    return false;
}

stock bool:IsPlayerInAnyHood(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    return (gInHoodID[playerid] != INVALID_HOOD_ID);
}

stock Hood:GetPlayerHoodID(playerid, &Hood:hoodid = INVALID_HOOD_ID) {
    if (!IsPlayerConnected(playerid)) {
        hoodid = INVALID_HOOD_ID;
        
        return hoodid;
    }

    hoodid = gInHoodID[playerid];

    return hoodid;
}

stock bool:ShowHoodsForPlayer(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    foreach (new Hood:i : Hood) {
        GangZoneShowForPlayer(playerid, gHoodData[i][E_HOOD_ZONE_ID], (HOOD_COLOR_ALPHA | (~0xFF & GetTeamColor(gHoodData[i][E_HOOD_TEAM_ID]))));
    }

    return true;
}

/**
 * # Hooks
 */

hook OnPlayerEnterDynamicArea(playerid, STREAMER_TAG_AREA:areaid) {
    if (gInHoodID[playerid] == INVALID_HOOD_ID) {
        new
            data[2]
        ;

        Streamer_GetArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, data);

        if (data[0] == HOOD_STREAMER_IDENTIFIER) {
            new const
                Hood:hoodid = Hood:data[1]
            ;

            gInHoodID[playerid] = hoodid;
            Iter_Add(PlayerAtHood<_:hoodid>, playerid);
            
            if (CallLocalFunction("OnPlayerEnterHood", "ii", playerid, _:hoodid)) {
                return ~1;
            }
        }
    }
    
    return 1;
}

hook OnPlayerLeaveDynamicArea(playerid, STREAMER_TAG_AREA:areaid) {
    if (gInHoodID[playerid] != INVALID_HOOD_ID) {
        new
            data[2]
        ;

        Streamer_GetArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, data);

        if (data[0] == HOOD_STREAMER_IDENTIFIER) {
            new const
                Hood:hoodid = Hood:data[1]
            ;
        
            gInHoodID[playerid] = INVALID_HOOD_ID;
            Iter_Remove(PlayerAtHood<_:hoodid>, playerid);

            if (CallLocalFunction("OnPlayerLeaveHood", "ii", playerid, _:hoodid)) {
                return ~1;
            }
        }
    }
    
    return 1;
}

hook OnPlayerDisconnect(playerid, reason) {
    if (gInHoodID[playerid] != INVALID_HOOD_ID) {
        Iter_Remove(PlayerAtHood<_:gInHoodID[playerid]>, playerid);
        gInHoodID[playerid] = INVALID_HOOD_ID;
    }

    return 1;
}