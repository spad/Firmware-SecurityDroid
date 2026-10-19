# Progetto: Firmware Security Droid (testa K-2SO) – XIAO ESP32-S3 + PCA9685

Firmware per la testa del Security Droid (K-2SO) realizzata dall'utente: occhi animatronici (servo + NeoPixel) e 5 LED rossi laterali che lampeggiano con pattern che simulano attività di elaborazione.

Riferimento (solo consultazione, hardware diverso): `../K-2SO-Animatronic` (fork in github.com/spad/K-2SO-Animatronic). Usa ESP32Servo diretto senza PCA9685, I2C su GPIO1/2, MP3, IR e web UI.

## Come lavorare su questo progetto

- Rispondi sempre in **italiano**.
- **Un passo alla volta**: proponi un solo step (una modifica, un test, un comando), poi attendi il risultato prima di passare al successivo. Non riempire la risposta con molti controlli o azioni insieme.
- L'utente ha esperienza di elettronica: evita raccomandazioni ovvie (GND comune, polarità, ecc.).
- Il codice del repo di riferimento **non è dell'utente**: va letto e migliorato, non copiato. Prima di riprenderne parti, spiega cosa intendi cambiare.

## Ambiente

- **VS Code + PlatformIO** (mai Arduino IDE). Board `seeed_xiao_esp32s3`.
- `env:main` = firmware (`src/main.cpp`). I test hardware stanno in `src/tests/`, ognuno con un env dedicato (es. `env:i2c_scan`).
- GitHub personale: account `spad` (via SSH). Non usare l'account aziendale.

## Hardware in breve

I dettagli completi (cablaggio, alimentazione, calcoli) sono in `docs/HARDWARE.md`.

| Componente | Collegamento |
|---|---|
| Microcontrollore | Seeed XIAO ESP32-S3 |
| Driver PWM | Adafruit PCA9685 originale (16 canali), I2C indirizzo 0x40 |
| I2C | D4 (GPIO5) = SDA, D5 (GPIO6) = SCL |
| Servo | 2 micro servo 9g (tipo SG90) su canali **0** e **1** |
| LED | 5 LED rossi 3 mm su canali **2, 3, 4, 5, 6** |
| NeoPixel | 2 Jewel da 7 LED in daisy chain = **14 pixel**, dati da **D0 (GPIO1)** della XIAO |
| Alimentazione | 5 V esterni sul morsetto V+ del PCA (servo, LED, NeoPixel); VCC del PCA = 3V3 della XIAO |

## Vincoli software critici

1. **Frequenza PWM unica per tutti i canali** del PCA9685: deve restare a **50 Hz** per i servo. I LED su canali 2–6 lavorano quindi anch'essi a 50 Hz: on/off è perfetto, il dimming mostra sfarfallio visibile.
2. **Modalità di uscita globale**: il bit OUTDRV (totem-pole / open-drain) e il bit INVRT del registro MODE2 valgono per **tutti** i canali. Le uscite devono restare in **totem-pole** (default), altrimenti i servo non ricevono il segnale. Non usare `setOutputMode(false)`.
3. LED completamente accesi/spenti senza PWM: `setPWM(ch, 4096, 0)` = sempre acceso, `setPWM(ch, 0, 4096)` = sempre spento.
4. Servo: usare `writeMicroseconds()` o calcolare i tick a 50 Hz; range SG90 indicativo 500–2400 µs, **da calibrare** sui servo reali. Valutare `setOscillatorFrequency()` per correggere la tolleranza dell'oscillatore interno (nominale 25 MHz).
5. NeoPixel: 14 pixel, ordine GRB, 800 kHz. Limitare la luminosità via software (vedi budget di corrente in `docs/HARDWARE.md`).

## Librerie previste

- `Adafruit_PWMServoDriver` (PCA9685)
- `Adafruit_NeoPixel` (su ESP32-S3 usa la periferica RMT)
- `Wire` con `Wire.begin()` sui pin I2C di default della XIAO S3 (D4/D5)

## Punti aperti

- [ ] Verifica connessione I2C XIAO ↔ PCA9685: `pio run -e i2c_scan -t upload -t monitor`.
- [ ] Dimming dei LED rossi (requisito dell'utente): a 50 Hz lo sfarfallio è più visibile ai bassi livelli. Applicare una correzione gamma alla luminosità e valutare sul campo se lo sfarfallio è accettabile. Alternativa, se non lo è: spostare i LED su pin della XIAO con LEDC ad alta frequenza.
- [ ] Pattern di lampeggio dei 5 LED rossi laterali (simulazione attività di elaborazione).
- [ ] Livello logico dati NeoPixel: l'ESP32 esce a 3,3 V, i Jewel alimentati a 5 V richiedono circa 3,5 V minimi. Verificare se funziona stabile; se no, aggiungere un level shifter (es. 74AHCT125).
- [ ] Analisi del codice di riferimento (animazioni occhi) da reimplementare qui.
- [ ] Calibrazione fine corsa dei due servo.
