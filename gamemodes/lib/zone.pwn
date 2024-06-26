#if defined _INC_ZONE_
    #endinput
#endif
#define _INC_ZONE_

#if !defined MAX_ZONES
    #define MAX_ZONES (Zone:128)
#endif

#if !defined ZONE_COLOR_ALPHA
    #define ZONE_COLOR_ALPHA (0x80)
#endif

#if !defined ZONE_STREAMER_IDENTIFIER
    #define ZONE_STREAMER_IDENTIFIER (0)
#endif

#define INVALID_ZONE_ID (Zone:-1)

static enum E_ZONE_DATA {
    Float:E_ZONE_MIN_X,
    Float:E_ZONE_MIN_Y,
    Float:E_ZONE_MAX_X,
    Float:E_ZONE_MAX_Y,

    Team:E_ZONE_TEAM_ID,
    E_ZONE_ZONE_ID,
    STREAMER_TAG_AREA:E_ZONE_AREA_ID
};

static
    gZoneData[MAX_ZONES][E_ZONE_DATA]
;

const static
    ZONE_ITER_SIZE = _:MAX_ZONES
;

new
    Iterator:Zone<Zone:ZONE_ITER_SIZE>,
    Iterator:PlayersInZone[MAX_ZONES]<MAX_PLAYERS>,
    Iterator:ZonesFromTeam[MAX_TEAMS]<Zone:ZONE_ITER_SIZE>
;

/**
 * # Functions
 */

forward Zone:CreateZone(Float:minX, Float:minY, Float:maxX, Float:maxY);
forward bool:IsValidZone(Zone:zoneid);
forward bool:GetZoneAtPoint(Float:x, Float:y, Float:z, &Zone:zoneid);
forward bool:GetPlayerZone(playerid, &Zone:zoneid);
forward bool:AddZoneToTeam(Zone:zoneid, Team:teamid);
forward bool:RemoveZoneFromTeam(Zone:zoneid);

forward OnPlayerEnterZone(playerid, Zone:zoneid);
forward OnPlayerLeaveZone(playerid, Zone:zoneid);

/**
 * # API
 */

stock Zone:CreateZone(Float:minX, Float:minY, Float:maxX, Float:maxY) {
    new const
        Zone:zoneid = Zone:Iter_Alloc(Zone)
    ;

    if (_:zoneid == INVALID_ITERATOR_SLOT) {
        return INVALID_ZONE_ID;
    }

    gZoneData[zoneid][E_ZONE_MIN_X] = minX;
    gZoneData[zoneid][E_ZONE_MIN_Y] = minY;
    gZoneData[zoneid][E_ZONE_MAX_X] = maxX;
    gZoneData[zoneid][E_ZONE_MAX_Y] = maxY;

    gZoneData[zoneid][E_ZONE_TEAM_ID] = INVALID_TEAM_ID;
    gZoneData[zoneid][E_ZONE_ZONE_ID] = GangZoneCreate(minX, minY, maxX, maxY);
    gZoneData[zoneid][E_ZONE_AREA_ID] = CreateDynamicRectangle(minX, minY, maxX, maxY);

    new
        data[2]
    ;

    data[0] = ZONE_STREAMER_IDENTIFIER;
    data[1] = _:zoneid;

    Streamer_SetArrayData(STREAMER_TYPE_AREA, gZoneData[zoneid][E_ZONE_AREA_ID], E_STREAMER_EXTRA_ID, data);

    return zoneid;
}

stock bool:IsValidZone(Zone:zoneid) {
    return (0 <= _:zoneid < ZONE_ITER_SIZE) && Iter_Contains(Zone, zoneid);
}

stock bool:AddZoneToTeam(Zone:zoneid, Team:teamid) {
    if (!IsValidZone(zoneid)) {
        return false;
    }

    if (!IsValidTeam(teamid)) {
        return false;
    }

    if (gZoneData[zoneid][E_ZONE_TEAM_ID] != INVALID_TEAM_ID) {
        return false;
    }

    gZoneData[zoneid][E_ZONE_TEAM_ID] = teamid;

    Iter_Add(ZonesFromTeam[teamid], zoneid);

    return true;
}

stock bool:RemoveZoneFromTeam(Zone:zoneid) {
    if (!IsValidZone(zoneid)) {
        return false;
    }

    if (!IsValidTeam(teamid)) {
        return false;
    }

    if (gZoneData[zoneid][E_ZONE_TEAM_ID] == INVALID_TEAM_ID) {
        return false;
    }

    gZoneData[zoneid][E_ZONE_TEAM_ID] = INVALID_TEAM_ID;

    Iter_Remove(ZonesFromTeam[teamid], zoneid);

    return true;
}

stock bool:GetZoneTeam(Zone:zoneid, &Team:teamid) {
    if (!IsValidZone(zoneid)) {
        return false;
    }

    teamid = gZoneData[zoneid][E_ZONE_TEAM_ID];

    return true;
}

stock bool:GetZoneAtPoint(Float:x, Float:y, Float:z, &Zone:zoneid) {
    new
        STREAMER_TAG_AREA:arr[256],
        data[2]
    ;

    for (new i, size = GetDynamicAreasForPoint(x, y, z, arr); i < size; ++i) {
        Streamer_GetArrayData(STREAMER_TYPE_AREA, arr[i], E_STREAMER_EXTRA_ID, data);

        if (data[0] == ZONE_STREAMER_IDENTIFIER) {
            zoneid = Zone:data[1];

            return true;
        }
    }

    return false;
}

stock bool:GetPlayerZone(playerid, &Zone:zoneid) {
    new
        Float:x,
        Float:y,
        Float:z
    ;

    if (!GetPlayerPos(playerid, x, y, z)) {
        return false;
    }

    return GetZoneAtPoint(x, y, z, zoneid);
}

stock bool:ShowZonesForPlayer(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    foreach (new Zone:i : Zone) {
        GangZoneShowForPlayer(playerid, gZoneData[i][E_ZONE_ZONE_ID], (ZONE_COLOR_ALPHA | (~0xFF & GetTeamColor(gZoneData[i][E_ZONE_TEAM_ID]))));
    }

    return true;
}

/**
 * # Hooks
 */

hook OnPlayerEnterDynamicArea(playerid, STREAMER_TAG_AREA:areaid) {
    new
        data[2]
    ;

    Streamer_GetArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, data);

    if (data[0] == ZONE_STREAMER_IDENTIFIER) {
        new const
            Zone:zoneid = Zone:data[1]
        ;

        Iter_Add(PlayersInZone[zoneid], playerid);
        CallLocalFunction("OnPlayerEnterZone", "ii", playerid, _:zoneid);
    }
    
    return 1;
}

hook OnPlayerLeaveDynamicArea(playerid, STREAMER_TAG_AREA:areaid) {
    new
        data[2]
    ;

    Streamer_GetArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, data);

    if (data[0] == ZONE_STREAMER_IDENTIFIER) {
        new const
            Zone:zoneid = Zone:data[1]
        ;
        
        Iter_Remove(PlayersInZone[zoneid], playerid);
        CallLocalFunction("OnPlayerLeaveZone", "ii", playerid, _:zoneid);
    }
    
    return 1;
}