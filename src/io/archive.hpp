/**
 *  QCalc - Extensible programming calculator
 *  Copyright (C) 2021  Julian Zampiccoli
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
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef QCALC_ARCHIVE_HPP
#define QCALC_ARCHIVE_HPP

#include <map>
#include <vector>
#include <string>
#include <memory>
#include <functional>

class Archive {
public:
    enum Format : int {
        ARCHIVE_FORMAT_BASE_MASK = 0xff0000,
        ARCHIVE_FORMAT_CPIO = 0x10000,
        ARCHIVE_FORMAT_CPIO_POSIX = (ARCHIVE_FORMAT_CPIO | 1),
        ARCHIVE_FORMAT_CPIO_BIN_LE = (ARCHIVE_FORMAT_CPIO | 2),
        ARCHIVE_FORMAT_CPIO_BIN_BE = (ARCHIVE_FORMAT_CPIO | 3),
        ARCHIVE_FORMAT_CPIO_SVR4_NOCRC = (ARCHIVE_FORMAT_CPIO | 4),
        ARCHIVE_FORMAT_CPIO_SVR4_CRC = (ARCHIVE_FORMAT_CPIO | 5),
        ARCHIVE_FORMAT_CPIO_AFIO_LARGE = (ARCHIVE_FORMAT_CPIO | 6),
        ARCHIVE_FORMAT_SHAR = 0x20000,
        ARCHIVE_FORMAT_SHAR_BASE = (ARCHIVE_FORMAT_SHAR | 1),
        ARCHIVE_FORMAT_SHAR_DUMP = (ARCHIVE_FORMAT_SHAR | 2),
        ARCHIVE_FORMAT_TAR = 0x30000,
        ARCHIVE_FORMAT_TAR_USTAR = (ARCHIVE_FORMAT_TAR | 1),
        ARCHIVE_FORMAT_TAR_PAX_INTERCHANGE = (ARCHIVE_FORMAT_TAR | 2),
        ARCHIVE_FORMAT_TAR_PAX_RESTRICTED = (ARCHIVE_FORMAT_TAR | 3),
        ARCHIVE_FORMAT_TAR_GNUTAR = (ARCHIVE_FORMAT_TAR | 4),
        ARCHIVE_FORMAT_ISO9660 = 0x40000,
        ARCHIVE_FORMAT_ISO9660_ROCKRIDGE = (ARCHIVE_FORMAT_ISO9660 | 1),
        ARCHIVE_FORMAT_ZIP = 0x50000,
        ARCHIVE_FORMAT_EMPTY = 0x60000,
        ARCHIVE_FORMAT_AR = 0x70000,
        ARCHIVE_FORMAT_AR_GNU = (ARCHIVE_FORMAT_AR | 1),
        ARCHIVE_FORMAT_AR_BSD = (ARCHIVE_FORMAT_AR | 2),
        ARCHIVE_FORMAT_MTREE = 0x80000,
        ARCHIVE_FORMAT_RAW = 0x90000,
        ARCHIVE_FORMAT_XAR = 0xA0000,
        ARCHIVE_FORMAT_LHA = 0xB0000,
        ARCHIVE_FORMAT_CAB = 0xC0000,
        ARCHIVE_FORMAT_RAR = 0xD0000,
        ARCHIVE_FORMAT_7ZIP = 0xE0000,
        ARCHIVE_FORMAT_WARC = 0xF0000,
        ARCHIVE_FORMAT_RAR_V5 = 0x100000,
    };

    static void extractToDisk(const std::string &archive, const std::string &outputDirectory,
                              std::function<void(const std::string &)> progressCallback);

    Archive();

    explicit Archive(std::istream &stream);

    Format getFormat();

    void addEntry(const std::string &name, const std::vector<char> &data) {
        mEntries[name] = data;
    }

    std::map<std::string, std::vector<char>> &entries() { return mEntries; }

    const std::map<std::string, std::vector<char>> &entries() const { return mEntries; }

    void save(std::ostream &stream, Format format);

private:
    Format format;
    std::map<std::string, std::vector<char>> mEntries;
};

#endif //QCALC_ARCHIVE_HPP
