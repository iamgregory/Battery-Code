/*------------------------------------------------------------------------------
 * BME.h
 * Created by Nima Ghods
 * Data: 5/21/2012
 * setsup the constants and parameters for BME_XX.ino
 *----------------------------------------------------------------------------*/

// flag limits
  #define tempVCAlarm 60    //virtual cell temperature limit for temperature error 
  #define tempTiAlarm 75    // BME LTC chip temperature limit for temperature error 
  #define tempHSAlarm 120   //heat sink temperature limit for temperature error
  #define tempVCWarn 40    //virtual cell temperature limit for temperature warning 
  #define tempTiWarn 65    //BME LTC chip temperature limit for temperature warning
  #define tempHSWarn 110   //heat sink temperature limit for temperature warning
  
  #define volHighAlarm 4.205  //high virtual cell voltage limit for voltage error 
  #define volLowBalAlarm 3.7   // the voltage at whitch the system will not go in to balancing mode
  #define balRecLimit 3.9      // minimum voltage limit for recommending balancing
  #define volLowWarn 3.2       //low virtual cell voltage limit for voltage warning
  #define volLowAlarm 3.0      //low virtual cell voltage limit for voltage error 
  #define deadBatAlarm 2.5   // the voltage at whitch the system will not go in to charge mode
  #define volMismatch 5.0       //voltage mismatch limit between calculated and measured total voltage 
  #define volModMismatch 0.05   //voltage mismatch limit between calculated and measured total voltage 
 
  #define presHighLimit 3.5    //High pressure limit
  #define presLowLimit 1.5     //Low Pressure limit
  #define presRateHigh 1.0    //High pressure rate limit
  
  #define inCurLimit 20.0      //current in limit durring Drive
  #define highInCur 92.0      //high current in limit during Charging 
  #define highOutCur -2.0      //current out limit during Charging
  #define inOutCur 1.0          // current in or out limit durring stop and balance
  #define doneCur 4.45         //the current at which the charging is called done
  
  #define balRecVol 0.050       // voltage difference at which balancing will be recommended 
  #define volTolerance 0.01   // the max voltage difference that the vertual cells will have at the end of balancing 
  #define volBalStop -0.006   // voltage difference that the battery will stop balancing
  
  #define timeoutLimit 180000     //a timeout limit of 5*36000 sec==>10 hours for charging and balanceing 
  #define overrideTLimit 310        // a time limit for priority flag of 2 or 3 5*62==>62sec

//BMU ADC conversion constants
  #define curConst 0.08587        //80/V*3.3V/4095*4.01ohm/3.01ohm  sensor resulution*adc resulution*voltage divider
  #define volConst 0.04852  //.0439    //(174Kohm+10Kohm)/10Kohm*(10kohm+5.1kohm)/5.1kohm*3.3V/4095
  #define presConst 0.0061034   //1 kpa/5V/0.018*4.7ohm/3.2ohm*3.3V/4095*0.14503 gpsi/kpa
  #define presOffset 0.0058016     //0.04 kpa 0.14503 gpsi/kpa
  #define capConst 0.0000555556    //0.2 sec==>.000055556 hours

// BMU battery prameters
  #define BMENum 14          // number of BMEs attached to the BMU
  #define cellNum 3          // number of virtual cells attached to a BME
  #define BMEConDelay1 2300   // delay for BME ADC conversion commands
  #define BMEConDelay2 2000   // delay for BME ADC conversion commands
  #define BME_MD 2           //Set to:2 for 7kHz, 3 for 26Hz mode(filtered)   

// temperature reading prameters
  #define IntTempConst  0.013333  //0.1mV/7.5mV is the convertion from the reading to K for internal temperature
  #define Binv .00024777  // 1/ B-value where B-value is 4190 K
  #define T0inv 0.003354  // 1/T_0 where T_0 is 298.15 K
  #define VIN 5.0      // Temperature reference

