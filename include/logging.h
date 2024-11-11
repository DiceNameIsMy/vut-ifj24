//
// Created by nur on 22.9.24.
//

#ifndef LOGGING_H
#define LOGGING_H

#include <stdio.h>

#ifndef BASE_PATH
#define BASE_PATH ""
#endif

#ifdef DEBUG
#define loginfo( s, ... ) \
fprintf( stderr, "[INF] %s:%u: " s "\n", \
         strstr(__FILE__, BASE_PATH) ? strstr(__FILE__, BASE_PATH) + sizeof(BASE_PATH) - 1 : __FILE__, \
         __LINE__, ##__VA_ARGS__ )
#else
#define loginfo( s, ... )
#endif

#endif //LOGGING_H
