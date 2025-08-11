// --------------------------------
// RAMA IEEE UNAL: CEIMTUN - RAS MINISUMO
// --------------------------------

#include "movimiento.h" // Librería personalizada creada por nosotros

// ------------------------------
// CONFIGURACIONES Y DEFINICIONES
// ------------------------------

const int sensoresIR[]   = {A7, A3, A4, A5}; // [lateral_izq, frontal_izq, frontal_der, lateral_der]
const int numSensoresIR  = 4;

const int sensoresQtr[]  = {A0, A1};         // [izquierdo, derecho]
const int numSensoresQtr = 2;

const int umbralDeteccion = 40; // Detección de contrincante. Valor por definir en pruebas
const int umbralAtaque    = 20; //Decisión de ataque. Valor por definir en pruebas

float valoresIR[numSensoresIR];
int indiceMin=0;
int nuevoIndiceMin=0;

int obtenerIndiceMinimo(float valores[], int numSensores);

// ------------------------------
// DEFINICIÓN DE ESTADOS
// ------------------------------
enum EstadoSumo {
  BUSCAR,
  REINTEGRARSE,
  ATAQUE,
  ACERCARSE,
  ESQUIVAR
};

EstadoSumo estadoActual = BUSCAR;

// ------------------------------
// FUNCIÓN: LECTURA GENERAL
// ------------------------------
void leerTodosLosSensores() {
  for (int i = 0; i < numSensoresIR; i++)
    valoresIR[i] = leerSensorIR(sensoresIR[i]);

  /*for (int i = 0; i < numSensoresQtr; i++)
    valoresQtr[i] = leerSensorQtr(sensoresQtr[i]);*/ //Probemos primero sin qtr
}


// ------------------------------
// SETUP
// ------------------------------
void setup() {
  Serial.begin(9600);

  Serial.println("=====================================");
  Serial.println(" INICIANDO ROBOT SUMO - RAMA IEEE UNAL");
  Serial.println("=====================================");

  for (int i = 0; i < numSensoresIR; i++) {
    pinMode(sensoresIR[i], INPUT);
  }
  /*for (int i = 0; i < numSensoresQtr; i++) {
    pinMode(sensoresQtr[i], INPUT);
  }*/

  pinMode(PWMA, OUTPUT); pinMode(AIN1, OUTPUT); pinMode(AIN2, OUTPUT);
  pinMode(PWMB, OUTPUT); pinMode(BIN1, OUTPUT); pinMode(BIN2, OUTPUT);
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH);

  Serial.println("CONFIGURACIÓN COMPLETA. COMENZANDO BÚSQUEDA...");
}

// ------------------------------
// ESTRATEGIA: BUSCAR
// ------------------------------
void Buscar() {
  Serial.println("-> ESTADO: BUSCAR");
  girarMoviendoseDerecha();

  indiceMin = obtenerIndiceMinimo(valoresIR, numSensoresIR);
  Serial.print("Sensor con valor mínimo: ");
  Serial.println(indiceMin);

  corregirDireccion(indiceMin);

}

// ------------------------------
// ESTRATEGIA: ACERCARSE
// ------------------------------
void Acercarse() {
  Serial.println("-> ESTADO: ACERCARSE");
  nuevoIndiceMin = obtenerIndiceMinimo(valoresIR, numSensoresIR);

  if (valoresIR[nuevoIndiceMin] > umbralDeteccion) {
    Serial.println("PERDÍ AL ENEMIGO -> CAMBIO A BUSCAR");
    return;
  }

  if (nuevoIndiceMin != indiceMin) {
    indiceMin = nuevoIndiceMin;
    Serial.print("El enemigo se movió, corrigiendo a sensor: ");
    Serial.println(indiceMin);
    corregirDireccion(indiceMin);
  }else
  

  moverAdelante();
}

// ------------------------------
// ESTRATEGIA: ATAQUE
// ------------------------------
void Ataque() {
  Serial.println("-> ESTADO: ATAQUE");
  moverAdelanteRapido();

  if (valoresIR[nuevoIndiceMin] >= 100) {
    Serial.println("ROMPIENDO ZONA MUERTA CON EMPUJE");
    moverAdelanteRapido();
    return;
  }

  nuevoIndiceMin = obtenerIndiceMinimo(valoresIR, numSensoresIR);

  if (valoresIR[nuevoIndiceMin] > umbralDeteccion) {
    Serial.println("PERDÍ AL ENEMIGO -> CAMBIO A BUSCAR");
    return;
  }

  if (nuevoIndiceMin != indiceMin) {
    indiceMin = nuevoIndiceMin;
    Serial.print("El enemigo se movió, corrigiendo a sensor: ");
    Serial.println(indiceMin);
    corregirDireccion(indiceMin);
  }
}


