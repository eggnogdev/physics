#ifndef __PHYSICS_CIRCLE_H
#define __PHYSICS_CIRCLE_H
#include "array.h"
#include "linalg.h"
#include "position.h"
#include "velocity.h"

struct Circle {
    float               radius;
    float               mass;
    struct Velocity     velocity;
    struct Position     position;
};

DEFINE_DYNAMIC_ARRAY(struct Circle, struct_Circle_array);

// Fill a buffer of floats to represent the positions of all struct Circle in `*a`.
//
// This function assumes the buffer is large enough to fit all positions.
void dynamic_struct_Circle_array_to_position_buffer(struct dynamic_struct_Circle_array *a, float *out_buffer) {
    size_t count = a->length;
    for (size_t i = 0; i < count; i++) {
        size_t buffer_i = i * 2;
        out_buffer[buffer_i] = a->elements[i].position.x;
        out_buffer[buffer_i + 1] = a->elements[i].position.y;
    }
}

#endif // __PHYSICS_CIRCLE_H
