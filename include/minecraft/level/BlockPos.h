#pragma once

class Vec3;
struct BlockPos {

    int x, y, z;
    BlockPos(Vec3 const&);
    BlockPos(float,float,float);
};