// debugging variables
  boolean uartPrint=true;    // print for debugging
  String inputString="";
  
 // loop timing variables
 const long controlTime=200000;  // loop time in uSec  .2 s loops ==> 5Hz
 const float dt=controlTime/1000000.0;  // control time in sec
 long timeStamp=0;          // used to keep track of the loop time
 long dloopTime=0;         //actual loop time in usec
 long BMCcommdt=0;          // the time between bmc communications
 int loopCount=0;            // counts the number of loops up to the count Limit
 const int countLimit=1500;          //1500 at 5Hz is 5min
 const int bmeSelfTestTime=1500;    // set the self test to be done every 5 min
 const int bmcComTime=5;            // set the bmc communication to be done every 1 second (1Hz)
 
// Arduino due pins in use 
  const int tVolInPin = A3;  // Analog input pin for the total voltage
  const int presIn1Pin = A1;  // Analog pressure sensor pin
  const int curInPin = A4;  // Analog current sensor pin
  const int cur0InPin=A7;   // current sencor referance pin
  const int relay1 = 47;   // negative side relay or relay 1 pin
  const int relay2 = 49;   // positive side relay or realy 2 pin 
  const int frontWPin = 22;  //front water leak sensor
  const int backWPin = 23;    // back water leak sensor

// BMU measurement Variables
float totalVoltage =0;        // total half-string voltage read from ADC
float tVolOld =0;          // the last total voltage
float pressure =0;        // pressure sensor reading
float presOld =0;              // last pressure value
float presRate =0;          // filtered pressure rate
float current0 =0;         // current offset
float current =0;        // value read from the LEM sensor
float curOld =0;      // last current value
boolean fwLeak =0;         // front leak sensor
boolean bwLeak =0;         // back leak sensor

// BMU calculated Variables
float cap=435;            // battery capacity in Amp hours
const float capMax=435;  
float volSum=0.0;          // calculated sum of all virtual cells
float SOC=50;             // percent 0-100 state of charge
float maxTemp=0;          //  max temperature of all virtual cells
float minVol=7.0;        //  minimum voltage of virtual cells?
float maxVol=0.0;          // maximum voltage of virtual cells?
float balance2Vol=7.0;    // voltage to balance to
float charge2Vol=3.0;      //voltage to charge to
String BMCcommad="";      //The command from BMC
int conOnTime=0;          // counter to enable contactor

// BME communication PEC (packet error code) check table
int pec15Table[256];
int CRC15poly=0x4599;
int selfCheckVal1=0x9555;
int selfCheckVal2=0x6AAA;

// BME varibles
typedef struct  {
  byte addr;             //BME address
  int vol[cellNum];      // array of virtual cell voltages
  int vSum;              //measured  BME voltage
  int vref2;              
  int temp[4];           // temperature of: 0-2 = virtual cells, 3 =heatsink
  int iTemp;              // internal temperature of BME LTC84 chip
  float modSum;            //calculated BME voltage (sum of layers)
  float fVol[cellNum];    //float versions of everything
  float fVSum;
  float fVref2; 
  float fTemp[4];
  float fiTemp;
  boolean ignoreT[4];
  boolean ignoreiT;
  boolean uFlag[cellNum];
  boolean oFlag[cellNum];
  boolean balFlag[cellNum];
  boolean muxCheck;
  boolean dataCheck;
  boolean volSelfCheck;
  boolean AuxSelfCheck;
  boolean StatSelfCheck;
  int DCC;                  // 3 bit registry, each bit is a flag for balancing a corresponding virtual layer
  byte GPIO;                //5 Auxiliary GPIO ports
} BMEdata;                   //BME data struct 

BMEdata BME[BMENum];
boolean DCP=false;     //Discharge premitted

//BMU modes flags
boolean stopOn=true;
boolean driveOn=false;
boolean chargeOn=false;
boolean balanceOn=false;
boolean stopUntil=false;

//BMU flags
unsigned int flagBMU=0;         //all the BMU flags up to 32 flags
unsigned int flagOverride=0;         //the flags that the BMC wants to override
boolean flagIgnoreTemp=false;
byte flagPriority=0;             //the Priority of the active flag under the current mode

