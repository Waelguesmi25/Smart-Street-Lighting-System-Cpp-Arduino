/*
  Projet : Réseau de lampadaires intelligent (POO, héritage, polymorphisme)
  Auteur : Wael GUESMI & Maha ROMDHANI
  Matériel : Arduino Uno/Nano, 3 LEDs (pins PWM), 4 capteurs IR (digital),
            1 bouton pour changer le mode.
*/

#include <Arduino.h>

/* ======================= Config pins ======================= */
const uint8_t PIN_LAMP_1 = 3;  // PWM
const uint8_t PIN_LAMP_2 = 5;  // PWM
const uint8_t PIN_LAMP_3 = 6;  // PWM

const uint8_t PIN_CAPT_1 = 7;  // digital input
const uint8_t PIN_CAPT_2 = 8;
const uint8_t PIN_CAPT_3 = 9;
const uint8_t PIN_CAPT_4 = 10; // capteur spécial : extinction générale

const uint8_t PIN_BTN_MODE = 2; // bouton pour changer de mode

/* ======================= Base : Composant ======================= */
class Composant {
  public:
    virtual void initialiser() = 0;
    virtual void mettreAJour() = 0;
    virtual ~Composant() {}
};

/* ======================= MessageManager ======================= */
class MessageManager : public Composant {
  public:
    void initialiser() override {
      // Serial initialisé dans setup
    }
    void mettreAJour() override {}
    
    // Message quand une personne est détectée
    void notifierDetection(uint8_t idCapteur) {
      Serial.println();
      Serial.println("========================================");
      Serial.print(">>> PERSONNE DETECTEE PAR CAPTEUR ");
      Serial.print(idCapteur);
      Serial.println(" <<<");
      Serial.println("========================================");
    }
    
    // Message quand la personne quitte un capteur
    void notifierFinDetection(uint8_t idCapteur) {
      Serial.print("[INFO] Personne a quitte la zone du capteur ");
      Serial.println(idCapteur);
    }
    
    void notifierAllumage(uint8_t idLampe) {
      Serial.print("[LAMPE] >> Lampe ");
      Serial.print(idLampe);
      Serial.println(" ALLUMEE <<");
    }
    
    void notifierExtinction(uint8_t idLampe) {
      Serial.print("[LAMPE] >> Lampe ");
      Serial.print(idLampe);
      Serial.println(" ETEINTE (delai expire) <<");
    }
    
    void notifierMode(const char* modeName) {
      Serial.println();
      Serial.println("****************************************");
      Serial.print("** CHANGEMENT DE MODE -> ");
      Serial.print(modeName);
      Serial.println(" **");
      Serial.println("****************************************");
    }
    
    // Message de progression dans le réseau
    void notifierProgression(uint8_t capteurDepart, uint8_t capteurArrivee) {
      Serial.println("----------------------------------------");
      Serial.print("=> DEPLACEMENT: Capteur ");
      Serial.print(capteurDepart);
      Serial.print(" -> Capteur ");
      Serial.println(capteurArrivee);
      Serial.println("----------------------------------------");
    }
};

/* ======================= Capteur (abstrait) ======================= */
class Capteur : public Composant {
  public:
    virtual bool detecte() = 0;
    virtual bool estActif() = 0;
    virtual String description() = 0;
};

/* ======================= CapteurIR (concret) ======================= */
class CapteurIR : public Capteur {
  private:
    uint8_t pin;
    uint8_t id;
    bool lastState;
    bool currentActive; // état actuel (HIGH = personne présente)
    unsigned long lastDebounceMillis;
    const unsigned long debounceDelay = 50; // ms
    MessageManager* msgMgr;
  public:
    CapteurIR(uint8_t pin_, uint8_t id_, MessageManager* mm) :
      pin(pin_), id(id_), lastState(false), currentActive(false), 
      lastDebounceMillis(0), msgMgr(mm) {}

    void initialiser() override {
      pinMode(pin, INPUT);
      lastState = digitalRead(pin);
      currentActive = lastState;
    }

    // Renvoie true si NOUVELLE détection (front montant)
    bool detecte() override {
      bool reading = digitalRead(pin);
      if (reading != lastState) {
        if (millis() - lastDebounceMillis > debounceDelay) {
          lastDebounceMillis = millis();
          lastState = reading;
          
          if (reading == HIGH && !currentActive) {
            // Front montant : nouvelle détection
            currentActive = true;
            if (msgMgr) msgMgr->notifierDetection(id);
            return true;
          } else if (reading == LOW && currentActive) {
            // Front descendant : personne partie
            currentActive = false;
            if (msgMgr) msgMgr->notifierFinDetection(id);
          }
        }
      }
      return false;
    }

