# Hardware – dettagli di cablaggio e alimentazione

## Alimentazione

- **Alimentatore 5 V esterno** → morsetto a vite del PCA9685 (V+ e GND). Alimenta servo, LED rossi (tramite il rail V+ solo se necessario) e NeoPixel.
- **Condensatore elettrolitico 1000 µF / 35 V** montato sulla piazzola dedicata del PCA9685, tra V+ e GND.
- **VCC del PCA9685** ← **3V3 della XIAO**. Alimenta solo la logica del chip.
  - VCC non deve andare a 5 V: le pull-up I2C della scheda Adafruit sono collegate a VCC e porterebbero 5 V sui pin dell'ESP32, che non li tollerano.
- Il pin **V+** sulla fila laterale del PCA non è collegato alla XIAO.
- **GND comune** tra XIAO, PCA9685, alimentatore e NeoPixel.

### Alimentazione della XIAO

- **In fase di test**: via USB.
- **Configurazione finale**: dai 5 V esterni al pin **5V** della XIAO tramite un diodo Schottky **1N5819** (anodo verso l'alimentatore, catodo verso il pin 5V). Il diodo permette di collegare anche l'USB per la programmazione senza conflitti. Al pin arrivano circa 4,6 V, sufficienti per il regolatore interno.

## Mappa dei pin della XIAO ESP32-S3

| Pin XIAO | GPIO | Uso |
|---|---|---|
| D4 | GPIO5 | I2C SDA → PCA9685 |
| D5 | GPIO6 | I2C SCL → PCA9685 |
| D0 | GPIO1 | Dati NeoPixel → DIN del primo Jewel |
| 3V3 | – | VCC del PCA9685 |
| 5V | – | Ingresso alimentazione tramite 1N5819 (configurazione finale) |
| GND | – | GND comune |

## Canali del PCA9685

| Canale | Carico | Note |
|---|---|---|
| 0 | Micro servo 9g (SG90) | Segnale 50 Hz, alimentazione da V+ |
| 1 | Micro servo 9g (SG90) | Segnale 50 Hz, alimentazione da V+ |
| 2 | LED rosso 3 mm | Anodo sul pin PWM, catodo su GND |
| 3 | LED rosso 3 mm | Anodo sul pin PWM, catodo su GND |
| 4 | LED rosso 3 mm | Anodo sul pin PWM, catodo su GND |
| 5 | LED rosso 3 mm | Anodo sul pin PWM, catodo su GND |
| 6 | LED rosso 3 mm | Anodo sul pin PWM, catodo su GND |
| 7–15 | liberi | |

### LED rossi

- La scheda Adafruit ha già una resistenza da **220 Ω in serie** su ogni uscita: non servono resistenze esterne.
- Collegamento **diretto (source)**: le uscite lavorano a VCC = 3,3 V. Corrente per LED rosso (Vf circa 1,9 V): (3,3 − 1,9) / 220 ≈ **6 mA**. Luminosità adeguata per indicatori.
- Il collegamento "a sink" (anodo su V+, catodo sul PWM, uscite in open-drain) è **escluso**: la modalità open-drain è globale e impedirebbe ai servo di ricevere il segnale.

## NeoPixel Jewel (2 × 7 LED)

- Daisy chain: XIAO → DIN Jewel 1 → DOUT Jewel 1 → DIN Jewel 2. Totale **14 pixel**.
- Alimentazione: PWR e GND dei Jewel dal rail **5 V del PCA9685** (morsetto/V+).
- Consigliata una resistenza da **300–470 Ω** in serie sulla linea dati, vicino al DIN del primo Jewel.
- Livello logico: dati a 3,3 V con Jewel a 5 V sono al limite della soglia (circa 0,7 × VDD = 3,5 V). Spesso funziona, ma se compaiono colori errati o sfarfallii serve un level shifter (74AHCT125 o simile).

## Budget di corrente sul 5 V (stime)

| Carico | Tipico | Picco |
|---|---|---|
| 2 servo SG90 | 2 × 150 mA | 2 × 700 mA (avvio/stallo) |
| 14 NeoPixel | dipende da colore/luminosità | 14 × 60 mA = 840 mA (bianco pieno) |
| 5 LED rossi | 30 mA (dal 3V3 via VCC) | – |
| XIAO ESP32-S3 (configurazione finale) | 100 mA | circa 350 mA (WiFi) |

- Alimentatore consigliato: **almeno 3 A a 5 V**.
- Limitare la luminosità dei NeoPixel via software (es. `setBrightness()` al 30–50%) riduce di molto i picchi.
