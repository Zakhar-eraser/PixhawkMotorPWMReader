#define pwmMax 1900
#define pwmNeut 1300
#define pwmMin 800
#define ESC_SPEED 34
#define NO_SIGNAL 50

#define MEASURES_COUNT 10 //max 29
#define MEASURE_PWM pwmNeut
#define BOOL 0
#define PWM 0

#define COUNTER 0
#define SUMM_PWM 0

#define PWMAI 44
#define PWMA 12
#define DIRA1 34 
#define DIRA2 35
#define offsetA 17

#define PWMBI 45
#define PWMB 8
#define DIRB1 36 
#define DIRB2 37
#define offsetB 16

#define PWMCI 46
#define PWMC 9
#define DIRC1 43 
#define DIRC2 42
#define offsetC 17

#define PWMDI 13
#define PWMD 5
#define DIRD1 A5
#define DIRD2 A4
#define offsetD 17

volatile unsigned long lastTime = 0;

int PWMInfo[4][9] = {{PWMAI, PWMA, DIRA1, DIRA2, COUNTER, SUMM_PWM, offsetA, BOOL, 0}, {PWMBI, PWMB, DIRB1, DIRB2, COUNTER, SUMM_PWM , offsetB, BOOL, 0}, {PWMCI, PWMC, DIRC1, DIRC2, COUNTER, SUMM_PWM, offsetC, BOOL, 0}, {PWMDI, PWMD, DIRD1, DIRD2, COUNTER, SUMM_PWM, offsetD, BOOL, 0}};

class Motor
{
  private:
  unsigned int readPWM, writePWM, directionPIN1, directionPIN2,calibrationCounter, pwm = 0;
  int pwmOffset = 0;
  unsigned long summPWM = 0;
  boolean canPrint = false;
};

int SetSpeed(int pwmRead, int pwmWrite, int dir1, int dir2, int* counter, int* sumPWM, int offSet, int* pwm){
  int PWMsignal = pulseIn(pwmRead, HIGH, (unsigned long)(1000000.0/ESC_SPEED)) + offSet;
  *pwm = PWMsignal;
  if(*pwm <= pwmMax && *pwm >= pwmMin)
  {
    if(*counter < MEASURES_COUNT)
    {
      *counter += 1;
      *sumPWM += *pwm;
    }
    if(*pwm >= pwmNeut)
    {
      *pwm = map(*pwm, pwmNeut, pwmMax, 0, 255);
      digitalWrite(dir1, LOW); digitalWrite(dir2, HIGH);
    }
    else
    {
      *pwm = map(*pwm, pwmNeut, pwmMin, 0, 255);
      digitalWrite(dir1, HIGH); digitalWrite(dir2, LOW);
    }
  } else
  {
    if(*pwm > NO_SIGNAL){
      if(*pwm > pwmMax)
      {
        *pwm = 255;
        digitalWrite(dir1, LOW); digitalWrite(dir2, HIGH);
      } else digitalWrite(dir1, HIGH); digitalWrite(dir2, LOW);
    }else *pwm = 0;
  }
  return PWMsignal;
}

void setup() {
  Serial.begin(9600);
  pinMode(PWMA, OUTPUT);pinMode(PWMAI, INPUT);
  pinMode(DIRA1, OUTPUT);pinMode(DIRA2, OUTPUT);
  pinMode(PWMB, OUTPUT);pinMode(PWMBI, INPUT);
  pinMode(DIRB1, OUTPUT);pinMode(DIRB2, OUTPUT);
  pinMode(PWMC, OUTPUT);pinMode(PWMCI, INPUT);
  pinMode(DIRC1, OUTPUT);pinMode(DIRC2, OUTPUT);
  pinMode(PWMD, OUTPUT);pinMode(PWMDI, INPUT);
  pinMode(DIRD1, OUTPUT);pinMode(DIRD2, OUTPUT);
}

void loop() {
  byte i;
  if(Serial.available()>0)
  {
    Serial.println("Start");
    Serial.readString();
    for(i = 0; i < 4; i++)
    {
      PWMInfo[i][4] = 0;
      PWMInfo[i][5] = 0;
      PWMInfo[i][7] = 1;
    }
  }
  for(i = 0; i < 4; i++)
  {
    Serial.print("PWM");
    Serial.print(i + 1);
    Serial.print(" ");
    Serial.println(SetSpeed(PWMInfo[i][0], PWMInfo[i][1], PWMInfo[i][2], PWMInfo[i][3], &PWMInfo[i][4], &PWMInfo[i][5], PWMInfo[i][6], &PWMInfo[i][8]));
    if(PWMInfo[i][7] && PWMInfo[i][4] == MEASURES_COUNT)
    {
      PWMInfo[i][7] = 0;
      PWMInfo[i][6] += MEASURE_PWM - PWMInfo[i][5]/MEASURES_COUNT;
      Serial.print("PWM");
      Serial.print(i + 1);
      Serial.print(" ");
      Serial.println(PWMInfo[i][6]);
    }
  }
  for(i = 0; i < 4; i++)
  {
    analogWrite(PWMInfo[i][1], PWMInfo[i][8]);
  }
}