boolean leakFlag =false;         //Leak in front or back of battery bottle

boolean tempAlarmFlag =false;    //Any VC temp > 60 C, Heat Sink temp > 120 C, Internal chip temp>75 C
boolean tempWarnFlag =false;     //Any VC temp > 40 C, Heat Sink temp > 110 C, Internal chip temp>65 C
boolean tempFailFlag =false;     //Any defective/failed temperature sensor reading, when sensor disconnected or shorted out

boolean presRateFlag =false;     //Pressure rate > 1 PSI/sec
boolean presFlag =false;        // Pressure < 1.5 PSI or Pressure  > 2.5 PSI

boolean volHighAlarmFlag =false;      //Any VC voltage > 4.25 V 
boolean volLowBalAlarmFlag=false;    //Any VC voltage < 3.7 V
boolean volLowWarnFlag =false;     //Any VC voltage < 3.2 V
boolean volLowAlarmFlag =false;    //Any VC voltage < 3.0 V
boolean deadBatAlarmFlag=false;    //Any VC voltage < 2.5 V
boolean volFailFlag =false;      //Any VC voltage < .1 V or >6.5 or Vref2<2.978 or>3.020
boolean volMisFlag =false;      /*5V difference between overall half-string voltage and sum of half-string VC voltages or
                                     50mV difference between battery module voltage and sum of its VC voltages*/
boolean bmeAlarmFlag =false;     // If any cell over/under voltage failures or self-check failures are sent from BME 
boolean bmeComFlag =false;       // Communication failure occurs between BMU and BME
boolean bmcComFlag =false;       // Communication failure occurs between BMU and BMC

boolean driveCurflag=false;          //Current >20 durring Drive
boolean chargeCurFlag=false;      //Current > 92A or current <2A during Charge
boolean stopCurFlag=false;     //abs(Current)>1A

boolean timeoutFlag=false;      //Charging or balance time > 10 hours

boolean chargeDoneFalg =false;   // charging done flag
boolean balDoneFlag =false;      // balancing done flag
boolean balRecFlag =false;       // balancing recommended flag

boolean fanOn=false;
unsigned long overrideCount=0;
unsigned long timeoutCount=0;

//************************ SPI parameters *************************//
// pins used for the connection with the BME and BMC with SPI
const int chipSelectPin = 52;
const int csBME1 =2;
const int csBME2 =3;
const int ClockDivider=100;
const int MISOPin=50;

//************************ Ethernet parameters *************************//
// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte BMUNum=0;
byte Add0=5;
byte Add1=6;
byte Add2=7;
//byte mac[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xE1 };
byte mac[6] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xE1 };
byte ipadd[]={192,168,0, 170};
// the router's gateway address:
byte gateway[] = { 192, 168, 0, 1 };
// the subnet:
byte subnet[] = { 255, 255, 255, 0 };
int port=40;
//IPAddress ip(ipadd);
EthernetServer server(port);
// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):

//**************************** Voltage lookup tables ************************//

