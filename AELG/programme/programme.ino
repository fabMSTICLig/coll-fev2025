
#include <Servo.h>
#include <Adafruit_NeoPixel.h>

#define SERVO1_PIN 5
#define SERVO2_PIN 6
#define SERVO3_PIN 3

Servo myservo1;
Servo myservo2;
Servo myservo3;


#define LED_PIN 2
#define LED_COUNT 3

#define LASER 7

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Définition des broches analogique
#define JOYSTICK_X A0  // X -> droite / gauche
#define JOYSTICK_Y A1  // Y -> vitesse / direction

#define LASER_BUT A2
#define BREAK A3
#define BOOST A4
#define NERF A5

// Définition de la marge autour du point d'origine (0, 0)
#define THRESHOLD 20

// Variables global
// calX : calibration en 0 de l'axe X
// calY : calibration en 0 de l'axe Y
int calX, calY;
// vitG vitesse moteur gauche
// vitD vitesse moteur droit
float vitG, vitD;

int slow = 0;

float bound(float val, float min, float max) {
  if (val < min) return min;
  else if (val > max) return max;
  else return val;
}


void set_leds_color(int r, int b, int v) {
  int32_t color = strip.Color(r, v, b);
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
}


void set_pixel_color(int i, int r, int v, int b) {
  strip.setPixelColor(i, strip.Color(r, v, b));
  strip.show();
}

// setup()
void setup() {
  // Initialisation du port série
  Serial.begin(115200);

  myservo1.attach(SERVO1_PIN);
  myservo2.attach(SERVO2_PIN);
  myservo3.attach(SERVO3_PIN);

  pinMode(LASER, OUTPUT);
  pinMode(LASER_BUT, INPUT_PULLUP);
  pinMode(BREAK, INPUT_PULLUP);
  pinMode(BOOST, INPUT_PULLUP);
  pinMode(NERF, INPUT_PULLUP);

  // Calibration de la valeur (0, 0) du joystick
  calX = analogRead(JOYSTICK_X);
  calY = analogRead(JOYSTICK_Y);


  strip.begin();            // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();             // Turn OFF all pixels ASAP
  strip.setBrightness(50);  // Set BRIGHTNESS to about 1/5 (max = 255)
}



// loop()
void loop() {
  // Calcul des données
  getData();

  if (digitalRead(BOOST) == LOW) {
    slow = 0;
  } else {
    slow = 75;
  }
  Serial.println(digitalRead(NERF));
  if (digitalRead(NERF) == 0) {
    myservo3.write(180);
  } else {
    myservo3.write(0);
  }

  if (digitalRead(LASER_BUT) == 0) {
    digitalWrite(LASER, HIGH);
  } else {
    digitalWrite(LASER, LOW);
  }


  if (digitalRead(BREAK) == LOW) {
    myservo1.attach(SERVO1_PIN);
    myservo2.attach(SERVO2_PIN);
    myservo1.write(0);
    myservo2.write(180);
    delay(50);
    myservo1.detach();
    myservo2.detach();
  }

  if (vitG == 0 && vitD == 0) {
    myservo1.detach();
    myservo2.detach();
      set_pixel_color(0,0,0,0);
      set_pixel_color(2,0,0,0);
  } else {
    myservo1.attach(SERVO1_PIN);
    myservo2.attach(SERVO2_PIN);
    myservo1.write(map(vitG, -100, 100, 0 + slow, 180 - slow));
    myservo2.write(map(vitD, -100, 100, 180 - slow, 0 + slow));

    if(vitG==vitD)
    {
      if(vitG>0)
      {
        set_pixel_color(0,100,100,100);
        set_pixel_color(2,100,100,100);
      }
      else
      {
        set_pixel_color(0,100,0,0);
        set_pixel_color(2,100,0,0);

      }

    }else if(vitG<vitD)
    {
      set_pixel_color(0,150,30,0);
      set_pixel_color(2,0,0,0);
    }
    else
    {
      set_pixel_color(2,150,30,0);
      set_pixel_color(0,0,0,0);

    }
  }


  // Delai de 500ms pour pouvoir lire la console
  delay(100);
}

// Fonction calculant les divers coefficient de vitesse, la direction, ainsi que la vitesse
void getData() {
  // rawX : valeur brute en X du joystick centrée sur calX
  // rawY : valeur brute en Y du joystick centrée sur calY
  // vitesse : vitesse calculé
  int rawX, rawY, vitesse;

  // Mesure des valeurs brute en X et Y
  rawX = analogRead(JOYSTICK_X);
  rawY = analogRead(JOYSTICK_Y);

  // Si rawY < THRESHOLD OU rawY > THRESHOLD
  if (rawY - calY > THRESHOLD || rawY - calY < -THRESHOLD) {
    // La vitesse est égale à map(rawY) depuis 0 ~ (1023 - calY) vers 0 ~ MAX_SPEED
    vitesse = map(rawY, 0, 1023, -100, 100);
  }  // Si rawY proche de 0
  else {
    // Les moteurs sont marqués comme arrétés, et vitesse = 0
    vitesse = 0;
  }
  if (rawX - calX > THRESHOLD || rawX - calX < -THRESHOLD) {
    //Selon la valeur de l'axe X, on applique un decalage sur la vitesse des moteurs gauche et droite.
    int rotate = map(rawX, 0, 1023, -100, 100);
    vitG = vitesse - rotate;
    vitD = vitesse + rotate;
    //on s'assure que la valeur reste entre -100 et 100;
    vitG = bound(vitG, -100, 100);
    vitD = bound(vitD, -100, 100);
  } else {
    vitG = vitesse;
    vitD = vitesse;
  }
}