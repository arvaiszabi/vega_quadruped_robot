float read_float()
{
 String serial_data;
 while(!Serial.available()); // Várakozás érték bevitelére.
 serial_data = Serial.readStringUntil('\n');
 float number = serial_data.toFloat();
 Serial.println(number);
 return number;
}
int read_int()
{
 String serial_data;
 while(!Serial.available());
 serial_data = Serial.readStringUntil('\n');
 int number = serial_data.toInt();
 Serial.println(number);
 return number;
}
void core_ui()
{
  String instruction = Serial.readStringUntil('\n'); //Bevitt szöveg beolvasása.
  if (instruction == FORWARD)
  {
    Serial.println(">Gait launched!");
    gait_algorithm();
  }
  else if (instruction == BACKWARD)
  {
    Serial.println(">Backward gait launched!");
    b_gait_algorithm(); 
  }
  else if (instruction == LEFT)
  {
    Serial.println(">Turning left!");
    turning_left();
  }
  else if (instruction == RIGHT)
  {
    Serial.println(">Turning right!");
    turning_right();
  }
  else if (instruction == STOP)
  {
    Serial.println(">Stopped!");
  }
  else if (instruction == HEIGHT_ADJ)
  {
    Serial.println(">New height:");
    new_height = read_float();
    adj_height(new_height);
    Serial.println(">New height has set!");
  }
  else if (instruction == STEP_ADJ)
  {
    Serial.println(">New step size:");
    step_dist = read_float();
    precalc_step_arc(step_dist);
    Serial.println(">New step distance has set!");
  }
  else if (instruction == TURN_ADJ)
  {
    Serial.println(">New turn step size:");
    turn_dist = read_float();
    precalc_turn_line(turn_dist);
    Serial.println(">New turning step distance has set!");      
  }
  else if (instruction == SET_STP_RES)
  {
    Serial.println(">Enter number of points along step path:");
    while(1)
    {
      int num_points = read_int();
      if (num_points <= MAX_STP_RES && num_points >= 3)
      {
        step_resolution = num_points; 
        precalc_turn_line(turn_dist);
        precalc_step_arc(step_dist);
        Serial.println(">New step paths have set!");
        break;
      }
      Serial.println("Please enter a value between 3 and 10!");
    }     
  }
  else if (instruction == SET_TIBIA)
  {
    Serial.println(">Enter length of tibia:");
    while(1)
    {
      float tib_length = read_float();
      if (tib_length >= MIN_TIBIA_L && tib_length <= MAX_TIBIA_L)
      {
        b = tib_length;
        Serial.println(">New tibia length has set!");
        break;
      }
      Serial.println("Please enter a value between 115.0 and 125.0!");
    }     
  }
  else if (instruction == BATTERY_CH)
  {
    check_battery();
  }
  else if (instruction == BARRIER_CH)
  {
    check_distance();
  }
  else if (instruction == LIGHT_CH)
  {
    check_light();
  }
  else if (instruction == SET_DELAY)
  {
    Serial.println(">New motion delay:");
    int temp = read_int();
    if (temp >= 0)
    {
      mot_delay = temp;
      Serial.println(">New delay has set!");
    }
  }
  else if (instruction == SET_ENGINE)
  {
    Serial.println(">New motion delay:");
    int temp = read_int();
    if (temp >= 0)
    {
      engine_delay = temp;
      Serial.println(">New delay has set!");
    }
  }
  else if (instruction == SET_STDELAY)
  {
    Serial.println(">New motion delay:");
    int temp = read_int();
    if (temp >= 0)
    {
      stage_delay = temp;
      Serial.println(">New delay has set!");
    }
  }
  else if (instruction == REST_STATE)
  {
    adj_height(REST_H);
    Serial.println(">Rest state has set!");
  }
  else if (instruction == SIDLE_LEFT)
  {
    Serial.println(">Sidling left...!");
    sidle_left();
  }
  else if (instruction == SIDLE_RIGHT)
  {
    Serial.println(">Sidling right...!");
    sidle_right();
  }
  else if (instruction == SET_STP_X)
  {
    //Serial.println(">New STEP_X ratio:");
    float stp_x = read_float();
    if(stp_x <= 3.0 && stp_x > 0.0)
      step_x_rat = stp_x;
    //Serial.println(">New STEP_X has set!");
  }
  else if (instruction == SET_STP_Y)
  {
    //Serial.println(">New STEP_Y ratio:");
    float stp_y = read_float();
    if(stp_y <= 3.0 && stp_y > 0)
      step_y_rat = stp_y; 
    //Serial.println(">New STEP_Y has set!");
  }
  else if (instruction == SET_TURN_Z)
  {
    //Serial.println(">New TURN_Z ratio:");
    float stp_t_z = read_float();
    if(stp_t_z <= 3.0 && stp_t_z > 0.0)
      turn_z_rat = stp_t_z;
    //Serial.println(">New TURN_Z has set!");
  }
  else if (instruction == SET_TURN_Y)
  {
    //Serial.println(">New TURN_Y ratio:");
    float stp_t_y = read_float();
    if(stp_t_y <= 3.0 && stp_t_y > 0)
      turn_y_rat = stp_t_y;
    //Serial.println(">New TURN_Y has set!");
  }
  else 
  {
    Serial.println(">Invalid or bad command!");
  }     
}
