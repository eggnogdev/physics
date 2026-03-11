#ifndef __PHYSICS_CIRCLE_H
#define __PHYSICS_CIRCLE_H
#include "array.h"
#include "position.h"
#include "velocity.h"

struct Circle {
    float               radius;
    float               mass;
    struct Velocity     velocity;
    struct Position     position;
};

DEFINE_DYNAMIC_ARRAY(struct Circle, struct_Circle_array);

#endif // __PHYSICS_CIRCLE_H
