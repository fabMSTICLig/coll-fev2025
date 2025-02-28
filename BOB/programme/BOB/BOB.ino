#include <IRremote.h>
#include <Servo.h>
#include <Adafruit_NeoPixel.h>
#include <Ultrasonic.h>
#include <LiquidCrystal_PCF8574.h>

// ^ include des 5 library:
// - infra rouge (IRremote.h)
// - servo moteurs (Servo.h)
// - Ruban de leds (Adafruit_NeoPixel.h)
// - Capteur ultrason (Ultrasonic.h)
// - ecran LCD (LiquidCrystal_PCF8574)

// DEFINITION PIN DE CONNEXION DU CAPTEUR INFRA ROUGE
#define IR_PIN 3

// DEFINITION PIN DE CONNEXION DU CAPTEUR ULTRASON
#define US_PIN 7

//DEFINITION PIN SERVO
#define SERVO1_PIN 5
#define SERVO2_PIN 6

// DEFINITION PIN LEDS
#define LED_PIN_1 A1
#define LEDHEAD_PIN A0

// DEFINITION NOMBRE DE LEDS
#define LED_COUNT 3
#define LEDHEAD_COUNT 16

// DEFINITION DES BOUTONS DE LA TELECOMMANDE
#define FLECHE_HAUT 0x46
#define FLECHE_BAS 0x15
#define FLECHE_GAUCHE 0x44
#define FLECHE_DROITE 0x43
#define TOUCHE_1 0x16
#define TOUCHE_2 0x19
#define TOUCHE_3 0xD
#define TOUCHE_4 0xC
#define TOUCHE_5 0x18
#define TOUCHE_6 0x5E
#define TOUCHE_7 0x8
#define TOUCHE_8 0x1C
#define TOUCHE_9 0x5A
#define TOUCHE_0 0x52
#define TOUCHE_ETOILE 0x42
#define TOUCHE_DIESE 0x4A
#define TOUCHE_OK 0x40

// variables "myservo1" et "myservo2" pour manipuler les servo moteurs
Servo myservo1;
Servo myservo2;

//Variable pour manipuler l'ecran LCD
LiquidCrystal_PCF8574 lcd(0x27);  // définie l'adresse I2C de l'ecran LCD (0x27) 20 caracteres sur 4 lignes

// variable "ultrasonic" pour manipuler le capteur à ultrason
Ultrasonic ultrasonic(US_PIN);

// variables "strip" pour manipuler le ruban de leds
Adafruit_NeoPixel devant(LED_COUNT, LED_PIN_1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel head(LEDHEAD_COUNT, LEDHEAD_PIN, NEO_GRB + NEO_KHZ800);



void init_lcd() {
  int error = 0;
  Wire.begin();
  Wire.beginTransmission(0x27);
  error = Wire.endTransmission();

  if (error == 0) {
    Serial.println("LCD found.");
    lcd.begin(20, 4);
    lcd.clear();
    lcd.setBacklight(255);

  } else {
    Serial.print("Error: ");
    Serial.print(error);
    Serial.println(": LCD not found.");
  }
}

void setup() {
  //////////////////// INITIALISATION ////////////////////

  // initialisation moniteur serie
  Serial.begin(9600);
  Serial.println("START");

  // initialisation leds
  devant.begin();
  devant.show();
  devant.setBrightness(50);

  head.begin();
  head.show();
  head.setBrightness(50);


  // initialisation servo
  myservo1.attach(SERVO1_PIN);
  myservo2.attach(SERVO2_PIN);
  myservo1.write(90);
  myservo2.write(90);

  // initialisation ecran LCD (avec la fonction definie plus haut)
  init_lcd();


  // initialisation infra rouge
  Serial.println("Enabling IR");
  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);
  Serial.println("Enabled IR");

  //////////////////// FIN INITIALISATION ////////////////////
}

// Fonction "set_led_color" qui prend 4 parametres : le ruban de leds à modifier (&strip1 ou &strip2) et 3 variables numériques (entre 0 et 255) et applique cette couleur aux leds
void set_led_color(Adafruit_NeoPixel *strip, int red, int green, int blue) {
  // crée une varaible "color" qui contient les informations des niveau de rouge de vert et de bleu
  int32_t color = strip->Color(red, green, blue);
  Serial.println("set led color");
  // boucle "for" pour itérer sur chaque led du ruban
  for (int i = 0; i < strip->numPixels(); i = i + 1) {
    // defini la couleur "color" pour la led numero "i"
    strip->setPixelColor(i, color);
    // applilque la couleur definie
    strip->show();
  }
}

// Fonction "eteindre_leds" qui eteind toutes les leds
void eteindre_leds(Adafruit_NeoPixel *strip) {
  int32_t color = strip->Color(0, 0, 0);
  Serial.println("set led color");
  for (int i = 0; i < strip->numPixels(); i = i + 1) {
    strip->setPixelColor(i, color);
    strip->show();
  }
}

//Fonction "avancer" qui fait tourner les servomoteurs pour faire avancer le robot
void avancer() {
  myservo1.write(180);
  myservo2.write(0);
}

