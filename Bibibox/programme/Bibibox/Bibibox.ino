#include <IRremote.h>
#include <Servo.h>
#include <Adafruit_NeoPixel.h>
#include <Ultrasonic.h>
// ^ include des 4 library:
// - infra rouge (IRremote.h) 
// - servo moteurs (Servo.h)
// - Ruban de leds (Adafruit_NeoPixel.h)
// - Capteur ultrason (Ultrasonic.h)

// DEFINITION PIN DE CONNEXION DU CAPTEUR INFRA ROUGE
#define IR_PIN      3

// DEFINITION PIN DE CONNEXION DU CAPTEUR ULTRASON
#define US_PIN      7

//DEFINITION PIN SERVO
#define SERVO1_PIN  5
#define SERVO2_PIN  6

// DEFINITION PIN LEDS
#define LED_PIN_1   A0
#define LED_PIN_2   A1

// DEFINITION NOMBRE DE LEDS
#define LED_COUNT  3

// DEFINITION DES BOUTONS DE LA TELECOMMANDE
#define FLECHE_HAUT   0x46
#define FLECHE_BAS    0x15
#define FLECHE_GAUCHE 0x44
#define FLECHE_DROITE 0x43
#define TOUCHE_1      0x16
#define TOUCHE_2      0x19
#define TOUCHE_3      0xD
#define TOUCHE_4      0xC
#define TOUCHE_5      0x18
#define TOUCHE_6      0x5E
#define TOUCHE_7      0x8
#define TOUCHE_8      0x1C
#define TOUCHE_9      0x5A
#define TOUCHE_0      0x52
#define TOUCHE_ETOILE 0x42
#define TOUCHE_DIESE  0x4A
#define TOUCHE_OK     0x40

// variables "myservo1" et "myservo2" pour manipuler les servo moteurs
Servo myservo1;
Servo myservo2;

// variable "ultrasonic" pour manipuler le capteur à ultrason
Ultrasonic ultrasonic(US_PIN);

// variables "strip" pour manipuler le ruban de leds
Adafruit_NeoPixel phares(LED_COUNT, LED_PIN_1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel gyrophares(LED_COUNT, LED_PIN_2, NEO_GRB + NEO_KHZ800);

void setup()
{
    //////////////////// INITIALISATION ////////////////////
    
    // initialisation moniteur serie
    Serial.begin(9600);

    // initialisation leds
    phares.begin();
    phares.show();
    phares.setBrightness(50);
    
    gyrophares.begin();
    gyrophares.show();
    gyrophares.setBrightness(50);
    
  
    // initialisation infra rouge
    Serial.println("Enabling IR");
    IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);
    Serial.println("Enabled IR");

    // initialisation servo
    myservo1.attach(SERVO1_PIN);
    myservo2.attach(SERVO2_PIN);
    myservo1.write(90);
    myservo2.write(90);

    //////////////////// FIN INITIALISATION ////////////////////

}
//variable pour le capteur utltrason (le cpopilote)
int copilote = 1;

// Fonction "set_led_color" qui prend 5 parametres : le ruban de leds à modifier (&phares ou &gyrophares) et 3 variables numériques (entre 0 et 255) et applique cette couleur aux leds, et le délai entre tous les allumages de leds
void set_led_color(Adafruit_NeoPixel *strip, int red, int green, int blue,int delai)
{
    // crée une varaible "color" qui contient les informations des niveau de rouge de vert et de bleu
    int32_t color = strip->Color(red, green, blue);
    Serial.print("set led color");
    // boucle "for" pour itérer sur chaque led du ruban
    for(int i = 0; i < strip->numPixels(); i = i + 1)
    {
        // defini la couleur "color" pour la led numero "i"
        strip->setPixelColor(i, color);
        // applilque la couleur definie
        strip->show();
        delay (delai);

    }
}

// Fonction "eteindre_leds" qui eteind toutes les leds
void eteindre_leds(Adafruit_NeoPixel *strip)
{
    int32_t color = strip->Color(0, 0, 0);
    Serial.print("set led color");
    for(int i = 0; i < strip->numPixels(); i = i + 1)
    {
      strip->setPixelColor(i, color);
      strip->show();
    }
}

//Fonction "avancer" qui fait tourner les servomoteurs pour faire avancer le robot
void avancer()
{
    myservo1.write(180);
    myservo2.write(0);  
}

//Fonction "arreter" qui stop les servomoteurs pour arreter le robot
void arreter()
{
    myservo1.write(90);
    myservo2.write(90);  
}

//Fonction "Droite" qui fait tourner les servomoteurs pour faire tourner à droite le robot
void Droite()
{
    myservo1.write(180);
    myservo2.write(180);
}
//Fonction "Gauche" qui fait tourner les servomoteurs pour faire tourner à gauche le robot
void Gauche()
{   
    myservo1.write(0);
    myservo2.write(0);
}
//Fonction "Reculer" qui fait tourner les servomoteurs pour faire reculer le robot
void Reculer()
{
    myservo1.write(0);
    myservo2.write(180);
} 

