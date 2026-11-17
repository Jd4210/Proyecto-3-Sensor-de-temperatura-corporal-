#include <Arduino.h>
/*
   Terminal Module is like a chat box. It allows you to send and receive commands between your
   board and smartphone.

   You can reduce the size of library compiled by enabling only those modules that you
   want to use. For this first define CUSTOM_SETTINGS followed by defining
   INCLUDE_modulename.

   Explore more on: https://thestempedia.com/docs/dabble/terminal-module/
*/
#define CUSTOM_SETTINGS
#define INCLUDE_TERMINAL_MODULE
#include <DabbleESP32.h>

#define RXD2 16
#define TXD2 17

String respuesta = "";

void MostrarMenu()
{
    Terminal.println("-------------------------");
    Terminal.println("MONITOR DE TEMPERATURA");
    Terminal.println("1. Obtener temperatura");
    Terminal.println("2. Guardar temperatura");
    Terminal.println("-------------------------");
}

void setup()
{
    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

    Dabble.begin("MyEsp32.1");
}

void loop()
{
    Dabble.processInput();

    // Recibir comando Bluetooth
    while (Terminal.available() > 0)
    {
        char comando = Terminal.read();

        // Ignorar saltos de linea
        if (comando == '\n' || comando == '\r')
        {
            continue;
        }

        // Solicitar temperatura
        if (comando == '1')
        {
            Serial2.write((uint8_t)1);

            Terminal.println("Obteniendo temperatura...");
        }

        // Solicitar almacenamiento
        else if (comando == '2')
        {
            Serial2.write((uint8_t)2);

            Terminal.println("Guardando temperatura...");
        }

        // Comando invalido
        else
        {
            Terminal.println("Comando invalido.");
            Terminal.println("Por favor presione 1 o 2.");

            MostrarMenu();
        }
    }

    // Recibir respuesta desde Nucleo 1
    while (Serial2.available() > 0)
    {
        char dato = Serial2.read();

        // // Almacenamiento exitoso
        // if (dato == '3')
        // {
        //     Terminal.println("Temperatura guardada exitosamente.");

        //     MostrarMenu();
        // }

        // // Error de almacenamiento
        // if (dato == '4')
        // {
        //     Terminal.println("Error: No se pudo guardar la temperatura.");

        //     MostrarMenu();
        // }

        // Recibir texto de temperatura
        if (dato == '\n')
        {
            Terminal.println(respuesta);
            respuesta = "";

            MostrarMenu();
        }
        else if (dato != '\r')
        {
            respuesta += dato;
        }
    }
}