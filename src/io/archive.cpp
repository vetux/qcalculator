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

#include <istream>
#include <utility>

#include "io/archive.hpp"

#include "archive.h"
#include "archive_entry.h"

Archive::Archive() = default;

Archive::Archive(std::istream &stream) {
    std::vector<char> buf = std::vector<char>(std::istreambuf_iterator<char>(stream),
                                              std::istreambuf_iterator<char>());

    auto a = archive_read_new();
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);
    archive_read_open_memory(a, buf.data(), buf.size());

    //500mb max archive input file size
    std::vector<char> readBuffer(100000 * 500);

    struct archive_entry *entry;
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        std::string path = archive_entry_pathname(entry);
        if (path.empty() || path.back() == '/' || path.back() == '\\') {
            archive_read_data_skip(a);
            continue; //Skip directory entries
        }
        auto ret = archive_read_data(a, readBuffer.data(), readBuffer.size());
        mEntries[path].resize(ret);
        for (int i = 0; i < ret; i++) {
            mEntries[path].at(i) = readBuffer.at(i);
        }
    }

    format = static_cast<Archive::Format>(archive_format(a));

    archive_read_close(a);
    archive_read_free(a);
}

Archive::Format Archive::getFormat() {
    return format;
}

void Archive::save(std::ostream &s, Archive::Format format) {
    size_t size = 0;
    for (auto &entry: mEntries)
        size += entry.first.size() + entry.second.size() + 1024;

    std::vector<char> buffer(size);
    size_t used;

    struct archive *a = archive_write_new();

    archive_write_set_format(a, format);
    archive_write_open_memory(a, buffer.data(), size, &used);

    for (auto &entry: mEntries) {
        auto *e = archive_entry_new();

        archive_entry_set_pathname(e, entry.first.c_str());
        archive_entry_set_size(e, entry.second.size());
        archive_entry_set_filetype(e, AE_IFREG);
        archive_entry_set_perm(e, 0644);

        archive_write_header(a, e);
        archive_write_data(a, entry.second.data(), entry.second.size());

        archive_entry_free(e);
    }

    archive_write_close(a);
    archive_write_free(a);

    s.write(buffer.data(), used);
}