// Fonction "capteur_ultrason" qui retourne la distance avec un obstacle en centimètre
long capteur_ultrason()
{
  long RangeInCentimeters;
    RangeInCentimeters = ultrasonic.read();
    Serial.print(RangeInCentimeters);//0~400cm
    Serial.println(" cm");

    return (RangeInCentimeters);
}

void loop()
{ 
  Serial.println(copilote);
    // si obstacle détécté
    if (capteur_ultrason() < 10 && copilote == 1)
    {
      set_led_color(&gyrophares, 255,0,0,0);
      arreter();
      delay(500);
      eteindre_leds(&gyrophares);
      
    }

    // sinon si quelquechose a été recu par la capteur infrarouge
    else if (IrReceiver.decode())
    {
        // test toutes les touches pour savoir la quelle à ete appuyée
        if (IrReceiver.decodedIRData.command == FLECHE_HAUT)
        {
            Serial.println("touche: FLECHE_HAUT");
            avancer();
        }
        else if (IrReceiver.decodedIRData.command == FLECHE_BAS)
        {
            Serial.println("touche: FLECHE_BAS");
            Reculer();
        }
        else if (IrReceiver.decodedIRData.command == FLECHE_GAUCHE)
        {
            Serial.println("touche: FLECHE_GAUCHE");
            Gauche();
        }
        else if (IrReceiver.decodedIRData.command == FLECHE_DROITE)
        {
            Serial.println("touche: FLECHE_DROITE");
            Droite();
        }
        else if (IrReceiver.decodedIRData.command == TOUCHE_1)
        {
            set_led_color(&gyrophares, 255,50,0,0);
            set_led_color(&phares, 255,255,255,0);
            Serial.println("touche: TOUCHE_1");

        }
        else if (IrReceiver.decodedIRData.command == TOUCHE_2)
        {
            set_led_color(&gyrophares, 255,0,255,0);
            set_led_color(&phares, 255,255,255,0);
             Serial.println("touche: TOUCHE_2");

        }
        else if (IrReceiver.decodedIRData.command == TOUCHE_3)
        {
          set_led_color(&gyrophares, 45,30,80,50);
          set_led_color(&phares, 255,255,255,50);
          Serial.println("touche: TOUCHE_3");

        }
        else if (IrReceiver.decodedIRData.command == TOUCHE_4)
        {
          set_led_color(&gyrophares, 254,40,30,0);
          set_led_color(&phares, 255,255,255,0);
          Serial.println("touche: TOUCHE_4");
        
        }
        else if (IrReceiver.decodedIRData.command == TOUCHE_5)
        {
          set_led_color(&gyrophares, 190,90,38,0);
          set_led_color(&phares, 255,255,255,0);
          Serial.println("touche: TOUCHE_5");

        }
        else if (IrReceiver.decodedIRData.command == TOUCHE_6)
        {
          set_led_color(&gyrophares, 20,20,20,0);
          set_led_color(&phares, 20,20,20,0);
          Serial.println("touche: TOUCHE_6");

        }
        else if (IrReceiver.decodedIRData.command == TOUCHE_7)
        {
          Serial.println("touche: TOUCHE_7");
          set_led_color(&phares, 255, 255, 255,0);
          set_led_color(&gyrophares, 0, 255, 0,0);

        }
        else if (IrReceiver.decodedIRData.command == TOUCHE_8)
        {
          Serial.println("touche: TOUCHE_8");
          set_led_color(&phares, 255, 255, 255,0);
          set_led_color(&gyrophares, 0, 0, 255,0);
        }
        else if (IrReceiver.decodedIRData.command == TOUCHE_9)
        {
          set_led_color(&phares, 255, 255, 255,0);
          set_led_color(&gyrophares, 255, 255, 255,0);
          Serial.println("touche: TOUCHE_ETOILE");

        }
        else if (IrReceiver.decodedIRData.command == TOUCHE_0)
        {
          Serial.println("touche: TOUCHE_0");
          eteindre_leds(&phares);
          eteindre_leds(&gyrophares);
        }
        else if (IrReceiver.decodedIRData.command == TOUCHE_ETOILE)
        {
          copilote = 1;
        }
        else if (IrReceiver.decodedIRData.command == TOUCHE_OK)
        {
          arreter();
          eteindre_leds(&phares);
          eteindre_leds(&gyrophares);
          Serial.println("touche: TOUCHE_OK");
        }
        IrReceiver.resume();
    }
    if (IrReceiver.decode() && IrReceiver.decodedIRData.command == TOUCHE_DIESE)        //déactivateur du copilote (le copilote arrête le robot si il y a un obstacle)
    {
        if (copilote == 1)
        {
          copilote = 0;
        }                       //les lignes peuvent être utiliser si l'on veut utiliser un seul bouton de la télécomande pour le copilote.
        //else if (copilote == 0)
        //{
        //  copilote = 1;
        //}
        delay (500);
        Serial.println("touche: TOUCHE_DIESE");
        Serial.println(copilote);
    }
}