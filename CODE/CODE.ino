#include <DS3231.h>         //Memanggil RTC3231 Library
#include <Wire.h>           // i2C Conection Library
#include <LiquidCrystal.h>  //Libraries
#include <EEPROM.h>
#include <IRremote.h>

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);  //Arduino pins to lcd

// Define the pin where the IR receiver is connected
const int RECV_PIN = 8;           // Pin 11 for the IR receiver
int ledPin = A2;                  // Analog pin for LED
bool ledState = false;            // Store LED state (ON/OFF)
bool ledSwitch = true;
unsigned long lastToggle = 0;     // Track time of last toggle
const long debounceDelay = 1000;  // 1 second delay

// Define PIR sensor variables
// Define the pins
const int pirPin = 9;      // PIR sensor input pin
int pirState = LOW;        // Store the current state of PIR sensor
int val = 0;               // Variable to store the PIR sensor status

// Define the brightness levels
bool fullBrightness = false; // Full brightness

// Index to track which person's details to display
int currentPersonIndex = 0;

IRrecv irrecv(RECV_PIN);
decode_results results;

int bt_clock = 1, bt_up = 1, bt_light = 1, bt_down = 1, bt_timer = 1, bt_info = 1;

#define switch_1 A0
#define switch_2 A1
#define buzzer 13

// Init DS3231
DS3231 rtc(SDA, SCL);

// Init a Time-data structure
Time t;  //pencacah string time()

int hh = 0, mm = 0, ss = 0, dd = 0, bb = 0, set_day;
int yy = 0;
String Day = "  ";

int StartHH = 0, StartMM = 0, FinishHH = 0, FinishMM = 0, setMode = 0, setAlarm = 0, alarmMode = 1;
int Start1HH, Start1MM, Finish1HH, Finish1MM;
int Start2HH, Start2MM, Finish2HH, Finish2MM;
int Start3HH, Start3MM, Finish3HH, Finish3MM;
int Start4HH, Start4MM, Finish4HH, Finish4MM;

int timer1, timer2, timer3, timer4;

int stop = 0, mode = 0, flag = 0;

void setup() {
  rtc.begin();  // memulai koneksi i2c dengan RTC

  irrecv.enableIRIn();  // Start the IR receiver
  pinMode(switch_1, OUTPUT);
  pinMode(switch_2, OUTPUT);
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);  // Set the LED pin as an output

  pinMode(buzzer, OUTPUT);

  lcd.begin(16, 2);  // Configura lcd numero columnas y filas

  lcd.setCursor(0, 0);
  lcd.print("   WELCOME TO   ");
  lcd.setCursor(0, 1);
  lcd.print("  ATBU BAUCHI  ");
  delay(7000);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("   FACULTY OF   ");
  lcd.setCursor(0, 1);
  lcd.print("ENGINEERING TECH");
  delay(7000);
  lcd.clear();

  stop = EEPROM.read(50);
  if (stop == 0) {
  } else {
    mode = 1;
    WriteEeprom();
    delay(20);
    mode = 2;
    WriteEeprom();
    delay(20);
    mode = 3;
    WriteEeprom();
    delay(20);
    mode = 4;
    WriteEeprom();
    delay(20);
    mode = 0;
  }
  EEPROM.write(50, 0);

  ReadEeprom();
  //Set RTC Untuk Pertama kali
  //rtc.setDOW(2);  // Set Day-of-Week to SUNDAY
  //rtc.setTime(00, 9, 50);
  //rtc.setDate(12, 11, 2017);
}

