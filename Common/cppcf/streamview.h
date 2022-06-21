#pragma once

#include <iostream>
#include <list>
#include "sector.h"
#include <queue>
#include "svector.h"

namespace CFCPP
{
class StreamView : public std::iostream
{
public:
    StreamView(const SVector<Sector> &sectorChain, int sectorSize, const Stream &stream);
    StreamView(const SVector<Sector> &sectorChain, int sectorSize, std::streamsize length,
               std::queue<Sector> &availableSectors, const Stream stream, bool isFatStream = false);

    std::ostream& Write (const char *buffer, std::streamsize offset, std::streamsize count);
    std::streamsize Read(char *buffer, std::streamsize offset, std::streamsize count);
    std::streamsize Seek(std::streamsize offset, int origin);
    void SetLength(std::streamsize value);

    int ReadInt32();
    void WriteInt32(int val);

private:
    void adjustLength(std::streamsize value);
    void adjustLength(std::streamsize value, std::queue<Sector> &availableSectors);
private:
    int sectorSize;
    std::streamsize length;

    SVector<Sector> sectorChain;
    Stream stream;
    bool isFatStream = false;
    int buf = 0;

public:
    std::list<Sector> freeSectors;
    std::streamsize position;
};
}
