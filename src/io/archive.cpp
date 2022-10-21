/**
 *  QCalc - Extensible programming calculator
 *  Copyright (C) 2022  Julian Zampiccoli
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
#include <filesystem>

#include <QDir>

#include "io/archive.hpp"

#include "archive.h"
#include "archive_entry.h"

static int copy_data(struct archive *ar, struct archive *aw) {
    long r;
    const void *buff;
    size_t size;
    la_int64_t offset;

    for (;;) {
        r = archive_read_data_block(ar, &buff, &size, &offset);
        if (r == ARCHIVE_EOF)
            return (ARCHIVE_OK);
        if (r < ARCHIVE_OK)
            return (int) r;
        r = archive_write_data_block(aw, buff, size, offset);
        if (r < ARCHIVE_OK) {
            return (int) r;
        }
    }
}

const QStringList &Archive::getFormatMimeTypes() {
    static const QStringList ret = {
            "application/zip",
            "application/x-7z-compressed",
            "application/x-rar-compressed",
            "application/x-tar",
            "application/x-iso9660-image",
            "application/x-cpio",
            "application/x-shar",
    };
    return ret;
}

Archive::Format Archive::getFormatFromExtension(const std::string &extension) {
    auto ext = extension;
    if (!ext.empty() && *ext.begin() == '.') {
        ext.erase(ext.begin());
    }
    if (ext == "cpio") {
        return (Archive::Format) ARCHIVE_FORMAT_CPIO;
    } else if (ext == "shar") {
        return (Archive::Format) ARCHIVE_FORMAT_SHAR;
    } else if (ext == "tar") {
        return (Archive::Format) ARCHIVE_FORMAT_TAR;
    } else if (ext == "iso") {
        return (Archive::Format) ARCHIVE_FORMAT_ISO9660;
    } else if (ext == "zip") {
        return (Archive::Format) ARCHIVE_FORMAT_ZIP;
    } else if (ext == "ar") {
        return (Archive::Format) ARCHIVE_FORMAT_AR;
    } else if (ext == "raw" || ext == "bin") {
        return (Archive::Format) ARCHIVE_FORMAT_RAW;
    } else if (ext == "xar") {
        return (Archive::Format) ARCHIVE_FORMAT_XAR;
    } else if (ext == "lha") {
        return (Archive::Format) ARCHIVE_FORMAT_LHA;
    } else if (ext == "cab") {
        return (Archive::Format) ARCHIVE_FORMAT_CAB;
    } else if (ext == "rar") {
        return (Archive::Format) ARCHIVE_FORMAT_RAR;
    } else if (ext == "7zip") {
        return (Archive::Format) ARCHIVE_FORMAT_7ZIP;
    } else if (ext == "warc") {
        return (Archive::Format) ARCHIVE_FORMAT_WARC;
    } else {
        throw std::runtime_error("Unsupported extension " + ext);
    }
}

void Archive::extractToDisk(const std::string &filename,
                            const std::string &outputDirectory,
                            std::function<void(const std::string &)> progressCallback) {
    auto outputDir = outputDirectory;
    if (!outputDir.empty() && outputDir.back() != '/')
        outputDir += "/";

    auto originalPath = QDir::current();
    QDir::setCurrent(outputDir.c_str());

    struct archive *a;
    struct archive *ext;
    struct archive_entry *entry;
    int flags;
    int r;

    /* Select which attributes we want to restore. */
    flags = ARCHIVE_EXTRACT_TIME;
    flags |= ARCHIVE_EXTRACT_PERM;
    flags |= ARCHIVE_EXTRACT_ACL;
    flags |= ARCHIVE_EXTRACT_FFLAGS;

    a = archive_read_new();
    archive_read_support_format_all(a);
    archive_read_support_filter_all(a);
    ext = archive_write_disk_new();
    archive_write_disk_set_options(ext, flags);
    archive_write_disk_set_standard_lookup(ext);

    r = archive_read_open_filename(a, filename.c_str(), 10240);

    if (r < ARCHIVE_WARN)
        throw std::runtime_error("Failed to read archive " + filename);

    for (;;) {
        r = archive_read_next_header(a, &entry);
        if (r == ARCHIVE_EOF)
            break;
        if (r < ARCHIVE_WARN)
            throw std::runtime_error("Failed to read header: " + std::string(archive_error_string(a)));

        auto entryPath = std::string(archive_entry_pathname(entry));

        progressCallback("Extracting " + entryPath);

        r = archive_write_header(ext, entry);
        if (r < ARCHIVE_OK) {}
        else if (archive_entry_size(entry) > 0) {
            r = copy_data(a, ext);
            if (r < ARCHIVE_WARN)
                throw std::runtime_error("Failed to read header: " + std::string(archive_error_string(a)));
        }
        r = archive_write_finish_entry(ext);
        if (r < ARCHIVE_WARN)
            throw std::runtime_error("Failed to read header: " + std::string(archive_error_string(a)));
    }

    archive_read_close(a);
    archive_read_free(a);
    archive_write_close(ext);
    archive_write_free(ext);

    QDir::setCurrent(originalPath.path());
}