void loop() {
  t = rtc.getTime();
  Day = rtc.getDOWStr(1);

  if (setMode == 0) {
    hh = t.hour, DEC;
    mm = t.min, DEC;
    ss = t.sec, DEC;
    dd = t.date, DEC;
    bb = t.mon, DEC;
    yy = t.year, DEC;
    //if(t.hour>12){hh=t.hour-12;}// for removing 24 houre
    //else{hh=t.hour;}
  }

  if (setAlarm == 0) {
    lcd.setCursor(0, 0);
    lcd.print((hh / 10) % 10);
    lcd.print(hh % 10);
    lcd.print(":");
    lcd.print((mm / 10) % 10);
    lcd.print(mm % 10);
    lcd.print(":");
    lcd.print((ss / 10) % 10);
    lcd.print(ss % 10);
    lcd.print("  T:");
    lcd.print(rtc.getTemp(), 0);
    lcd.write(223);
    lcd.print("C");
    lcd.print("  ");

    lcd.setCursor(1, 1);
    lcd.print(Day);
    lcd.print(" ");
    lcd.print((dd / 10) % 10);
    lcd.print(dd % 10);
    lcd.print("/");
    lcd.print((bb / 10) % 10);
    lcd.print(bb % 10);
    lcd.print("/");
    lcd.print((yy / 1000) % 10);
    lcd.print((yy / 100) % 10);
    lcd.print((yy / 10) % 10);
    lcd.print(yy % 10);
  }

  setupClock();
  setTimer();
  delay(100);
  blinking();

  //Timer1 ON
  if (timer1 == 1 && alarmMode == 1 && hh == Start1HH && mm == Start1MM) {
    ledSwitch = false;
  }
  //Timer1 OFF
  if (timer1 == 1 && alarmMode == 1 && hh == Finish1HH && mm == Finish1MM) {
    ledSwitch = true;
  }

  //Timer2 ON
  if (timer2 == 1 && alarmMode == 1 && hh == Start2HH && mm == Start2MM) {
    ledSwitch = false;
  }
  //Timer2 OFF
  if (timer2 == 1 && alarmMode == 1 && hh == Finish2HH && mm == Finish2MM) {
    ledSwitch = true;
  }

  //Timer3 ON
  if (timer3 == 1 && alarmMode == 1 && hh == Start3HH && mm == Start3MM) {
    ledSwitch = false;
  }
  //Timer3 OFF
  if (timer3 == 1 && alarmMode == 1 && hh == Finish3HH && mm == Finish3MM) {
    ledSwitch = true;
  }

  //Timer4 ON
  if (timer4 == 1 && alarmMode == 1 && hh == Start4HH && mm == Start4MM) {
    ledSwitch = false;
  }
  //Timer4 OFF
  if (timer4 == 1 && alarmMode == 1 && hh == Finish4HH && mm == Finish4MM) {
    ledSwitch = true;
  }

  delay(100);
  digitalWrite(buzzer, LOW);
}

void blinking() {
  //BLINKING SCREEN
  //Set Clock
  if (setAlarm < 2 && setMode == 1) {
    lcd.setCursor(0, 0);
    lcd.print("  ");
  }
  if (setAlarm < 2 && setMode == 2) {
    lcd.setCursor(3, 0);
    lcd.print("  ");
  }
  if (setAlarm < 2 && setMode == 3) {
    lcd.setCursor(6, 0);
    lcd.print("  ");
  }
  if (setAlarm < 2 && setMode == 4) {
    lcd.setCursor(1, 1);
    lcd.print("   ");
  }
  if (setAlarm < 2 && setMode == 5) {
    lcd.setCursor(5, 1);
    lcd.print("  ");
  }
  if (setAlarm < 2 && setMode == 6) {
    lcd.setCursor(8, 1);
    lcd.print("  ");
  }
  if (setAlarm < 2 && setMode == 7) {
    lcd.setCursor(11, 1);
    lcd.print("    ");
  }

  //Set Timer
  if (setMode == 0 && setAlarm == 1 && mode == 0) {
    lcd.setCursor(2, 1);
    lcd.print("  ");
  }
  if (setMode == 0 && setAlarm == 2 && mode == 0) {
    lcd.setCursor(6, 1);
    lcd.print("  ");
  }
  if (setMode == 0 && setAlarm == 3 && mode == 0) {
    lcd.setCursor(10, 1);
    lcd.print("  ");
  }
  if (setMode == 0 && setAlarm == 4 && mode == 0) {
    lcd.setCursor(13, 1);
    lcd.print("  ");
  }

  if (setMode == 0 && setAlarm == 1 && mode > 0) {
    lcd.setCursor(11, 0);
    lcd.print("  ");
  }
  if (setMode == 0 && setAlarm == 2 && mode > 0) {
    lcd.setCursor(14, 0);
    lcd.print("  ");
  }
  if (setMode == 0 && setAlarm == 3 && mode > 0) {
    lcd.setCursor(11, 1);
    lcd.print("  ");
  }
  if (setMode == 0 && setAlarm == 4 && mode > 0) {
    lcd.setCursor(14, 1);
    lcd.print("  ");
  }
}

