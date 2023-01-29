/*********************************************************************************************************************
 * Arduino Quadruped Robot / VEGA /                                                                                  *
 * Created by Árvai Szabolcs                                                                                         *
 * 2022-09-04  v1.00 RC                                                                                              * 
 * Platform: Raspberry Pico / Arduino PRO Mini / Arduino UNO (prototype)                                             *
 *********************************************************************************************************************/
#include <Servo.h>
#include <math.h>
#include "constants.h"
#include "global_var.h"
#include "sensor.h"
#include "kinematics.h"
#include "initialize.h"
#include "vega_core_ui.h"


void setup() 
{
  Serial.begin(115200);
  initialize();
  precalc_step_arc(step_dist);
  precalc_turn_line(turn_dist);
  height = REST_H;
  Serial.println(">System ready! Waiting for commands!"); 
}

void loop() 
{
  delay(10);
  if(Serial.available() > 0)
  {   
    core_ui();
  }
  telemetry_timer++;
  if (TELEMETRY_WRITE == telemetry_timer)
  {
    check_battery();
    check_distance();
    telemetry_timer = 0;  
  }
}
