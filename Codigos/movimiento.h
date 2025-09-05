#include "lectura_sensores_robot.h"
//Variables Globales
const int VEL_NORMAL = 50;
const int VEL_RAPIDA = 130;
const int VEL_GIRO_LENTO=70;
// Pines Motores 
const int PWMA = 9;     // PWM motor derecho
const int AIN1 = 7;      // Sentido 1 motor derecho
const int AIN2 = 8;     // Sentido 2 motor derecho
const int PWMB = 3;      // PWM motor izquierdo
const int BIN1 = 5;      // Sentido 1 motor izquierdo
const int BIN2 = 4;      // Sentido 2 motor izquierdo
const int STBY = 6;      // Pin para STBY

  // -------------------------
  // Funciones de Movimiento
  // -------------------------



  // Función para mover los motores
  void moverMotor(int PWMPin, int pin1, int pin2, int velocidad, bool direccion) {
    digitalWrite(pin1, direccion ? HIGH : LOW);
    digitalWrite(pin2, direccion ? LOW : HIGH);
    analogWrite(PWMPin, velocidad);
  }

  void moverAmbosMotores(int velDer, bool dirDer, int velIzq, bool dirIzq) {
    moverMotor(PWMA, AIN1, AIN2, velDer, dirDer);  // Motor derecho
    moverMotor(PWMB, BIN1, BIN2, velIzq, dirIzq);  // Motor izquierdo
  }

  //Detener el robot
  void Detener() {
    moverAmbosMotores(0, true, 0, true);
  }

  // Movimiento hacia adelante
  void moverAdelante() {
    moverAmbosMotores(VEL_NORMAL, true, VEL_NORMAL, true);
  }

  // Movimiento hacia adelante rápido 
  void moverAdelanteRapido() {
    moverAmbosMotores(VEL_RAPIDA, true, VEL_RAPIDA, true);
  }


  void moverAtras(){
    moverAmbosMotores(VEL_NORMAL, false, VEL_NORMAL, false);
  }

  void moverAtrasRapido(){
    moverAmbosMotores(VEL_RAPIDA, false, VEL_RAPIDA, false);
  }

  void girarIzquierda(){
    moverAmbosMotores(VEL_NORMAL, true, VEL_GIRO_LENTO, false);
  }

  void girarIzquierdaRapido(){
    moverAmbosMotores(VEL_RAPIDA, true, VEL_RAPIDA, false);
  }

  void girarDerecha(){
    moverAmbosMotores(VEL_NORMAL, false, VEL_GIRO_LENTO, true);
  }

  void girarDerechaRapido(){
    moverAmbosMotores(VEL_RAPIDA, false, VEL_RAPIDA, true);
  }

void girarMoviendoseDerecha() {
  moverAmbosMotores(VEL_NORMAL / 2, true, VEL_NORMAL, true);
}

