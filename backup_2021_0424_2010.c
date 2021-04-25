#include <Servo.h>

const int moveSave = 3;// remember previous angle
const int servoCnt = 6;// Servo number
const int buttonSwitch1 = 2; //pin 2
const int buttonSwitch2 = 4; //pin 4
const int buttonSwitch3 = 6; //pin 6
const int servoCntTemp = servoCnt+1;

const int pot[servoCnt] = {A0,A1,A2,A3,A4,A5}; //Potentimeters

int potVal[servoCnt]; //Potentimeter values
int potAngle[servoCnt];

Servo aServo[servoCntTemp];

int tempAngle = 0; //store angle for a moment
int angle[servoCnt][moveSave+1]; //store angle and save move


//slow moving servo variables
int getAngle[servoCnt]; //get current angle
bool isWaiting[servoCnt]; // is waiting
int targetAngle[servoCnt] // target angle
int leftAngle[servoCnt] // target angle
int timeleft[servoCnt];
int moveDelay = 10;  //milliseconds


unsigned long time;
unsigned long movePeriod = 2;


int buttonPressed = 0; //for position set
int buttonModeNum = 4; // number different mode //1 :random // 2 :same angle // 3 : potentio with joystick // 4 : bluetooth control
int currMode = 0;
int setPos = 90;

void setup()
{
    pinMode(buttonSwitch1,INPUT);
    pinMode(buttonSwitch2,INPUT);
    pinMode(buttonSwitch3,INPUT);
    
    aServo[0].attach(3);
    aServo[1].attach(5);
    aServo[2].attach(7);
    aServo[3].attach(8);
    aServo[4].attach(10);
    aServo[5].attach(12);
    
    for( int i = 0 ; i < servoCnt ; i++ ){
        getAngle[i] = aServo[i].read();
    }

  Serial.begin(9600);
}


void loop() 
{
    time = millis(); //get time
    servoUpdate(); // run real time controling, servo motion (slow move) // start very first to check servo

    //print every 1 seconds
    if(time%1000 == 0){
        Serial.println(time);
        Serial.print("Current Mode : "); // show how many button pressed
        Serial.println(currMode);
    }
    buttonMode(); // set robot arm move mode / robot control style
    // directBluetoothMode(); // instantly turn on bluetooth contr0l
    controlManager(); // stores information and controls body




}

void moveServo(int a, int b)
{  
    leftAngle[a] = b - getAngle[i]; // move angle calcu

    if(leftAngle[a] > 0){ // + route
        isLeftAnglePositive = true;
        timeleft[i] = movePeriod*1000;
        isWaiting[a] = false;
    } else if(leftAngle[a] < 0){ // - route
        isLeftAnglePositive = true;
        timeleft[i] = movePeriod*1000;
        isWaiting[a] = false;
    }
}


void servoUpdate(){
    if(time%moveDelay == 0){  // 단위 시간 에 인입
        for( int i = 0 ; i < servoCnt ; i++ ){
            if(!isWaiting[i]){ // 움직이는 중이라면,
                getAngle[i] = aServo[i].read(); // 현재 각도를 가져와서
                if (getAngle[i] == targetAngle[i] || timeleft[i] <= 0 ){ // 같으면 완료 처리  // 시간초과시 생략(근데 2초에 단위 10밀리세컨이라 부족할리 없음)
                    leftAngle = 0;
                    timeleft = 0;
                    isWaiting[i] = true;

                } else { //다르면, 단위 이동만큼 실시
                    timeleft[i] = timeleft[i] - moveDelay;

                    if(isLeftAnglePositive){ //양수일때
                        aServo[i].write(getAngle[i]+1);
                        leftAngle[i]--;
                    } else{ ///음수일 때
                        aServo[i].write(getAngle[i]-1);
                        leftAngle[i]++;
                    }
                }
            }
        }
    }
}



void controlManager(){
    moveServoMode1();  // MODE 1 : random move every movePeriod of time   / 3seconds
    moveServoMode2();  // MODE 2 : Set position
    moveServoMode3();  // MODE 3 : on Set position, move every servo with potentiometer **** not yet (2 more potentio need)
    moveServoMode4();  // MODE 4 : on current position, move number 3 servo with potentiometer
}

void buttonMode(){
    if(digitalRead(buttonSwitch1) == HIGH) // if button pressed, dont move servos, set 90 dgree
    {
        buttonPressed ++;
        currMode = buttonPressed%buttonModeNum;
        Serial.println("Button Pressed!!!!");
        delay300();

        if(currMode == 0){
            showButtonMode();
        }
        if(currMode == 1){
            showButtonMode();

            Serial.println("Button Set Position!");
            moveServoToSetPos(currMode);
            Serial.print("ALL Servos's angle set to SET degree : ");
            Serial.println(setPos);
        }
        if(currMode == 2){
            showButtonMode();

            Serial.println("Potentiometer control MODE");
            moveServoToSetPos(currMode);
        }
        if(currMode == 3){
            showButtonMode();
            Serial.println("Bluetooth control MODE");
        }

    
                
        Serial.print("Button Pressed : "); // show how many button pressed
        Serial.println(buttonPressed);
    }
}

