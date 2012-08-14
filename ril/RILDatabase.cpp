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

#include <sqlite3.h>

#include "RILDatabase.h"
#include "config.h"

RILDatabase::RILDatabase() : m_database(NULL), m_selectOperator(NULL) {

}

RILDatabase::~RILDatabase() {
    sqlite3_finalize(m_selectOperator);
    sqlite3_close(m_database);
}

bool RILDatabase::open() {
    sqlite3_finalize(m_selectOperator);
    sqlite3_close(m_database);

    if(sqlite3_open_v2(DATABASE_PATH, &m_database, SQLITE_OPEN_READONLY,
        NULL) != SQLITE_OK) {

        setErrorString(sqlite3_errmsg(m_database));

        return false;
    }

    if(sqlite3_prepare_v2(m_database,
        "SELECT long_name, short_name FROM operators WHERE mccmnc = ? LIMIT 1",
        -1, &m_selectOperator, NULL) != SQLITE_OK) {

        setErrorString(sqlite3_errmsg(m_database));

        return false;
    }

    return true;
}

bool RILDatabase::lookupOperator(const std::string &mccmnc, std::string &longName,
                                 std::string &shortName) {

    int ret;

    ret = sqlite3_bind_text(m_selectOperator, 1, mccmnc.c_str(), -1, NULL);

    if(ret != SQLITE_OK) {
        setErrorString(sqlite3_errmsg(m_database));

        sqlite3_reset(m_selectOperator);

        return false;
    }

    ret = sqlite3_step(m_selectOperator);

    if(ret == SQLITE_ROW) {
        longName.assign((const char *) sqlite3_column_text(m_selectOperator, 0));
        shortName.assign((const char *) sqlite3_column_text(m_selectOperator, 1));

        ret = sqlite3_step(m_selectOperator);
    } else {
        longName.assign("<" + mccmnc + ">");
        shortName.assign(longName);
    }

    if(ret != SQLITE_DONE) {
        setErrorString(sqlite3_errmsg(m_database));

        sqlite3_reset(m_selectOperator);

        return false;
    }

    ret = sqlite3_reset(m_selectOperator);

    if(ret != SQLITE_OK) {
        setErrorString(sqlite3_errmsg(m_database));

        return false;
    }

    return true;
}