//Seting Jam ,Tanggal,Alarm/Timer
void setupClock(void) {

  if (irrecv.decode(&results)) {
    unsigned long codeValue = results.value;  // Get the code from the remote
    Serial.println(codeValue);                // Print the code for debugging

    lcd.setCursor(0, 0);
    lcd.print(codeValue);

    // Use switch-case to handle different button presses
    switch (codeValue) {
      case 16753245:  // Button 1 (Replace with your remote's actual code)
        bt_clock = 0;
        break;

      case 16769565:  // Button 2
        bt_timer = 0;
        break;

      case 16769055:  // Button 3
        bt_down = 0;
        break;

      case 16736925:  // Button 4
        bt_light = 0;
        break;

      case 16754775:  // Button 6
        bt_up = 0;
        break;

      case 16748655:  // Button 7
        bt_info = 0;
        break;

      default:
        lcd.clear();
        Serial.println("Unknown button pressed");
        lcd.setCursor(0, 0);
        lcd.print(" Invalid Input ");
        delay(1000);
        lcd.clear();
        break;
    }

    irrecv.resume();  // Receive the next value
  }

  if (setMode == 8) {
    lcd.setCursor(0, 0);
    lcd.print("Set Time Finish ");
    lcd.setCursor(0, 1);
    lcd.print("Set Date Finish ");
    delay(1000);
    rtc.setTime(hh, mm, ss);
    rtc.setDate(dd, bb, yy);
    lcd.clear();
    setMode = 0;
  }

  if (setAlarm > 0) { alarmMode = 0; }

  if (bt_clock == 0 && flag == 0) {
    flag = 1;
    if (setAlarm > 0) {
      WriteEeprom();
      setAlarm = 1;
      mode = 5;
    } else {
      setMode = setMode + 1;
    }
    digitalWrite(buzzer, HIGH);
  }

  if (bt_timer == 0 && flag == 0) {
    flag = 1;
    if (setMode > 0) {
      setMode = 8;
    } else {
      setAlarm = setAlarm + 1;
      if (setAlarm > 4) {
        setAlarm = 1;
        WriteEeprom();
        mode = mode + 1;
        ReadEeprom();
      }
    }
    lcd.clear();
    digitalWrite(buzzer, HIGH);
  }

  if (setAlarm == 1 && mode == 5) {
    lcd.setCursor(0, 0);
    lcd.print("Set Timer Finish");
    lcd.setCursor(0, 1);
    lcd.print("-EEPROM Updated-");
    delay(2000);
    lcd.clear();
    setAlarm = 0;
    mode = 0;
    alarmMode = 1;
  }

  if (bt_clock == 1 && bt_timer == 1) { flag = 0; }

  if (bt_up == 0) {
    if (setAlarm < 2 && setMode == 1) hh = hh + 1;
    if (setAlarm < 2 && setMode == 2) mm = mm + 1;
    if (setAlarm < 2 && setMode == 3) ss = ss + 1;
    if (setAlarm < 2 && setMode == 4) set_day = set_day + 1;
    if (setAlarm < 2 && setMode == 5) dd = dd + 1;
    if (setAlarm < 2 && setMode == 6) bb = bb + 1;
    if (setAlarm < 2 && setMode == 7) yy = yy + 1;
    //Timer
    if (mode == 0 && setMode == 0 && setAlarm == 1) timer1 = 1;
    if (mode == 0 && setMode == 0 && setAlarm == 2) timer2 = 1;
    if (mode == 0 && setMode == 0 && setAlarm == 3) timer3 = 1;
    if (mode == 0 && setMode == 0 && setAlarm == 4) timer4 = 1;

    if (mode > 0 && setMode == 0 && setAlarm == 1) StartHH = StartHH + 1;
    if (mode > 0 && setMode == 0 && setAlarm == 2) StartMM = StartMM + 1;
    if (mode > 0 && setMode == 0 && setAlarm == 3) FinishHH = FinishHH + 1;
    if (mode > 0 && setMode == 0 && setAlarm == 4) FinishMM = FinishMM + 1;

    if (hh > 23) hh = 0;
    if (mm > 59) mm = 0;
    if (ss > 59) ss = 0;
    if (set_day > 7) set_day = 0;
    if (dd > 31) dd = 0;
    if (bb > 12) bb = 0;
    if (yy > 2030) yy = 2000;
    if (StartHH > 23) StartHH = 0;
    if (StartMM > 59) StartMM = 0;
    if (FinishHH > 23) FinishHH = 0;
    if (FinishMM > 59) FinishMM = 0;
    rtc.setDOW(set_day);
    digitalWrite(buzzer, HIGH);
  }

  if (bt_down == 0) {
    if (setAlarm < 2 && setMode == 1) hh = hh - 1;
    if (setAlarm < 2 && setMode == 2) mm = mm - 1;
    if (setAlarm < 2 && setMode == 3) ss = ss - 1;
    if (setAlarm < 2 && setMode == 4) set_day = set_day - 1;
    if (setAlarm < 2 && setMode == 5) dd = dd - 1;
    if (setAlarm < 2 && setMode == 6) bb = bb - 1;
    if (setAlarm < 2 && setMode == 7) yy = yy - 1;
    //Timer
    if (mode == 0 && setMode == 0 && setAlarm == 1) timer1 = 0;
    if (mode == 0 && setMode == 0 && setAlarm == 2) timer2 = 0;
    if (mode == 0 && setMode == 0 && setAlarm == 3) timer3 = 0;
    if (mode == 0 && setMode == 0 && setAlarm == 4) timer4 = 0;

    if (mode > 0 && setMode == 0 && setAlarm == 1) StartHH = StartHH - 1;
    if (mode > 0 && setMode == 0 && setAlarm == 2) StartMM = StartMM - 1;
    if (mode > 0 && setMode == 0 && setAlarm == 3) FinishHH = FinishHH - 1;
    if (mode > 0 && setMode == 0 && setAlarm == 4) FinishMM = FinishMM - 1;

    if (hh < 0) hh = 23;
    if (mm < 0) mm = 59;
    if (ss < 0) ss = 59;
    if (set_day < 0) set_day = 7;
    if (dd < 0) dd = 31;
    if (bb < 0) bb = 12;
    if (yy < 0) yy = 2030;
    if (StartHH < 0) StartHH = 23;
    if (StartMM < 0) StartMM = 59;
    if (FinishHH < 0) FinishHH = 23;
    if (FinishMM < 0) FinishMM = 59;
    rtc.setDOW(set_day);
    digitalWrite(buzzer, HIGH);
  }

  // If the button is pressed
  if (bt_info == 0) {

    // Display each person's regNumber and scroll fullName as marquee
    lcd.clear();
    delay(400);
    lcd.setCursor(0, 0);
    lcd.print("This Project Is ");
    lcd.setCursor(0, 1);
    lcd.print("Conducted By... ");
    delay(3000);

    // Display each person's regNumber and fullName only once
    lcd.clear();
    delay(400);
    lcd.setCursor(0, 0);
    lcd.print(" 17/48669U/2");
    lcd.setCursor(0, 1);
    lcd.print(" AKOWE BASHIRU ");
    delay(3000);

    // Display each person's regNumber and fullName only once
    lcd.clear();
    delay(400);
    lcd.setCursor(0, 0);
    lcd.print(" 16/42369U/2");
    lcd.setCursor(0, 1);
    lcd.print(" IBRAHIM YAKUBU ");
    delay(3000);

    // Display each person's regNumber and fullName only once
    lcd.clear();
    delay(400);
    lcd.setCursor(0, 0);
    lcd.print(" 16/43792U/2");
    lcd.setCursor(0, 1);
    lcd.print(" AUWALU HARUNA ");
    delay(3000);

    // Display each person's regNumber and fullName only once
    lcd.clear();
    delay(400);
    lcd.setCursor(0, 0);
    lcd.print(" 17/47033U/2");
    lcd.setCursor(0, 1);
    lcd.print("OCHEME KENNETH M");
    delay(3000);

    // Display each person's regNumber and fullName only once
    lcd.clear();
    delay(400);
    lcd.setCursor(0, 0);
    lcd.print(" 16/45727U/2");
    lcd.setCursor(0, 1);
    lcd.print("BABA ABDULAZEEZ ");
    delay(3000);

    // Display each person's regNumber and fullName only once
    lcd.clear();
    delay(400);
    lcd.setCursor(0, 0);
    lcd.print(" 18/52405D/2");
    lcd.setCursor(0, 1);
    lcd.print(" FRIDAY OMALE ");
    delay(3000);

    // Display each person's regNumber and fullName only once
    lcd.clear();
    delay(400);
    lcd.setCursor(0, 0);
    lcd.print(" 17/47950U/2");
    lcd.setCursor(0, 1);
    lcd.print("YUSUF ABDULLAHI");
    delay(3000);

    // Display each person's regNumber and fullName only once
    lcd.clear();
    delay(400);
    lcd.setCursor(0, 0);
    lcd.print(" 17/48834U/2");
    lcd.setCursor(0, 1);
    lcd.print("MUHAMMAD MUSA SA");
    delay(3000);

    // Display each person's regNumber and fullName only once
    lcd.clear();
    delay(400);
    lcd.setCursor(0, 0);
    lcd.print(" 17/47391U/2");
    lcd.setCursor(0, 1);
    lcd.print("ADAMU ELISHA INU");
    delay(3000);

    // Display each person's regNumber and fullName only once
    lcd.clear();
    delay(400);
    lcd.setCursor(0, 0);
    lcd.print(" 17/49926U/2");
    lcd.setCursor(0, 1);
    lcd.print("KATVONG NANTIM F");
    delay(3000);

    // Display each person's regNumber and fullName only once
    lcd.clear();
    delay(400);
    lcd.setCursor(0, 0);
    lcd.print(" 17/49997U/2");
    lcd.setCursor(0, 1);
    lcd.print("JONGBO SHOLA JER");
    delay(3000);

    // Display each person's regNumber and fullName only once
    lcd.clear();
    delay(400);
    lcd.setCursor(0, 0);
    lcd.print(" 17/50677D/2");
    lcd.setCursor(0, 1);
    lcd.print("BALA IBRAHIM BUB");
    delay(3000);

    // Display each person's regNumber and fullName only once
    lcd.clear();
    delay(400);
    lcd.setCursor(0, 0);
    lcd.print(" 17/48680U/2");
    lcd.setCursor(0, 1);
    lcd.print("ABDULLAHI KABIR");
    delay(3000);

    // Display each person's regNumber and fullName only once
    lcd.clear();
    delay(400);
    lcd.setCursor(0, 0);
    lcd.print(" 17/46344U/2");
    lcd.setCursor(0, 1);
    lcd.print("YUSUF MUHAMMED L");
    delay(3000);

    // Display each person's regNumber and fullName only once
    lcd.clear();
    delay(400);
    lcd.setCursor(0, 0);
    lcd.print("SUPERVISOR 1");
    lcd.setCursor(0, 1);
    lcd.print("ENGR. IT BALAMI ");
    delay(3000);

    // Display each person's regNumber and fullName only once
    lcd.clear();
    delay(400);
    lcd.setCursor(0, 0);
    lcd.print("SUPERVISOR 2");
    lcd.setCursor(0, 1);
    lcd.print("ENGR. Y. SALE   ");
    delay(3000);

    digitalWrite(buzzer, HIGH);
  }

  // Check if bt_light button is pressed
  if (bt_light == 0) {
    unsigned long currentMillis = millis();  // Get current time
    if (currentMillis - lastToggle >= debounceDelay) {
      // Toggle the LED state between ON and OFF
      ledState = !ledState;
      digitalWrite(ledPin, ledState ? HIGH : LOW);

      // Print the LED state to the Serial Monitor
      Serial.print("LED is now: ");
      Serial.println(ledState ? "ON" : "OFF");

      // Update the last toggle time
      lastToggle = currentMillis;
    }

    // Resume receiving IR signals
    irrecv.resume();
  }

  // Read PIR sensor value
  val = digitalRead(pirPin);
  brightness();

    if (val == HIGH) {       // If motion is detected
      if (pirState == LOW) {

        // Set LED to full brightness
        fullBrightness = true;

        pirState = HIGH;   // Update PIR state
      }
    } else {               // If no motion is detected
      if (pirState == HIGH) {

         // Set LED to dim brightness
        fullBrightness = false;
      
        pirState = LOW;    // Update PIR state
    }
  }

  bt_clock = 1;
  bt_up = 1;
  bt_down = 1;
  bt_timer = 1;
  bt_info = 1;
  bt_light = 1;
}

