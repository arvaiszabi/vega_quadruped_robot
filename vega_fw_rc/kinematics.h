void ik_solver( int leg_num, float x, float y, float z) //Inverz kinematikát számoló függvény
{
  float c, alpha, beta, theta_fem, theta_tib_fem, theta_tib_fem_a, theta_cox, theta_tib;
  theta_cox = atan2(y, z); //Csípő ízületet először kell számolni, mert utána változhat az 'y', ha 'z' nem 0. 
  if(z != 0)
    { 
      if(z > 0)
        z += ROT_CORR;
      else
        z -= ROT_CORR;
      z -= (ROT_CORR * cos(1.57 - theta_cox));
      y += ROT_CORR * sin(1.57 - theta_cox);
      y = sqrt(sq(z) + sq(y));
    }
  c = sqrt(sq(x) + sq(y));
  alpha = atan2(y, x);
  beta = sq(a) + sq(c) - sq(b);
  beta /= 2 * a * c;
  beta = acos(beta); 
  theta_fem = alpha + beta; // --> szervo irányától függ
  theta_tib_fem = sq(a) + sq(b) - sq(c);
  theta_tib_fem /= 2 * a * b;
  theta_tib_fem = acos(theta_tib_fem);
  theta_tib_fem_a = M_PI - theta_tib_fem;
  theta_tib = theta_tib_fem - (M_PI-theta_fem);
  if(!isnan(theta_fem) && !isnan(theta_cox) && !isnan(theta_tib)) //Van hibás/elérhetetlen érték? NaN? Ha nincs, akkor kerülnek eltárolásra az új értékek!
  {
    if(leg_num == LR)
      {
        new_pos[leg_num][0] = theta_tib * RAD_TO_DEG + LR_TIB_OFFSET;
        new_pos[leg_num][1] = theta_fem * RAD_TO_DEG + LR_FEM_OFFSET;
        new_pos[leg_num][2] = ((180 - theta_cox * RAD_TO_DEG) + LR_COX_OFFSET);
      }
    else if(leg_num == RF)
      {
        new_pos[leg_num][0] = 180 - theta_tib * RAD_TO_DEG + RF_TIB_OFFSET;
        new_pos[leg_num][1] = 180 - theta_fem * RAD_TO_DEG + RF_FEM_OFFSET;
        new_pos[leg_num][2] = (180 - theta_cox * RAD_TO_DEG) + RF_COX_OFFSET;
      }           
    else if(leg_num == RR)
      {
        new_pos[leg_num][0] = 180 - theta_tib * RAD_TO_DEG + RR_TIB_OFFSET;
        new_pos[leg_num][1] = 180 - theta_fem * RAD_TO_DEG + RR_FEM_OFFSET; 
        new_pos[leg_num][2] = 180 - theta_cox * RAD_TO_DEG + RR_COX_OFFSET;
      }          
    else if(leg_num == LF)
      {
        new_pos[leg_num][0] = theta_tib * RAD_TO_DEG + LF_TIB_OFFSET;
        new_pos[leg_num][1] = theta_fem * RAD_TO_DEG + LF_FEM_OFFSET;
        new_pos[leg_num][2] = theta_cox * RAD_TO_DEG + LF_COX_OFFSET;
      }
   }
  else
  Serial.println("Warning! Destination unreachable!");
}

void ik_engine() //Szervo pozíciókat állítja be!
{
    for(int m = 0; m < 4; ++m)
      {
        legs[m][0].write(new_pos[m][0]);
        delay(engine_delay);
        legs[m][1].write(new_pos[m][1]);
        legs[m][2].write(new_pos[m][2]);
      }
} 

void adj_height(float new_height)
{
  while(height != new_height)
  {
  if(new_height > height)
      {
        height += 5.0;
        for(int i = 0; i < 4; ++i)
            ik_solver(i, 0.0, height, 0.0);
      }
  else if(new_height < height)
      {
        height -= 5.0;
        for(int i = 0; i < 4; ++i)
            ik_solver(i, 0.0, height, 0.0);
      }
  ik_engine();
  delay(mot_delay);
  }
}

