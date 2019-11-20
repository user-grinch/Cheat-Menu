-- Cheat Menu -  Cheat menu for Grand Theft Auto SanAndreas
-- Copyright (C) 2019 Grinch_

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
}

module.DRAW_TYPE =
{
    LIST    = 0,
    SEARCH  = 1
}

module.VEHICLE =
{
    IMAGE_HEIGHT = 75,
    IMAGE_WIDTH  = 100,
    IMAGE_EXT    = "jpg"
}
module.COMPONENT =
{
    IMAGE_HEIGHT = 80,
    IMAGE_WIDTH  = 100,
    IMAGE_EXT    = "jpg"
}

module.PAINTJOB =
{
    IMAGE_HEIGHT = 75,
    IMAGE_WIDTH  = 100,
    IMAGE_EXT    = "png"
}

module.PED =
{
    IMAGE_HEIGHT = 110,
    IMAGE_WIDTH  = 55,
    IMAGE_EXT    = "jpg"
}

module.WEAPON =
{
    IMAGE_HEIGHT = 65,
    IMAGE_WIDTH  = 65,
    IMAGE_EXT    = "jpg"
}

module.CLOTH =
{
    IMAGE_HEIGHT = 100,
    IMAGE_WIDTH  = 80,
    IMAGE_EXT    = "jpg"
}

module.TRAIN_CAM_FIX =
{
    ON  = 171,
    OFF = 6,
}

module.VEHICLE_COLOR =
{
    MAX_VALUE  = 139,
    MIN_VALUE  = 0,
}

module.DRIVE_TYPE =
{
    FWD = 82,
    RWD = 83,
    AWD = 84,
}

module.ENGINE_TYPE =
{
    PETROL   = 80,
    DISEAL   = 81,
    ELECTRIC = 82,
}

module.LIGHTS =
{
    LONG  = 0,
    SMALL = 1,
    BIG   = 2,
    TALL  = 3,
}

return module