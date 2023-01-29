short arccos_table[LUT_SIZE];

float a = FEMUR_L;
float b = TIBIA_L;

float height; // Robot magasságának állításához.
float new_height;
float step_x_rat = 1.0;
float step_y_rat = 2.5;
float turn_z_rat = 1.0;
float turn_y_rat = 2.5;
int step_resolution = STEP_RES;
float step_arc[MAX_STP_RES + 1][2];
float turn_line[MAX_STP_RES + 1][2];

String distance = "dis";
String battery = "bat";
String light = "lit";

int telemetry_timer = 0; 
int act_pos[4][3]; // Aktuális helyzet, ezen értékek változnak mozgáskor, lásd ik_engine() függvényt.
int new_pos[4][3]; // Új testhelyzet számított értékei.
int offsets[4][3] = {{LF_TIB_OFFSET, LF_FEM_OFFSET, LF_COX_OFFSET},
                     {RF_TIB_OFFSET, RF_FEM_OFFSET, RF_COX_OFFSET},
                     {LR_TIB_OFFSET, LR_FEM_OFFSET, LR_COX_OFFSET},
                     {RR_TIB_OFFSET, RR_FEM_OFFSET, RR_COX_OFFSET},};

bool step_start = true;
bool walk_stop = false;
float step_dist = 20.0;
float turn_dist = 5.0;
int mot_delay = 10;
int stage_delay = 250;
int engine_delay = 2;

//Különböző fejlesztőpanelek pin-kiosztásai, megfelelő sor elől a komment jelet ki kell venni!
//Pinek sorrendje az inicializáló függvénynek megfelelően vannak.
//          -->0T,0F,0C,1T,1F,1C,2T,2F,2C,3T,3F,3C<--
//Raspberry Pi Pico
//int servo_pins[]= {17,18,19,12,11,10,20,21,22,9,8,0};
//Arduino PRO Mini
int servo_pins[]= {2,3,4,13,12,11,5,6,7,10,8,9};
//Arduino Uno
//int servo_pins[]= {2,3,4,5,6,7,8,9,10,11,12,13};

Servo legs[4][3]; //Szervo motorok tömbje. 4 láb, 3 motor, tehát 4x3 elrendezés.
