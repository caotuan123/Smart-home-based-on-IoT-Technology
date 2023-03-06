#ifndef MANI_H_
#define MANI_H_

#include "Arduino.h"
#include "painlessMesh.h"
#include "DHT.h"
#include <stdlib.h>
#include <stdio.h>
#include "string.h"
#include "update_gen.h"

#define HOST_ID 1305484227
#define LIGHT_PIN LED_BUILTIN

#define MESH_PREFIX "Mesh_username"
#define MESH_PASSWORD "mesh_password"
#define MESH_PORT 5555
// https://thestempedia.com/tutorials/interfacing-mq-2-gas-sensor-with-evive/
#define M_H2 -0.47305447
#define B_H2 1.412572126
#define M_LPG -0.454838059
#define B_LPG 1.25063406
#define M_METHANE -0.372003751
#define B_METHANE 1.349158571
#define M_CO -0.33975668
#define B_CO 1.512022272
#define M_ALCOHOL -0.373311285
#define B_ALCOHOL 1.310286169
#define M_SMOKE -0.44340257
#define B_SMOKE 1.617856412
#define M_PROPANE -0.461038681
#define B_PROPANE 1.290828982
#define R0_GAZ 1.3151 // 0.438828

#define INDEX_TEMP 4
#define INDEX_HUM 5
#define INDEX_HIC 6
#define INDEX_GAS_PPM 7

#define PERIOD 5000

int temp_counter = 0;
int hum_counter = 3;
int hic_counter = 6;
int ppm_counter = 9;
void dataColector(void);
painlessMesh mesh;
Scheduler userScheduler;
Task colectData(PERIOD *TASK_MILLISECOND, TASK_FOREVER, &dataColector, &userScheduler, false);

#endif