void gait_algorithm()
{
  while(1)
  {
  for(int i = 0; i <= step_resolution; ++i)
  {
    ik_solver(LF, step_arc[i][0], height - step_arc[i][1], 0.0);
    ik_solver(RR, step_arc[i][0], height - step_arc[i][1], 0.0);
    if(!step_start)
    {
    ik_solver(LR, step_dist-step_arc[i][0], height, 0.0);
    ik_solver(RF, step_dist-step_arc[i][0], height, 0.0);
    }
    ik_engine();
    delay(mot_delay);
    }
  delay(stage_delay);
  step_start = false;
  for(int i = 0; i <= step_resolution; ++i)
  {
    ik_solver(LR, step_arc[i][0], height - step_arc[i][1], 0.0);
    ik_solver(RF, step_arc[i][0], height - step_arc[i][1], 0.0);
    ik_solver(LF, step_dist-step_arc[i][0] , height, 0.0);
    ik_solver(RR, step_dist-step_arc[i][0] , height, 0.0);
    ik_engine();
    delay(mot_delay);
    }
  delay(stage_delay);
  if(walk_stop)
  {
    for(int i = 0; i <= step_resolution; ++i)
    {
      ik_solver(LR, step_dist-step_arc[i][0] , height - step_arc[i][1], 0.0);
      ik_solver(RF, step_dist-step_arc[i][0] , height - step_arc[i][1], 0.0);
      ik_engine();
      delay(mot_delay);
      }
    walk_stop = false;
    step_start= true;
    break;
    }
  check_distance();
  if(Serial.available()>0)
  {
    walk_stop = true;
  } 
    }
}

void b_gait_algorithm()
{  
  while(1)
  {
  for(int i = 0; i <= STEP_RES; ++i)
  {
    ik_solver(LF,  -step_arc[i][0], height - step_arc[i][1], 0.0);
    ik_solver(RR,  -step_arc[i][0], height - step_arc[i][1], 0.0);
    if(!step_start)
    {
    ik_solver(LR, -step_dist+step_arc[i][0], height, 0.0);
    ik_solver(RF, -step_dist+step_arc[i][0], height, 0.0);
    delay(mot_delay);
    }
    ik_engine();
    
  }
  delay(stage_delay);
  step_start = false;
  for(int i = 0; i <= STEP_RES; ++i)
  {
    ik_solver(LR, -step_arc[i][0], height - step_arc[i][1], 0.0);
    ik_solver(RF, -step_arc[i][0], height - step_arc[i][1], 0.0);
    ik_solver(LF, -step_dist+step_arc[i][0] , height, 0.0);
    ik_solver(RR, -step_dist+step_arc[i][0] , height, 0.0);
    ik_engine();
    delay(mot_delay);
    }
    delay(stage_delay);
  if(walk_stop)
  {
    for(int i = 0; i <= STEP_RES; ++i)
    {
      ik_solver(LR, -step_dist +  step_arc[i][0], height - step_arc[i][1], 0.0);
      ik_solver(RF, -step_dist +  step_arc[i][0], height - step_arc[i][1], 0.0);
      ik_engine();
      delay(mot_delay);
    }
    walk_stop = false;
    step_start= true;
    break;
    }
    check_distance();
    if(Serial.available()>0)
    {
      walk_stop = true;
    }
  } 
}

void turning_left()
{
  while(1)
  {
  for(int i = 0; i <= STEP_RES; ++i)
  {
    ik_solver(LF, 0.0, height - turn_line[i][1], -turn_line[i][0]);
    ik_solver(RR, 0.0, height - turn_line[i][1],  turn_line[i][0]);
    ik_engine();
    delay(mot_delay);
  }
  delay(stage_delay);
  for(int i = 0; i <= STEP_RES; ++i)
  {
    ik_solver(RF, 0.0, height - turn_line[i][1], turn_line[i][0]);
    ik_solver(LR, 0.0, height - turn_line[i][1], turn_line[i][0]);
    ik_engine();
    delay(mot_delay);
    }
   delay(stage_delay);
  for(int i = 0; i <= STEP_RES; ++i)
  {
    ik_solver(RF, 0.0, height, turn_dist - turn_line[i][0]);
    ik_solver(LR, 0.0, height, turn_dist - turn_line[i][0]);
    ik_solver(LF, 0.0, height, -turn_dist + turn_line[i][0]);
    ik_solver(RR, 0.0, height, turn_dist - turn_line[i][0]);
    ik_engine();
    delay(mot_delay);
    }
  if(walk_stop)
  {
    walk_stop = false;
    break;
    }
    if(Serial.available()>0)
    {
      walk_stop = true;
    }
  delay(stage_delay);
  }
}


