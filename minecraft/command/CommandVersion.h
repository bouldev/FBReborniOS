#pragma once

/*class CommandVersion {

private:
    int i, i2;

public:
    CommandVersion(int, int);

};*/

class CommandVersion {
public:
  int minVersion, maxVersion;
  inline CommandVersion() : minVersion(0), maxVersion(0x7FFFFFFF) {}
  inline CommandVersion(int minVersion, int maxVersion) : minVersion(minVersion), maxVersion(maxVersion) {}
};