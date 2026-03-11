#ifndef __PHYSICS_CIRCLE_H
#define __PHYSICS_CIRCLE_H
#include "position.h"
#include "velocity.h"

struct Circle {
    float               radius;
    float               mass;
    struct Velocity     velocity;
    struct Position     position;
};

#endif // __PHYSICS_CIRCLE_H