void moveServoMode1(){
    if(time%(int(movePeriod)*1000) == 0 && currMode == 0){
        Serial.print("movePeriod_time  : "); 
        Serial.println(time/1000); 


        // get random angle and set angle        
        for(int j=0 ; j < servoCnt ; j++){
            tempAngle = random(0,40)+70; // servo angle random value create
            setAngle(j,tempAngle);
            
            Serial.print("Servo : ");
            Serial.print(j+1);
            Serial.print(" angle : ");
            Serial.println(angle[j][0]);
        }
        
        
        //move servo
        for(int j=0; j<servoCnt ; j++){
            if(j!=3){
                moveServo(j,angle[j][0]);
            }
        }
        Serial.println("move cycle Finished");
        
        // previous angle save
        for(int j=0 ; j < servoCnt ; j++){
            Serial.print("\n Servo : ");
            Serial.println(j+1);

            for(int i = moveSave; i > 0 ; i--)
            {
                angle[j][i]=angle[j][i-1];
                Serial.print("angle ");
                Serial.print(i);
                Serial.print(" : ");
                Serial.println(angle[j][i]);
            }
        }
        Serial.println("previous angle save Finished");
    }               
}

void moveServoMode2(){
    if(currMode == 1){
        setPosIncrease(currMode); //joystick control
        setPosDecrease(currMode); //joystick control
    }
}


void moveServoMode3(){
    if(currMode == 2){
        setPosIncrease(currMode); //joystick control
        setPosDecrease(currMode); //joystick control
        getPotenAngle(); //joystick control

        for(int j=0; j<servoCnt ; j++){
            moveServo(j,potAngle[j]);
        }

        if(time%300 == 0){
            for(int j=0; j<servoCnt ; j++){
                Serial.print(j+1);
                Serial.print(" Servo Angle : ");
                Serial.println(potAngle[j]);
            }
        }
    }
}

void moveServoMode4(){
    if(currMode == 3){
        /// get blue data (arm num & angle)
        // refresh angle
        // adjust angle




        for(int j=0; j<servoCnt ; j++){
            moveServo(j,potAngle[j]);
        }

        if(time%300 == 0){
            for(int j=0; j<servoCnt ; j++){
                Serial.print(j+1);
                Serial.print(" Servo Angle : ");
                Serial.println(potAngle[j]);
            }
        }
    }
}



//bluetooth mode short cut
void directBluetoothMode(){
    /// if bluetooh ok => instanly turn currMode to 4

    // process
    // app -> make bluetooth control button first / connect / click 
    // => 1) get current angle => adjust angle to phone
    // => 2) arduino : turn currmode to 4
    // => if ready, bluetooth control on
}

void getPotenAngle(){
    for(int i = 0 ; i <servoCnt ; i++){
        potVal[i] = analogRead(pot[i]);
        potAngle[i] = map(potVal[i], 0, 1023, 0, 179);
    }

    //temp code (6 poten = delete )/ 4poten = USE
    potAngle[0] = 90;
    potAngle[1] = 90;


    // pot1Val = analogRead(pot1); // This will read the values from the potentimeters and store it...
    // pot1Angle = map(pot1Val, 0, 1023, 0, 179); // ... and this will map the values from the potentiometers to values the servos can use and store it for later use
}

void setPosIncrease(int a){
    if(digitalRead(buttonSwitch2) == HIGH){
        setPos = setPos + 3;
        Serial.print("SET degree INcreased \n Current SET degree : ");
        Serial.println(setPos);
        moveServoToSetPos(a);
        delay300();
    }
}

void setPosDecrease(int a){
    if(digitalRead(buttonSwitch3) == HIGH){
        setPos = setPos - 3;
        Serial.print("SET degree DEcreased \n Current SET degree : ");
        Serial.println(setPos);
        moveServoToSetPos(a);
        delay300();
    }
}

void moveServoToSetPos(int a){
    setAngleTemp(a);
    //move servo
    moveServoTemp(a);
}



void setAngle(int a, int b) // move + position save
{
    angle[a][0] = b;
    angle[a][moveSave]=b;
}

void setAngleTemp(int a){
    for(int j=0 ; j < servoCnt ; j++){
        tempAngle = setPos;// servo angle set degree
        angle[j][moveSave]= tempAngle;
        Serial.print("Servo : ");
        Serial.print(j+1);
        Serial.print(" angle : ");
        Serial.println(angle[j][moveSave]);
    }
}

void moveServoTemp(int a)
{
    if(a==2){
        for(int j=0; j< servoCnt ; j++){
            if(j != 3){
                moveServo(j,angle[j][moveSave]);
            }
        }
    }  
    else{
        for(int j=0; j< servoCnt ; j++){
        moveServo(j,angle[j][moveSave]);
        }
    }  
}

void delay300(){
    delay(300);
}

void showButtonMode(){
    Serial.print("Button MODE : ");
    Serial.println(currMode + 1);
}







