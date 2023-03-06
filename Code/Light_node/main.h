#ifndef MANI_H_
#define MANI_H_

#include "Arduino.h"
#include "painlessMesh.h"
#include "light.h"
#include "fifo_string.h"

#define LIGHT_PIN LED_BUILTIN
#define MESH_PREFIX "Mesh_username"
#define MESH_PASSWORD "mesh_password"
#define MESH_PORT 5555

extern painlessMesh mesh;

Scheduler userScheduler;
Light light0(4, 0, "light_0");
Light light1(5, 1, "light_1");
FifoString fifo(10);

#endif