    bool estActif() override {
      return currentActive;
    }

    String description() override {
      return String("CapteurIR #") + id;
    }

    void mettreAJour() override {
      detecte();
    }
    
    uint8_t getId() { return id; }
};

/* ======================= Luminaire (abstrait) ======================= */
class Luminaire : public Composant {
  public:
    virtual void allumer() = 0;
    virtual void eteindre() = 0;
    virtual bool estAllume() = 0;
    virtual String nom() = 0;
};

/* ======================= Lampadaire (concret) ======================= */
class Lampadaire : public Luminaire {
  private:
    uint8_t pinLED;
    uint8_t id;
    bool etat;
    unsigned long dernierMouvement;
    unsigned long delaiExtinction;
    MessageManager* msgMgr;
    uint8_t pwmValeur;
    bool enFade;
  public:
    Lampadaire(uint8_t pin_, uint8_t id_, MessageManager* mm, unsigned long delaiMs = 7000, bool fade = true) :
      pinLED(pin_), id(id_), etat(false), dernierMouvement(0),
      delaiExtinction(delaiMs), msgMgr(mm), pwmValeur(255), enFade(fade) {}

    void initialiser() override {
      pinMode(pinLED, OUTPUT);
      eteindre();
    }

    void allumer() override {
      if (!etat) {
        etat = true;
        if (enFade) {
          for (int v = 0; v <= pwmValeur; v += 25) {
            analogWrite(pinLED, v);
            delay(8);
          }
        }
        analogWrite(pinLED, pwmValeur);
        if (msgMgr) msgMgr->notifierAllumage(id);
      }
      dernierMouvement = millis();
    }

    void eteindre() override {
      if (etat) {
        if (enFade) {
          for (int v = pwmValeur; v >= 0; v -= 25) {
            analogWrite(pinLED, v);
            delay(8);
          }
        }
        analogWrite(pinLED, 0);
        etat = false;
        if (msgMgr) msgMgr->notifierExtinction(id);
      }
    }

    bool estAllume() override { return etat; }

    String nom() override {
      return String("Lampadaire #") + id;
    }

    void remettreTimer() {
      dernierMouvement = millis();
      if (!etat) allumer();
    }

    void mettreAJour() override {
      if (etat) {
        if (millis() - dernierMouvement > delaiExtinction) {
          eteindre();
        }
      }
    }
};

/* ======================= ModeLuminosite ======================= */
enum Mode { MODE_JOUR = 0, MODE_NUIT = 1, MODE_MAINTENANCE = 2, MODE_DEMO = 3 };

class ModeLuminosite : public Composant {
  private:
    uint8_t pinButton;
    Mode modeActuel;
    unsigned long lastDebounceMillis;
    const unsigned long debounceDelay = 50;
    bool lastBtnState;
    MessageManager* msgMgr;
  public:
    ModeLuminosite(uint8_t pinBtn, MessageManager* mm) : 
      pinButton(pinBtn), modeActuel(MODE_NUIT), lastDebounceMillis(0), 
      lastBtnState(LOW), msgMgr(mm) {}

    void initialiser() override {
      pinMode(pinButton, INPUT_PULLUP);
      lastBtnState = digitalRead(pinButton);
      if (msgMgr) msgMgr->notifierMode(modeName());
    }

    void mettreAJour() override {
      bool reading = digitalRead(pinButton);
      if (reading != lastBtnState) {
        if (millis() - lastDebounceMillis > debounceDelay) {
          lastDebounceMillis = millis();
          lastBtnState = reading;
          if (reading == LOW) {
            cycleMode();
          }
        }
      }
    }

    void cycleMode() {
      modeActuel = static_cast<Mode>((modeActuel + 1) % 4);
      if (msgMgr) msgMgr->notifierMode(modeName());
    }

    Mode getMode() { return modeActuel; }

    const char* modeName() {
      switch (modeActuel) {
        case MODE_JOUR: return "JOUR";
        case MODE_NUIT: return "NUIT";
        case MODE_MAINTENANCE: return "MAINTENANCE";
        case MODE_DEMO: return "DEMO";
        default: return "UNKNOWN";
      }
    }
};