void setTimer() {
  //Timer
  if (setMode == 0 && setAlarm > 0 && mode > 0) {
    lcd.setCursor(0, 0);
    lcd.print("Timer");
    lcd.print(mode);
    lcd.print(" On :");
    lcd.setCursor(11, 0);
    lcd.print((StartHH / 10) % 10);
    lcd.print(StartHH % 10);
    lcd.print(":");
    lcd.print((StartMM / 10) % 10);
    lcd.print(StartMM % 10);

    lcd.setCursor(0, 1);
    lcd.print("Timer");
    lcd.print(mode);
    lcd.print(" Off:");
    lcd.setCursor(11, 1);
    lcd.print((FinishHH / 10) % 10);
    lcd.print(FinishHH % 10);
    lcd.print(":");
    lcd.print((FinishMM / 10) % 10);
    lcd.print(FinishMM % 10);
  }

  if (setMode == 0 && setAlarm > 0 && mode == 0) {
    lcd.setCursor(0, 0);
    lcd.print(" T1  T2  T3  T4 ");
    lcd.setCursor(0, 1);
    if (timer1 == 1) {
      lcd.print("  A");
    } else {
      lcd.print("  D");
    }

    if (timer2 == 1) {
      lcd.print("   A");
    } else {
      lcd.print("   D");
    }

    if (timer3 == 1) {
      lcd.print("   A");
    } else {
      lcd.print("   D");
    }

    if (timer4 == 1) {
      lcd.print("   A");
    } else {
      lcd.print("   D");
    }
  }
}

