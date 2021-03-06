/*
 *  Copyright (c) 2014 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef __KIS_LOCK_FREE_LOD_COUNTER_H
#define __KIS_LOCK_FREE_LOD_COUNTER_H

#include <QAtomicInt>


class KisLockFreeLodCounter
{
public:
    void addLod(int newLod) {
        int oldValue = 0;
        int newValue = 0;

        do {
            oldValue = m_num;

            int counter;
            int lod;
            unpackLod(oldValue, &counter, &lod);

            if (!counter) {
                lod = newLod;
            } else {
                Q_ASSERT(lod == newLod);
            }

            counter++;
            newValue = packLod(counter, lod);
        } while(!m_num.testAndSetOrdered(oldValue, newValue));
    }

    void removeLod() {
        int oldValue = 0;
        int newValue = 0;

        do {
            oldValue = m_num;

            int counter;
            int lod;
            unpackLod(oldValue, &counter, &lod);

            Q_ASSERT(counter > 0);

            counter--;
            newValue = packLod(counter, lod);
        } while(!m_num.testAndSetOrdered(oldValue, newValue));
    }

    int readLod() const {
        int value = m_num;

        int counter;
        int lod;
        unpackLod(value, &counter, &lod);

        return counter ? lod : -1;
    }

    void testingClear() {
        m_num = 0;
    }

private:
    static inline int packLod(int counter, int lod) {
        return (counter << 8) | (lod & 0xFF);
    }

    static inline void unpackLod(int value, int *counter, int *lod) {
        *lod = value & 0xFF;
        *counter = value >> 8;
    }

private:
    QAtomicInt m_num;
};

#endif /* __KIS_LOCK_FREE_LOD_COUNTER_H */
