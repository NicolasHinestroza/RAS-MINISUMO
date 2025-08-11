// ------------------------------
// CONFIGURACIONES Y CONSTANTES
// ------------------------------
const int NUM_LECTURAS = 10;
const float V_REF = 5.0;
const float k1 = 18.9;
const float k2 = -0.896;
const int QTR_UMBRAL = 200;

const int ADC_MIN_VALIDO = 5; 
const float DISTANCIA_MAXIMA = 100.0;

// ------------------------------
// SENSORES
// ------------------------------
const int sensoresIR[]  = {A7, A3, A4, A5}; // [lateral_izq, frontal_izq, frontal_der, lateral_der]
const int numSensoresIR = 4;
const int sensoresQtr[]  = {A0, A1};         // [izquierdo, derecho]
const int numSensoresQtr = 2;

float valoresIR[numSensoresIR];   // distancias en cm
bool valoresQtr[numSensoresQtr];

// ------------------------------
// SETUP
// ------------------------------
void setup() {
  Serial.begin(9600);
}

// ------------------------------
// LOOP PRINCIPAL
// ------------------------------
void loop() {
  leerTodosLosSensores();

  // Impresión ordenada de sensores IR
  Serial.println("----- LECTURA SENSORES IR (cm) -----");
  for (int i = 0; i < numSensoresIR; i++) {
    Serial.print("SensorIR[");
    Serial.print(i);
    Serial.print("]: ");
    Serial.print(valoresIR[i], 1);
    Serial.println(" cm");
  }
  Serial.println("----- LECTURA SENSORES QTR (bool) -----");
  for (int i = 0; i < numSensoresQtr; i++) {
    Serial.print("QTR[");
    Serial.print(i);
    Serial.print("]: ");
    Serial.println(valoresQtr[i] ? "Dojo" : "Borde");
  }

  Serial.println();
  delay(200);
}

// ------------------------------
// FUNCIONES
// ------------------------------
void leerTodosLosSensores() {
  for (int i = 0; i < numSensoresIR; i++)
    valoresIR[i] = leerSensorIR(sensoresIR[i]);

  for (int i = 0; i < numSensoresQtr; i++)
    valoresQtr[i] = leerSensorQtr(sensoresQtr[i]);
}


void ordenarValores(int array[]) {
  for (int j = 1; j < NUM_LECTURAS; j++) {
    int actual = array[j];
    int i = j - 1;
    while (i >= 0 && array[i] > actual) {
      array[i + 1] = array[i];
      i--;
    }
    array[i + 1] = actual;
  }
}

float adcToCm(float adcValue) {
  if (adcValue <= ADC_MIN_VALIDO) {
    return DISTANCIA_MAXIMA; // fuera de rango / objeto no detectado
  }
  float voltage = (adcValue * V_REF) / 1023.0; // usar 1023.0 para 10-bit ADC
  return pow(voltage * (1.0 / k1), 1.0 / k2);
}

float calcularMediana(int array[]) {
  ordenarValores(array);
  int medianaADC = (array[(NUM_LECTURAS - 1) / 2] + array[NUM_LECTURAS / 2]) / 2;
  return adcToCm(medianaADC);  // Conversión a cm
}

float leerSensorIR(int pin) {
  int lecturas[NUM_LECTURAS];
  for (int i = 0; i < NUM_LECTURAS; i++) {
    lecturas[i] = analogRead(pin);
    delay(5);
  }
  return calcularMediana(lecturas);
}

bool leerSensorQtr(int pin) {
  int valor=analogRead(pin);
  return valor>QTR_UMBRAL;
}
