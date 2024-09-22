//
// Created by nur on 22.9.24.
//

#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>

#ifdef DEBUG
#define loginfo( s, ... ) \
fprintf( stderr, "[INF] " __FILE__ ":%u: " s "\n", __LINE__, ##__VA_ARGS__ )
#else
#define loginfo( s, ... )
#endif

#endif //LOGGING_H
