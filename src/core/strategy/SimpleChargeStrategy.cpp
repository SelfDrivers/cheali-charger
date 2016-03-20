/*
    cheali-charger - open source firmware for a variety of LiPo chargers
    Copyright (C) 2013  Paweł Stawicki. All right reserved.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "SimpleChargeStrategy.h"
#include "ProgramData.h"
#include "Screen.h"
#include "Hardware.h"
#include "TheveninMethod.h"
#include "Program.h"
#include "memory.h"

namespace SimpleChargeStrategy {

    Strategy::statusType doStrategy();

    const Strategy::VTable vtable PROGMEM = {
        powerOn,
        powerOff,
        doStrategy
    };
}


void SimpleChargeStrategy::powerOn()
{
    SMPS::powerOn();
    TheveninMethod::initialize(true);
    SMPS::trySetIout(Strategy::minI);
}

void SimpleChargeStrategy::powerOff()
{
    SMPS::powerOff();
}

Strategy::statusType SimpleChargeStrategy::doStrategy()
{
    AnalogInputs::ValueType Vout;
    SimpleChargeStrategy::calculateThevenin();
    Vout = AnalogInputs::getVbattery();

    if(Vout > Strategy::endV) {
        Program::stopReason = string_batteryVoltageReachedUpperLimit;
        return Strategy::ERROR;
    }
    SMPS::trySetIout(Strategy::maxI);

    return Strategy::RUNNING;
}



void SimpleChargeStrategy::calculateThevenin()
{
    if(AnalogInputs::isOutStable()) TheveninMethod::calculateRthVth(SMPS::getIout());
}



