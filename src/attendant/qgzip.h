#pragma once

#include <zlib.h>
#include <QByteArray>

#define GZIP_WINDOWS_BIT 15 + 16
#define GZIP_CHUNK_SIZE 32 * 1024

class qgzip_t
{
public:
	static bool compress(const QByteArray& input, QByteArray& output, int level = -1);
	static bool decompress(const QByteArray& input, QByteArray& output);
};