void turning_right()
{
  while(1)
  {
  for(int i = 0; i < STEP_RES; ++i)
  {
    ik_solver(RF, 0.0, height - turn_line[i][1], -turn_line[i][0]);
    ik_solver(LR, 0.0, height - turn_line[i][1], -turn_line[i][0]);
    ik_engine();
    delay(mot_delay);
  }
  for(int i = 0; i < STEP_RES; ++i)
  {
    ik_solver(RR, 0.0, height - turn_line[i][1], -turn_line[i][0]);
    ik_solver(LF, 0.0, height - turn_line[i][1], turn_line[i][0]);
    ik_engine();
    delay(mot_delay);
    }
  for(int i = 0; i < STEP_RES; ++i)
  {
    ik_solver(RF, 0.0, height, -2*turn_dist + turn_line[i][0]);
    ik_solver(LR, 0.0, height, -2*turn_dist + turn_line[i][0]);
    ik_solver(LF, 0.0, height, 2*turn_dist - turn_line[i][0]);
    ik_solver(RR, 0.0, height,  -2*turn_dist + turn_line[i][0]);
    ik_engine();
    delay(mot_delay);
    }
  if(walk_stop)
  {
    walk_stop = false;
    break;
    }
    if(Serial.available()>0)
    {
      walk_stop = true;
    }
  }
}

void sidle_left()
{
  while(1)
  {
  for(int i = 0; i < STEP_RES; ++i)
  {
    ik_solver(LF, 0.0, height - turn_line[i][1], -turn_line[i][0]);
    ik_solver(RR, 0.0, height - turn_line[i][1], -turn_line[i][0]);
    ik_engine();
    delay(mot_delay);
  }
  for(int i = 0; i < STEP_RES; ++i)
  {
    ik_solver(LF, 0.0, height, -2*turn_dist+turn_line[i][0]);
    ik_solver(RR, 0.0, height, -2*turn_dist+turn_line[i][0]);
    ik_solver(RF, 0.0, height, -turn_line[i][0]);
    ik_solver(LR, 0.0, height, turn_line[i][0]);
    ik_engine();
    delay(mot_delay);
    }
  for(int i = 0; i < STEP_RES; ++i)
  {
    ik_solver(RF, 0.0, height - turn_line[i][1], -2*turn_dist + turn_line[i][0]);
    ik_solver(LR, 0.0, height - turn_line[i][1], 2*turn_dist - turn_line[i][0]);
    ik_engine();
    delay(mot_delay);
    }
  if(walk_stop)
  {
    walk_stop = false;
    break;
    }
  if(Serial.available()>0)
  {
    walk_stop = true;
    }
  }
}

void sidle_right()
{
  while(1)
  {
  for(int i = 0; i < STEP_RES; ++i)
  {
    ik_solver(RF, 0.0, height - turn_line[i][1], -turn_line[i][0]);
    ik_solver(LR, 0.0, height - turn_line[i][1], turn_line[i][0]);
    ik_engine();
    delay(mot_delay);
  }
  for(int i = 0; i < STEP_RES; ++i)
  {
    ik_solver(LF, 0.0, height, -turn_line[i][0]);
    ik_solver(RR, 0.0, height, -turn_line[i][0]);
    ik_solver(RF, 0.0, height, -2*turn_dist+turn_line[i][0]);
    ik_solver(LR, 0.0, height,  2*turn_dist-turn_line[i][0]);
    ik_engine();
    delay(mot_delay);
    }
  for(int i = 0; i < STEP_RES; ++i)
  {
    ik_solver(LF, 0.0, height - turn_line[i][1], -2*turn_dist + turn_line[i][0]);
    ik_solver(RR, 0.0, height - turn_line[i][1], -2*turn_dist + turn_line[i][0]);
    ik_engine();
    delay(mot_delay);
  }
  if(walk_stop)
  {
    walk_stop = false;
    break;
    }
  if(Serial.available()>0)
  {
    walk_stop = true;
    }
  }
}