//Fonction "arreter" qui stop les servomoteurs pour arreter le robot
void arreter() {
  myservo1.write(90);
  myservo2.write(90);
}

//Fonction "Droite" qui fait tourner les servomoteurs pour faire tourner à droite le robot
void Droite() {
  myservo1.write(180);
  myservo2.write(180);
}
//Fonction "Gauche" qui fait tourner les servomoteurs pour faire tourner à gauche le robot
void Gauche() {
  myservo1.write(0);
  myservo2.write(0);
}
//Fonction "Reculer" qui fait tourner les servomoteurs pour faire reculer le robot
void Reculer() {
  myservo1.write(0);
  myservo2.write(180);
}

// Fonction "capteur_ultrason" qui retourne la distance avec un obstacle en centimètre
long capteur_ultrason() {
  long RangeInCentimeters;
  RangeInCentimeters = ultrasonic.read();
  Serial.print(RangeInCentimeters);  //0~400cm
  Serial.println(" cm");

  return (RangeInCentimeters);
}

// Fonction permettant d'effacer l'ecran LCD et d'y afficher une phrase
void afficher_une_phrase_lcd(char *str) {
  lcd.clear();
  lcd.print(str);
}

void loop() {
  // si obstacle détécté
  if (capteur_ultrason() < 20) {
    set_led_color(&devant, 255, 0, 0);
    set_led_color(&head, 255, 0, 0);
    while (capteur_ultrason() < 25) {
      Gauche();
      delay(100);
    }
    arreter();
    eteindre_leds(&devant);
    eteindre_leds(&head);
  }
  // sinon si quelquechose a été recu par la capteur infrarouge
  else if (IrReceiver.decode()) {
    // test toutes les touches pour savoir la quelle à ete appuyée
    if (IrReceiver.decodedIRData.command == FLECHE_HAUT) {
      Serial.println("touche: FLECHE_HAUT");
      avancer();
    } else if (IrReceiver.decodedIRData.command == FLECHE_BAS) {
      Serial.println("touche: FLECHE_BAS");
      Reculer();
    } else if (IrReceiver.decodedIRData.command == FLECHE_GAUCHE) {
      Serial.println("touche: FLECHE_GAUCHE");
      Gauche();
    } else if (IrReceiver.decodedIRData.command == FLECHE_DROITE) {
      Serial.println("touche: FLECHE_DROITE");
      Droite();
    } else if (IrReceiver.decodedIRData.command == TOUCHE_1) {
      Serial.println("touche: TOUCHE_1");
      afficher_une_phrase_lcd("Salut je suis BOB!");
    } else if (IrReceiver.decodedIRData.command == TOUCHE_2) {
      Serial.println("touche: TOUCHE_2");
      afficher_une_phrase_lcd("BOOOOOB!!!!!");

    } else if (IrReceiver.decodedIRData.command == TOUCHE_3) {
      Serial.println("touche: TOUCHE_3");
      afficher_une_phrase_lcd("J'ai faim!!");

    } else if (IrReceiver.decodedIRData.command == TOUCHE_4) {
      Serial.println("touche: TOUCHE_4");
      afficher_une_phrase_lcd("(bruit de robot)");

    } else if (IrReceiver.decodedIRData.command == TOUCHE_5) {
      Serial.println("touche: TOUCHE_5");
      afficher_une_phrase_lcd("Fabrique au FabLab");

    } else if (IrReceiver.decodedIRData.command == TOUCHE_6) {
      Serial.println("touche: TOUCHE_6");
      afficher_une_phrase_lcd("A l'aide!!!         J'ai peeuuur!!");

    } else if (IrReceiver.decodedIRData.command == TOUCHE_7) {
      Serial.println("touche: TOUCHE_7");
      set_led_color(&devant, 0, 255, 0);
      set_led_color(&head, 0, 255, 0);

    } else if (IrReceiver.decodedIRData.command == TOUCHE_8) {
      Serial.println("touche: TOUCHE_8");
      set_led_color(&devant, 0, 0, 255);
      set_led_color(&head, 0, 0, 255);
    } else if (IrReceiver.decodedIRData.command == TOUCHE_9) {
      Serial.println("touche: TOUCHE_9");
      set_led_color(&devant, 255, 0, 255);
      set_led_color(&head, 255, 0, 255);

    } else if (IrReceiver.decodedIRData.command == TOUCHE_0) {
      Serial.println("touche: TOUCHE_0");
      afficher_une_phrase_lcd("Cours si tu peux,je vais te retrouver");
    } else if (IrReceiver.decodedIRData.command == TOUCHE_ETOILE) {
      Serial.println("touche: TOUCHE_ETOILE");
      eteindre_leds(&devant);
      eteindre_leds(&head);
      arreter();
    } else if (IrReceiver.decodedIRData.command == TOUCHE_DIESE) {
      Serial.println("touche: TOUCHE_DIESE");
      set_led_color(&devant, 255, 255, 0);
      set_led_color(&head, 255, 255, 0);

    } else if (IrReceiver.decodedIRData.command == TOUCHE_OK) {
      lcd.clear();
      Serial.println("touche: TOUCHE_OK");
    }
    IrReceiver.resume();
  }
}