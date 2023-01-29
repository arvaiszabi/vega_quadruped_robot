//-------------------------------------------------------------------------------
//Adatok a trigonometrikus számításokhoz
//-------------------------------------------------------------------------------
#define FEMUR_L     100.0
#define MIN_FEMUR_L 100.0
#define TIBIA_L     115.0
#define MIN_TIBIA_L 115.0
#define MAX_TIBIA_L 125.0
#define STAND_H     140.0
#define REST_H      80.0
#define ROT_CORR    24.7 //12.0
#define LF          0 //Lábak hivatkozásai--> 0,2 bal oldal; 1,3 jobb oldal
#define LR          2
#define RF          1
#define RR          3
#define MAX_STP_RES 10 //Lépések mozgáspálya pontjainak maximális száma.
#define STEP_RES    5 //Lépések 'felbontása' - hány pontból áll a mozgáspálya ---> a kezdeti értéke.

#define LF_COX_OFFSET    -4
#define LF_TIB_OFFSET     9
#define LF_FEM_OFFSET     5
#define LR_COX_OFFSET    -2
#define LR_TIB_OFFSET     8
#define LR_FEM_OFFSET     5
#define RF_COX_OFFSET     2
#define RF_TIB_OFFSET    -9 //-7 ezt mértem,de a magasság ezt adja vissza, ekkor jó
#define RF_FEM_OFFSET     5 //2
#define RR_COX_OFFSET     1
#define RR_FEM_OFFSET     7
#define RR_TIB_OFFSET    -3
//-------------------------------------------------------------------------------
//Műveleti kódok - később bővíthető
//-------------------------------------------------------------------------------
#define FORWARD     "forw"
#define BACKWARD    "back"
#define LEFT        "left"
#define RIGHT       "rigt"
#define STOP        "stop"
#define STEP_ADJ    "stpd"
#define HEIGHT_ADJ  "adjh"
#define TURN_ADJ    "trnd"
#define BATTERY_CH  "batc"
#define BARRIER_CH  "barc"
#define LIGHT_CH    "ligc"
#define REST_STATE  "rest"
#define SET_DELAY   "dely"
#define SET_STDELAY "sdel"
#define SET_ENGINE  "edel"
#define SIDLE_LEFT  "sidl"
#define SIDLE_RIGHT "sidr"
#define SET_FEMUR   "feml"
#define SET_TIBIA   "tibl"
#define SET_STP_RES "sres"
#define SET_STP_X   "stpx"
#define SET_STP_Y   "stpy"
#define SET_TURN_Z  "trnz"
#define SET_TURN_Y  "trny"
//-------------------------------------------------------------------------------
//Az acos tábla adatai, át kell írni a konstansokat szükség esetén!
//-------------------------------------------------------------------------------
#define DEG_RES     0.01 // tábla felbontása
#define LUT_SIZE    101  // 1 / felbontás + 1 --> 0 érték miatt
#define MUL_TO_LUT  100  // 1 / felbontás
//-------------------------------------------------------------------------------
//Szervo motor hasznos adatok későbbiekhez.
//-------------------------------------------------------------------------------
#define SERVO_DEG   11.111 // * msec = 1 fok
#define SERVO_ZERO  500    // msec = 0 fok
#define SERVO_180   2500   // msec = 180 fok
#define SERVO_90    1500   // msec = 90 fok
//-------------------------------------------------------------------------------
//Szenzorok adatai
//-------------------------------------------------------------------------------
#define DIST_SNSR         A6  //Távolságmérő ADC pin
#define LIGT_SNSR         A7  //Fotoellenállás ADC pin
#define LIGT_RD_COUNT      5  //Kiolvasások száma a fotoellenállásnál
#define REF_DIST_VALUE   555  //Referencia érték ADC-érték
#define REF_DIST          10  //Referencia távolság cm-ben
#define DIST_RD_COUNT      5  //Szenzor érték kiolvasások száma
#define BAT_SNSR          A0  //Akkumululátor feszültségosztó ADC pin
#define REF_BAT       4860.0  //Referencia feszültség /névleges 5V --> 4860mV/
#define REF_BAT_VALUE  164.0  //Referencia érték 4860mV-hoz
#define CELL_FULL     4200.0  //Cella feszültsége teljes töltöttség esetén (mV)
#define CELL_DEPL     3200.0  //Lemerült cella feszültsége (mV)
#define CELL_COUNT         4  //Hány cellából áll az akkupakk? (darab cella)
#define FULL_CHARGE  (CELL_COUNT * CELL_FULL)  //Akkumulátor-pack feszültsége teljes töltöttség esetén (mV)
#define DEPL_CHARGE  (CELL_COUNT * CELL_DEPL)  //Lemerült akkumulátor-pack feszültsége (mV)
#define CHARGE_REF   (FULL_CHARGE - DEPL_CHARGE) //Lemerült teljesen feltöltött akkumulátor-pack feszültség különbsége (mV)
#define BAT_RD_COUNT       5  //Kiolvasások száma
#define TELEMETRY_WRITE 1000 