// ------------------------------
// ESTRATEGIA: REINTEGRARSE (SIN USO ACTUAL PORQUE ESTÁ SIN QTR)
// ------------------------------
void Reintegrarse() {

}

// ------------------------------
// ESTRATEGIA: ESQUIVAR
// ------------------------------
void Esquivar() {
  Serial.println("-> ESTADO: ESQUIVAR");
  indiceMin = obtenerIndiceMinimo(valoresIR, numSensoresIR);

  moverAtras();

  switch (indiceMin) {
    case 0:
      Serial.println("ESQUIVANDO: Giro en sentido contrario (Derecha)");
      girarDerechaRapido();
      break;    
    case 3:
      Serial.println("ESQUIVANDO: Giro en sentido contrario (Izquierda)");
      girarIzquierdaRapido();
    default:
      Serial.println("ESQUIVANDO: Caso no previsto");
      break;
  }
}



// ------------------------------
// TRANSICIONES FSM
// ------------------------------
void cambiarEstado() {
  nuevoIndiceMin = obtenerIndiceMinimo(valoresIR, numSensoresIR);
  float valorMinActual = valoresIR[nuevoIndiceMin];

  Serial.print("TRANSICIÓN FSM -> Sensor: ");
  Serial.print(nuevoIndiceMin);
  Serial.print(" | Valor: ");
  Serial.println(valorMinActual);

  if (valorMinActual >= 100.00) {
    if (nuevoIndiceMin==2) {
      estadoActual = ATAQUE;
      Serial.println("ENEMIGO MUY CERCA (FRENTE) -> FORZANDO ATAQUE");
    } else {
      estadoActual = ESQUIVAR;
      Serial.println("ENEMIGO MUY CERCA (LATERAL) -> FORZANDO ESQUIVA");
    }
  } 
  else if ((nuevoIndiceMin ==0 ||nuevoIndiceMin == 2) && valorMinActual <= umbralAtaque) {
    estadoActual = ATAQUE;
  } 
  else if (valorMinActual <= umbralDeteccion) {
    estadoActual = ACERCARSE;
  } 
  else if(valorMinActual >=umbralDeteccion) {
    estadoActual = BUSCAR;
  }

  indiceMin = nuevoIndiceMin;
}

// ------------------------------
// LOOP PRINCIPAL
// ------------------------------
void loop() {
  leerTodosLosSensores();

  // Impresión ordenada de sensores
  Serial.println("----- LECTURA SENSORES IR (cm) -----");
  for (int i = 0; i < numSensoresIR; i++) {
    Serial.print("Sensor[");
    Serial.print(i);
    Serial.print("]: ");
    Serial.print(valoresIR[i], 1); // Un decimal impreso nomás
    Serial.println(" cm");
  }
  Serial.println("------------------------------------");
  delay(1200); // Cambiar a millis() cuando ya terminemos pruebas

  // FSM: Ejecución según estado actual
  switch (estadoActual) {
    case BUSCAR:    Buscar();    break;
    case ATAQUE:    Ataque();    break;
    case ACERCARSE: Acercarse(); break;
    case ESQUIVAR:  Esquivar();  break;
    default:        break;
  }

  cambiarEstado();
}

// ------------------------------
// FUNCIÓN AUXILIAR: OBTENER ÍNDICE DEL SENSOR CON VALOR MÍNIMO
// ------------------------------
int obtenerIndiceMinimo(float valores[], int numSensores) {
  int minIdx = 0;
  float minVal = valores[0];

  for (int i = 0; i < numSensores; i++) {
    if (valores[i] < minVal) {
      minVal = valores[i];
      minIdx = i;
    }
  }

  return minIdx;
}

// ------------------------------
// FUNCIÓN AUXILIAR: CORREGIR DIRECCIÓN SEGÚN SENSOR MÍNIMO
// ------------------------------
void corregirDireccion(int minIdx) {
  switch (minIdx) {
    case 0:
      Serial.println("Enemigo detectado a la izquierda: Giro Derecha del todo para alinearme");
      girarDerechaRapido();
      break;
    case 1:
      Serial.println("Enemigo detectado de frente pero ligeramente a la izquierda: Giro Derecha un poco para alinearme");
      girarDerecha();
      break;
    case 2:
      Serial.println("Enemigo detectado de frente pero ligeramente a la derecha: Giro Izquierda para alinearme");
      girarIzquierda();
      break;
    case 3:
      Serial.println("Enemigo detectado a la izquierda: Giro Izquierda del todo para alinearme");
      girarIzquierdaRapido(); 
    default:
      Serial.println("CORRECCIÓN: Sin acción (Índice desconocido)");
      break;
  }
}


