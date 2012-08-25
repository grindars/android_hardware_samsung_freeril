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
import android.os.Parcelable;
import android.os.Parcel;
import java.io.ByteArrayOutputStream;
import java.io.ByteArrayInputStream;

public final class OemRequestReply extends ByteArrayOutputStream implements Parcelable {
    public static final Parcelable.Creator<OemRequestReply> CREATOR =
        new Parcelable.Creator<OemRequestReply>() {
            public OemRequestReply createFromParcel(Parcel source) {
                return new OemRequestReply(source);
            }

            public OemRequestReply[] newArray(int size) {
                return new OemRequestReply[size];
            }
        };

    public OemRequestReply() {
        super();
    }

    public OemRequestReply(Parcel source) {
        super();

        readFromParcel(source);
    }

    public int describeContents() {
        return 0;
    }

    public void readFromParcel(Parcel source) {
        count = source.readInt();
        buf = new byte[count];
        source.readByteArray(buf);
    }

    public void writeToParcel(Parcel dest, int flags) {
        dest.writeInt(count);
        dest.writeByteArray(buf);
    }

    public ByteArrayInputStream asInput() {
        return new ByteArrayInputStream(buf, 0, count);
    }
}
