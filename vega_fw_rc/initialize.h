void generate_arccos_lut() //Ez a függvény generálja le a LUT-ot.
{
   Serial.print(">Az arccos LUT generálása.....");
   float arccos_value = 0.0;
   int i = 0;
   while(i < LUT_SIZE)
    {
      arccos_table[i] = (((acos(arccos_value) * RAD_TO_DEG) * SERVO_DEG) + SERVO_ZERO);
      arccos_value += DEG_RES;
      ++i;
    }
   Serial.print("OK.");
}

void initialize()
{
  Serial.print(">Initialize servo motors...");
  for(int i = 0; i < 4; ++i)
   {
      legs[i][0].attach(servo_pins[i * 3]); //Térd-rész minden láb estében a 0. indexű elem.
      legs[i][1].attach(servo_pins[i * 3 + 1]); //Comb-rész minden láb estében a 1. indexű elem.
      legs[i][2].attach(servo_pins[i * 3 + 2]); //Csípő-rész minden láb estében a 2. indexű elem.
      Serial.print("...");
   }
   Serial.println("Done.");
   Serial.print(">Calculating initial position...");
  for(int i = 0; i < 4; ++i)
   {
      ik_solver(i, 0.0,REST_H,0.0); // Alaphelyzet értékek számítása!
      Serial.print("...");
   }
   for(int i = 0; i < 4; ++i) // Alaphelyzet értékek másolása az aktuális pozíciókba.
   {
      act_pos[i][0] = new_pos[i][0];
      act_pos[i][1] = new_pos[i][1];
      act_pos[i][2] = new_pos[i][2];
      Serial.print("...");
   }
   Serial.println("Done.");
   Serial.print(">Setting up initial position...");
   for(int i = 0; i < 4; ++i) // Alaphelyzet beállítása.
   {
       legs[i][0].write(act_pos[i][0]);
       legs[i][1].write(act_pos[i][1]);
       legs[i][2].write(act_pos[i][2]);
       Serial.print("...");
   }
   Serial.println("Done.");
}
  
void precalc_step_arc(float step_dist)
{
  Serial.print(">Calculating the trajectory of steps");
  for(int i = 0; i <= step_resolution; ++i)
  {
    step_arc[i][0] = step_x_rat*((step_dist/2) - ((step_dist/2) * (cos((i)*(PI/step_resolution)))));
    step_arc[i][1] = step_y_rat*(abs((step_dist) * (sin((i)*(PI/step_resolution)))));
    Serial.print("...");
  }
  Serial.println("Done.");
  return;
}

void precalc_turn_line(float turn_dist)
{
  Serial.print(">Calculating the trajectory of turning steps");
  for(int i = 0; i <= step_resolution; ++i)
  {
    turn_line[i][0] = turn_z_rat*((turn_dist/2) - ((turn_dist/2) * (cos((i)*(PI/step_resolution)))));
    turn_line[i][1] = turn_y_rat*(abs((turn_dist) * (sin((i)*(PI/step_resolution)))));
    Serial.print("...");
  }
  Serial.println("Done.");
  return;
}