void ReadEeprom() {
  Start1HH = EEPROM.read(11);
  Start1MM = EEPROM.read(12);
  Finish1HH = EEPROM.read(13);
  Finish1MM = EEPROM.read(14);
  Start2HH = EEPROM.read(21);
  Start2MM = EEPROM.read(22);
  Finish2HH = EEPROM.read(23);
  Finish2MM = EEPROM.read(24);
  Start3HH = EEPROM.read(31);
  Start3MM = EEPROM.read(32);
  Finish3HH = EEPROM.read(33);
  Finish3MM = EEPROM.read(34);
  Start4HH = EEPROM.read(41);
  Start4MM = EEPROM.read(42);
  Finish4HH = EEPROM.read(43);
  Finish4MM = EEPROM.read(44);

  if (mode == 1) { StartHH = Start1HH, StartMM = Start1MM, FinishHH = Finish1HH, FinishMM = Finish1MM; }
  if (mode == 2) { StartHH = Start2HH, StartMM = Start2MM, FinishHH = Finish2HH, FinishMM = Finish2MM; }
  if (mode == 3) { StartHH = Start3HH, StartMM = Start3MM, FinishHH = Finish3HH, FinishMM = Finish3MM; }
  if (mode == 4) { StartHH = Start4HH, StartMM = Start4MM, FinishHH = Finish4HH, FinishMM = Finish4MM; }

  timer1 = EEPROM.read(1);
  timer2 = EEPROM.read(2);
  timer3 = EEPROM.read(3);
  timer4 = EEPROM.read(4);
}