/* ======================= ReseauLuminaire ======================= */
class ReseauLuminaire : public Composant {
  private:
    MessageManager* messageManager;
    ModeLuminosite* modeLum;
    CapteurIR* capteursIR[4];
    Lampadaire* lampes[3];
    int mapping[4];
    uint8_t dernierCapteurActif; // Pour traquer les déplacements
    
  public:
    ReseauLuminaire(MessageManager* mm, ModeLuminosite* ml) : 
      messageManager(mm), modeLum(ml), dernierCapteurActif(0) {
      mapping[0] = 0;
      mapping[1] = 1;
      mapping[2] = 2;
      mapping[3] = -1;
      
      capteursIR[0] = new CapteurIR(PIN_CAPT_1, 1, messageManager);
      capteursIR[1] = new CapteurIR(PIN_CAPT_2, 2, messageManager);
      capteursIR[2] = new CapteurIR(PIN_CAPT_3, 3, messageManager);
      capteursIR[3] = new CapteurIR(PIN_CAPT_4, 4, messageManager);

      lampes[0] = new Lampadaire(PIN_LAMP_1, 1, messageManager, 7000, true);
      lampes[1] = new Lampadaire(PIN_LAMP_2, 2, messageManager, 7000, true);
      lampes[2] = new Lampadaire(PIN_LAMP_3, 3, messageManager, 7000, true);
    }

    void initialiser() override {
      messageManager->initialiser();
      modeLum->initialiser();
      for (int i = 0; i < 4; ++i) capteursIR[i]->initialiser();
      for (int i = 0; i < 3; ++i) lampes[i]->initialiser();
    }

    void mettreAJour() override {
      modeLum->mettreAJour();
      Mode currentMode = modeLum->getMode();

      if (currentMode == MODE_JOUR) {
        for (int i = 0; i < 3; ++i) {
          if (lampes[i]->estAllume()) lampes[i]->eteindre();
        }
        return;
      }

      if (currentMode == MODE_MAINTENANCE) {
        for (int i = 0; i < 3; ++i) {
          if (!lampes[i]->estAllume()) lampes[i]->allumer();
        }
        return;
      }

      if (currentMode == MODE_DEMO) {
        static unsigned long lastDemo = 0;
        static int idx = 0;
        if (millis() - lastDemo > 1000) {
          for (int i = 0; i < 3; ++i) {
            if (i == idx) lampes[i]->allumer();
            else lampes[i]->eteindre();
          }
          idx = (idx + 1) % 3;
          lastDemo = millis();
        }
        return;
      }

      // MODE_NUIT : logique avec détection de progression
      for (int i = 0; i < 4; ++i) {
        bool d = capteursIR[i]->detecte();
        if (d) {
          // Vérifier si c'est une progression (changement de capteur)
          if (dernierCapteurActif != 0 && dernierCapteurActif != capteursIR[i]->getId()) {
            messageManager->notifierProgression(dernierCapteurActif, capteursIR[i]->getId());
          }
          dernierCapteurActif = capteursIR[i]->getId();
          
          int mapTo = mapping[i];
          if (mapTo >= 0 && mapTo < 3) {
            lampes[mapTo]->remettreTimer();
            if (mapTo - 1 >= 0) {
              lampes[mapTo - 1]->eteindre();
            }
          } else if (mapTo == -1) {
            for (int j = 0; j < 3; ++j) {
              lampes[j]->eteindre();
            }
          }
        }
      }

      for (int i = 0; i < 3; ++i) {
        lampes[i]->mettreAJour();
      }
    }
};

/* ======================= Globals / setup / loop ======================= */
MessageManager gMsgMgr;
ModeLuminosite gMode(PIN_BTN_MODE, &gMsgMgr);
ReseauLuminaire* gReseau;

void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }
  delay(50);
  Serial.println();
  Serial.println("========================================");
  Serial.println("=== SYSTEME LUMINAIRE INTELLIGENT ===");
  Serial.println("===     Version Amelioree v2.0     ===");
  Serial.println("========================================");
  Serial.println();

  gReseau = new ReseauLuminaire(&gMsgMgr, &gMode);
  gReseau->initialiser();
  
  Serial.println("[SYSTEME] Initialisation terminee.");
  Serial.println("[SYSTEME] Pret a detecter...");
  Serial.println();
}

void loop() {
  gReseau->mettreAJour();
}
