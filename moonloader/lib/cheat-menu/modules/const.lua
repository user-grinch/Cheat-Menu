-- Cheat Menu -  Cheat menu for Grand Theft Auto SanAndreas
-- Copyright (C) 2019-2020 Grinch_

-- This program is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 3 of the License, or
-- (at your option) any later version.

-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.

-- You should have received a copy of the GNU General Public License
-- along with this program.  If not, see <http://www.gnu.org/licenses/>.

local module = {}

-- Constant variables used throughout the menu

module.IDENTIFIER =
{
    VEHICLE   = 0,
    WEAPON    = 1,
    PAINTJOB  = 2,
    COMPONENT = 3,
    PED       = 4,
    CLOTHES   = 5,
    TELEPORT  = 6,
    MEMORY    = 8,
    ANIMATION = 9,
    MISSION   = 10
}

module.DRAW_TYPE =
{
    TEXT = 0,
    IMAGE = 1,
}

module.VEHICLE =
{
    IMAGE_HEIGHT = 75,
    IMAGE_WIDTH  = 100,
}

module.COMPONENT =
{
    IMAGE_HEIGHT = 80,
    IMAGE_WIDTH  = 100,
    TOTAL_SLOTS  = 14,
}

module.PAINTJOB =
{
    IMAGE_HEIGHT = 75,
    IMAGE_WIDTH  = 100,
}

module.PED =
{
    IMAGE_HEIGHT = 110,
    IMAGE_WIDTH  = 65,
}

module.WEAPON =
{
    IMAGE_HEIGHT = 65,
    IMAGE_WIDTH  = 65,
}

module.CLOTH =
{
    IMAGE_HEIGHT = 100,
    IMAGE_WIDTH  = 70,
}

module.TRAIN_CAM_FIX =
{
    ON  = 171,
    OFF = 6,
}

module.MEMORY_RB =
{
    NONE = 0,
    CAR  = 1,
    CHAR = 2,
}

module.DEFAULT_GANG_WEAPONS =
{
    {22,28,0}, -- Ballas
    {22,0,0}, -- Grove
    {22,0,0}, -- Vagos
    {0,0,0}, -- SF Rifa
    {22,28,0}, -- Da Nang Boys
    {24,0,0}, -- Mafia
    {22,30,0}, -- Triads
    {22,28,0}, -- VLA
    {0,0,0}, -- Gang 9 
    {0,0,0}, -- Gang 10
}

module.UPDATE_STATUS =
{
    DOWNLOADING   = 5,
    DOWNLOADED    = 58,
    HIDE_MSG      = 7,
    NEW_UPDATE    = 9,
}

module.VEHICLE_CLASS =
{
    IGNORE         = - 1,
    NORMAL         = 0,
    POOR_FAMILY    = 1,
    RICH_FAMILY    = 2,
    EXECUTIVE      = 3,
    WORKER         = 4,
    BIG            = 5,
    TAXI           = 6,
    MOPED          = 7,
    MOTOR_BIKE     = 8,
    LEISURE_BOAT   = 9,
    WORKER_BOAT    = 10,
    BICYCLE        = 11,
}

module.CHECKBOX_STATE =
{
    NOT_CONFIGURED = -1,
    OFF = 0,
    ON = 1,
}

module.MODEL_TYPE =
{
    INVALID = nil,
	ATOMIC = 1,
	TIME = 3,
	WEAPON = 4,
	CLUMP = 5,
	VEHICLE = 6,
	PED = 7,
	LOD = 8
}

return module