void WriteEeprom() {
  if (mode == 1) {
    EEPROM.write(11, StartHH);
    EEPROM.write(12, StartMM);
    EEPROM.write(13, FinishHH);
    EEPROM.write(14, FinishMM);
  }
  if (mode == 2) {
    EEPROM.write(21, StartHH);
    EEPROM.write(22, StartMM);
    EEPROM.write(23, FinishHH);
    EEPROM.write(24, FinishMM);
  }
  if (mode == 3) {
    EEPROM.write(31, StartHH);
    EEPROM.write(32, StartMM);
    EEPROM.write(33, FinishHH);
    EEPROM.write(34, FinishMM);
  }
  if (mode == 4) {
    EEPROM.write(41, StartHH);
    EEPROM.write(42, StartMM);
    EEPROM.write(43, FinishHH);
    EEPROM.write(44, FinishMM);
  }

  EEPROM.write(1, timer1);
  EEPROM.write(2, timer2);
  EEPROM.write(3, timer3);
  EEPROM.write(4, timer4);
}

void brightness(){

  if (fullBrightness == true && ledSwitch == true){

    // Set LED to dim brightness
    digitalWrite(switch_1, HIGH);
    digitalWrite(switch_2, LOW);
  }
  else if (fullBrightness == false && ledSwitch == true) {

    // Set LED to dim brightness
    digitalWrite(switch_1, LOW);
    digitalWrite(switch_2, HIGH);
  }

  if (ledSwitch == false) {

    // Set LED to off
    digitalWrite(switch_1, LOW);
    digitalWrite(switch_2, LOW);
  }
}