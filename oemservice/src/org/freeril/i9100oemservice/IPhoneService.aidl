/*
 * Free RIL implementation for Samsung Android-based smartphones.
 * Copyright (C) 2012  Sergey Gridasov <grindars@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

package org.freeril.i9100oemservice;

import org.freeril.i9100oemservice.IEventHandler;

interface IPhoneService {
    boolean enterServiceMode(
        in int modeType,
        in int subType
    );

    boolean exitServiceMode(
        in int modeType
    );

    boolean sendServiceKeyCode(
        in int keyCode
    );

    void registerEventHandler(in IEventHandler handler);
    void unregisterEventHandler(in IEventHandler handler);
}