int lookUpSOC[1000]={1000,999,997,996,994,993,992,990,989,988,986,985,983,982,981,979,978,977,975,974,973,971,970,969,967,966,964,963,962,960,959,958,956,
955,954,952,951,950,948,947,946,944,943,942,940,939,938,936,935,933,932,931,929,928,927,925,924,923,921,920,919,917,916,915,913,912,911,909,908,907,905,904,
903,901,900,899,897,896,895,893,892,891,890,888,887,886,884,883,882,880,879,878,876,875,874,872,871,870,868,867,866,864,863,862,861,859,858,857,855,854,853,
851,850,849,848,846,845,844,842,841,840,838,837,836,834,833,832,831,829,828,827,825,824,823,821,820,819,818,816,815,814,812,811,810,809,807,806,805,803,802,
801,800,798,797,796,794,793,792,790,789,788,787,785,784,783,782,780,779,778,776,775,774,773,771,770,769,767,766,765,764,762,761,760,759,757,756,755,753,752,
751,750,748,747,746,745,743,742,741,740,738,737,736,734,733,732,731,729,728,727,726,724,723,722,721,719,718,717,716,714,713,712,711,709,708,707,705,704,703,
702,701,699,698,697,696,694,693,692,690,689,688,687,686,684,683,682,681,679,678,677,676,674,673,672,671,669,668,667,666,664,663,662,661,659,658,657,656,655,
653,652,651,650,648,647,646,645,643,642,641,640,638,637,636,635,634,632,631,630,629,627,626,625,624,623,621,620,619,618,616,615,614,613,612,610,609,608,607,
605,604,603,602,601,599,598,597,596,595,593,592,591,590,588,587,586,585,584,582,581,580,579,578,576,575,574,573,572,570,569,568,567,565,564,563,562,561,559,
558,557,556,555,553,552,551,550,549,547,546,545,544,543,541,540,539,538,537,535,534,533,532,531,529,528,527,526,525,523,522,521,520,519,517,516,515,514,513,
511,510,509,508,507,505,504,503,502,501,499,498,497,496,495,493,492,491,490,489,488,486,485,484,483,482,480,479,478,477,476,474,473,472,471,470,468,467,466,
465,464,463,461,460,459,458,457,455,454,453,452,451,450,448,447,446,445,444,442,441,440,439,438,436,435,434,433,432,431,429,428,427,426,425,423,422,421,420,
419,418,416,415,414,413,412,410,409,408,407,406,405,403,402,401,400,399,398,396,395,394,393,392,390,389,388,387,386,385,383,382,381,380,379,378,376,375,374,
373,372,370,369,368,367,366,365,363,362,361,360,359,358,356,355,354,353,352,351,349,348,347,346,345,344,342,341,340,339,338,336,335,334,333,332,331,329,328,
327,326,325,324,322,321,320,319,318,317,315,314,313,312,311,310,308,307,306,305,304,303,301,300,299,298,297,296,294,293,292,291,290,289,287,286,285,284,283,
282,281,279,278,277,276,275,274,272,271,270,269,268,266,265,264,263,262,261,260,258,258,258,257,256,256,255,254,254,253,252,252,251,250,250,249,248,248,247,
246,246,245,244,244,243,242,242,241,240,240,239,238,238,237,236,236,235,234,234,233,232,232,231,230,230,229,228,228,227,226,226,225,224,224,223,222,222,221,
220,220,219,218,218,217,216,216,215,214,214,213,212,212,211,210,210,209,208,208,207,206,206,205,204,204,203,202,202,201,200,200,199,198,198,197,196,196,195,
194,194,193,192,192,191,190,190,189,188,188,187,186,186,185,184,184,183,182,182,181,180,180,179,178,178,177,176,176,175,174,174,173,172,172,171,170,170,169,
169,168,167,166,166,165,164,164,163,163,162,161,161,160,159,159,158,157,157,156,155,155,154,153,153,152,151,151,150,149,149,148,147,147,146,145,145,144,143,
143,142,142,141,140,139,139,138,138,137,136,136,135,134,134,133,132,132,131,130,130,129,128,128,127,126,126,125,124,124,123,122,122,121,121,120,119,119,118,
117,117,116,115,115,114,113,113,112,111,111,110,109,109,108,108,107,106,106,105,104,104,103,102,102,101,100,100,99,98,98,97,97,96,95,95,94,93,93,92,91,91,
90,89,89,88,87,87,86,86,85,84,84,83,82,82,81,80,80,79,78,78,77,76,76,75,75,74,73,73,72,71,71,70,69,69,68,67,67,66,66,65,64,64,63,62,62,61,60,60,59,58,58,
57,57,56,55,55,54,53,53,52,51,51,50,50,49,48,48,47,46,46,45,44,44,43,43,42,41,41,40,39,39,38,37,37,36,36,35,34,34,33,32,32,31,31,30,29,29,28,27,27,26,25,
25,24,24,23,22,22,21,21,20,19,19,18,17,17,16,16,15,14,14,13,13,12,11,11,10,10,9,8,8,7,7,6,5,5,4,4,3,3,2,2,1,0,0
};

