const int NUM_LECTURAS = 10;
const int QTR_UMBRAL=200; //OJO Esteban, este umbral se debe cuadrar de manera practica
const float V_REF = 5.0;
const float k1 = 18.9;
const float k2 = -0.896; 

const int ADC_MIN_VALIDO = 5;  // Lecturas menores se consideran "fuera de rango"
const float DISTANCIA_MAXIMA = 100.0; // Valor fijo para "no detectado"

// Ordena el arreglo con Insertion Sort
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

// Convierte un valor ADC a cm
float adcToCm(int adcValue){
  if (adcValue <= ADC_MIN_VALIDO) {
    return DISTANCIA_MAXIMA; // Si la lectura es muy baja, lo marco como fuera de rango
  }

  float voltage = (adcValue * V_REF) / 1024.0;
  return pow(voltage * (1.0 / k1), 1.0 / k2);
}

// Calcula la mediana y la convierte a cm
float calcularMediana(int array[]) {
  ordenarValores(array);
  int medianaADC = (array[(NUM_LECTURAS - 1) / 2] + array[NUM_LECTURAS / 2]) / 2;
  return adcToCm(medianaADC);  // Conversión a cm
}

// Lee un sensor IR con filtrado por mediana y devuelve la distancia en cm
float leerSensorIR(int pin) {
  int lecturas[NUM_LECTURAS];
  for (int i = 0; i < NUM_LECTURAS; i++) {
    lecturas[i] = analogRead(pin);
    delay(5);
  }
  return calcularMediana(lecturas);
}

// Lee directamente un sensor QTR sin filtrar (valor adc crudo)
bool leerSensorQtr(int pin) {
  int valor=analogRead(pin);
  return valor>QTR_UMBRAL;
}