Archive::Archive() = default;

Archive::Archive(std::istream &stream) {
    std::vector<char> buf = std::vector<char>(std::istreambuf_iterator<char>(stream),
                                              std::istreambuf_iterator<char>());

    auto a = archive_read_new();
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);
    archive_read_open_memory(a, buf.data(), buf.size());

    struct archive_entry *entry;
    // Read Entry Headers
    for (;;) {
        auto r = archive_read_next_header(a, &entry);
        if (r == ARCHIVE_EOF)
            break;
        if (r < ARCHIVE_WARN)
            throw std::runtime_error("Failed to read header: " + std::string(archive_error_string(a)));

        std::string path = archive_entry_pathname(entry);
        auto &e = mEntries[path];

        if (archive_entry_size(entry) > 0) {
            // Read Entry data
            const char *buff = nullptr;
            size_t size = 0;
            la_int64_t offset = 0;

            for (;;) {
                auto ret = archive_read_data_block(a, (const void **) &buff, &size, &offset);
                if (ret == ARCHIVE_EOF)
                    break;
                if (ret < ARCHIVE_OK)
                    throw std::runtime_error("Failed to read archive data: " + std::string(archive_error_string(a)));

                // Copy data to archive
                auto prevSize = e.size();
                e.resize(prevSize + size);
                for (auto i = 0; i < size; i++) {
                    e.at(prevSize + i) = buff[i];
                }
            }
        }
    }

    format = static_cast<Archive::Format>(archive_format(a));

    archive_read_close(a);
    archive_read_free(a);
}

Archive::Format Archive::getFormat() {
    return format;
}

void Archive::save(const std::string &outputFile, Archive::Format f) {
    struct archive *a = archive_write_new();

    archive_write_set_format(a, f);
    auto ret = archive_write_open_filename(a, outputFile.c_str());
    if (ret < ARCHIVE_OK)
        throw std::runtime_error("Failed to open output file " + std::string(archive_error_string(a)));

    for (auto &entry: mEntries) {
        auto *e = archive_entry_new();

        archive_entry_set_pathname(e, entry.first.c_str());
        archive_entry_set_filetype(e, AE_IFREG);
        archive_entry_set_perm(e, 0644);
        archive_entry_set_size(e, (long) entry.second.size());

        ret = archive_write_header(a, e);
        if (ret < ARCHIVE_OK)
            throw std::runtime_error("Failed to write archive header " + std::string(archive_error_string(a)));

        auto writeCount = archive_write_data(a, entry.second.data(), entry.second.size());
        if (writeCount < 0)
            throw std::runtime_error("Failed to write archive data " + std::string(archive_error_string(a)));

        archive_write_finish_entry(a);

        archive_entry_free(e);
    }

    archive_write_close(a);
    archive_write_free(a);
}