int lookUpVoltage[1000]={4179,4168,4162,4158,4155,4152,4150,4147,4145,4143,4140,4139,4138,4135,4133,4133,4130,4129,4128,4125,4124,4123,4121,4119,4118,4117,
4116,4113,4113,4111,4110,4108,4107,4106,4104,4104,4101,4100,4099,4097,4096,4095,4094,4093,4091,4089,4089,4086,4085,4084,4083,4082,4080,4079,4078,4077,4075,
4074,4072,4072,4069,4069,4067,4066,4064,4063,4062,4061,4060,4058,4057,4056,4055,4053,4052,4051,4050,4049,4047,4046,4045,4044,4042,4041,4040,4039,4038,4035,
4035,4033,4033,4030,4030,4029,4028,4025,4025,4023,4023,4021,4021,4018,4018,4017,4016,4013,4013,4012,4011,4008,4008,4006,4006,4005,4003,4002,4001,4000,3999,
3997,3996,3995,3994,3992,3991,3990,3989,3988,3986,3985,3984,3983,3981,3980,3979,3979,3977,3977,3974,3974,3972,3972,3970,3969,3968,3967,3966,3964,3963,3962,
3961,3959,3959,3957,3957,3955,3955,3953,3952,3951,3950,3948,3947,3946,3945,3944,3942,3942,3940,3940,3938,3938,3936,3935,3934,3933,3931,3930,3929,3928,3927,
3925,3925,3923,3923,3922,3920,3919,3918,3917,3916,3914,3913,3913,3911,3911,3909,3908,3907,3906,3905,3903,3902,3901,3901,3898,3898,3897,3896,3895,3894,3892,
3891,3891,3889,3889,3886,3886,3885,3884,3883,3881,3880,3879,3879,3876,3876,3875,3874,3873,3872,3870,3869,3869,3867,3867,3864,3864,3863,3862,3861,3859,3859,
3857,3857,3856,3855,3853,3852,3851,3850,3850,3848,3847,3846,3845,3844,3842,3842,3841,3840,3839,3837,3836,3835,3835,3834,3833,3831,3830,3830,3829,3828,3826,
3825,3825,3824,3823,3822,3820,3820,3819,3818,3817,3815,3815,3814,3813,3812,3811,3811,3809,3808,3808,3807,3806,3804,3803,3803,3802,3801,3801,3800,3798,3798,
3797,3796,3795,3793,3793,3792,3791,3791,3790,3789,3789,3787,3786,3786,3785,3784,3784,3782,3781,3781,3780,3779,3779,3778,3776,3776,3776,3774,3774,3774,3773,
3771,3771,3770,3769,3769,3768,3768,3767,3767,3765,3764,3764,3764,3763,3762,3762,3761,3759,3759,3759,3758,3757,3757,3756,3756,3754,3754,3753,3753,3752,3752,
3751,3750,3750,3750,3748,3748,3747,3747,3746,3746,3745,3745,3745,3743,3742,3742,3742,3741,3740,3740,3740,3739,3739,3737,3737,3737,3736,3735,3735,3735,3735,
3734,3732,3732,3732,3731,3731,3730,3730,3730,3729,3729,3728,3728,3728,3726,3726,3725,3725,3725,3724,3724,3723,3723,3723,3723,3721,3720,3720,3720,3720,3719,
3719,3718,3718,3718,3718,3717,3715,3715,3715,3715,3714,3714,3713,3713,3713,3713,3712,3712,3710,3710,3710,3710,3709,3709,3708,3708,3708,3708,3707,3707,3706,
3706,3706,3706,3706,3704,3704,3703,3703,3703,3703,3702,3702,3701,3701,3701,3701,3699,3699,3698,3698,3698,3698,3698,3697,3697,3696,3696,3696,3696,3695,3695,
3693,3693,3693,3693,3693,3692,3692,3691,3691,3691,3691,3691,3690,3690,3688,3688,3688,3688,3687,3687,3687,3686,3686,3686,3686,3685,3685,3685,3684,3684,3684,
3684,3682,3682,3681,3681,3681,3681,3681,3680,3680,3680,3679,3679,3679,3679,3678,3678,3676,3676,3676,3676,3676,3675,3675,3674,3674,3674,3674,3674,3673,3673,
3673,3671,3671,3671,3671,3670,3670,3669,3669,3669,3669,3669,3668,3668,3667,3667,3667,3667,3667,3665,3665,3664,3664,3664,3664,3663,3663,3663,3662,3662,3662,
3662,3662,3660,3660,3659,3659,3659,3659,3658,3658,3658,3657,3657,3657,3657,3656,3656,3656,3656,3654,3654,3654,3654,3654,3654,3654,3653,3653,3653,3653,3652,
3652,3652,3652,3652,3652,3652,3651,3651,3651,3649,3649,3649,3649,3649,3649,3649,3649,3648,3648,3648,3647,3647,3647,3647,3647,3647,3647,3646,3646,3646,3645,
3645,3645,3645,3645,3645,3643,3643,3643,3643,3642,3642,3642,3642,3642,3642,3642,3641,3641,3641,3640,3640,3640,3640,3640,3640,3638,3638,3638,3637,3637,3637,
3637,3637,3637,3636,3636,3636,3635,3635,3635,3635,3635,3635,3634,3634,3634,3632,3632,3632,3632,3632,3631,3631,3631,3630,3630,3630,3630,3630,3629,3629,3629,
3627,3627,3627,3627,3626,3626,3626,3625,3625,3625,3625,3625,3624,3624,3623,3623,3623,3623,3623,3621,3621,3620,3620,3620,3620,3620,3619,3619,3618,3618,3618,
3618,3616,3616,3616,3615,3615,3615,3615,3614,3614,3613,3613,3613,3613,3612,3612,3610,3610,3610,3610,3609,3609,3608,3608,3608,3607,3607,3605,3605,3605,3605,
3604,3604,3603,3603,3603,3602,3602,3601,3601,3601,3599,3599,3598,3598,3598,3597,3597,3596,3596,3596,3594,3593,3593,3593,3593,3592,3591,3591,3591,3590,3590,
3588,3588,3587,3587,3586,3586,3586,3585,3584,3584,3584,3584,3582,3581,3581,3581,3580,3579,3579,3579,3577,3577,3576,3576,3575,3575,3574,3574,3574,3573,3571,
3571,3571,3570,3570,3569,3569,3569,3568,3566,3566,3566,3565,3565,3564,3564,3564,3563,3562,3562,3562,3562,3560,3559,3559,3559,3558,3558,3557,3557,3557,3555,
3554,3554,3554,3553,3553,3552,3552,3552,3551,3551,3549,3549,3549,3548,3548,3547,3547,3547,3546,3546,3544,3544,3544,3543,3543,3542,3542,3542,3541,3540,3540,
3540,3538,3537,3537,3537,3536,3535,3535,3535,3533,3532,3532,3532,3531,3530,3530,3529,3529,3527,3527,3526,3525,3525,3524,3522,3522,3521,3520,3520,3519,3518,
3516,3515,3515,3514,3513,3511,3510,3510,3508,3508,3505,3505,3504,3503,3500,3500,3498,3497,3496,3494,3493,3491,3490,3488,3486,3483,3482,3481,3479,3476,3474,
3471,3469,3466,3464,3461,3459,3455,3453,3449,3447,3442,3439,3435,3430,3426,3422,3416,3410,3405,3399,3393,3387,3380,3371,3363,3355,3345,3334,3326,3315,3303,
3289,3277,3264,3247,3233,3215,3198,3179,3159,3136,3112,3089,3062
};

