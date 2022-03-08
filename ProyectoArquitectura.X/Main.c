/*
 * Main para los dispositivos:
 *      Sensor de Temperatura
 *      Reloj tiempo real RTC
 *      Leds 
 *		Bombillas
 *		LCD 16x2
 *		I2C
 *		Virtual terminal
 * Se utiliza el Microcontrolador PIC 18F4550
 */

/*!
\file   Main.c
\date   2022-08-03
\author Jesus Edwin Adrada Ruiz <jesusadrada@unicauca.edu.co> 104617020514
        Martha Jojhana Villota Hernández <vmartha@unicauca.edu.co> 104615021435
        Edward Alexander Rivera Urrutia <edwardrivera@unicauca.edu.co> 104614011298
        Juan Yela <juanyelaa@unicauca.edu.co> 104617020774
\brief  Example Controler .
 */
/*==========================================================================================================
 * Paquetes importados.
 ===========================================================================================================*/
#include "Bits_Configuracion.h"            // Incluimos los bits de configuración.
#include "I2C_librery.h"                   // Incluimos la librería I2C.
#include "LCD_librery.h"                   // Incluimos la librería para la pantalla LCD.
#include <stdio.h>                         // Incluimos la librería stdio.
#include <stdint.h>                        // Incluimos la librería stdint.
#include "adc_header.h"                    // Incluimos la libreria ADC.
/*==========================================================================================================
 * Variables globales
 ===========================================================================================================*/
uint8_t hours;                             // Variable de 8 bits para las horas.
uint8_t minutes;                           // Variable de 8 bits para los minutos.
uint8_t seconds;                           // Variable de 8 bits para los segundos.
uint8_t year;                              // Variable de 8 bits para los año.
uint8_t month;                             // Variable de 8 bits para el mes.
uint8_t day;                               // Variable de 8 bits para el dia.
uint8_t i;                                 // Variable de 8 bits
static char buffer_TX[] = "TEMP: xx,xx grados HORA: xx:xx:xx DATE: xx/xx/xx\r\n";    // Arreglo estatico que se imprime en la consola
/*==========================================================================================================
 * Declaracion de funciones.
 ===========================================================================================================*/
uint8_t Decimal_a_BCD (uint8_t numero);    // Función que convierte un número decimal a BCD. 
uint8_t BCD_a_Decimal (uint8_t numero);    // Función que convierte un número BCD a decimal.
void Reloj_Calendario (void);              // Función de visualización de HORA Y FECHA.
void Establecer_Hora (void);               // Funcion para establecer la HORA Y FECHA.
void Mostrar_Temperatura (void);           // Funcion para mostrar la temperatura en el LCD.
float Obtener_Temperatura (void);          // Obtiene la temperatura del sensor LM35.
void Imprimir_Cadena (int, int);           // Imprime los datos de la cadena por consola.
void Encender_Act(float);                  // Enciende los actuadores segun convenga.
/*==========================================================================================================
 * Nombre: BCD_a_Decimal.
 * Fucion: Convierte un número BCD a decimal.
 * Parametro de entrada: numero.
 * Tipo de variable de entrada: unit8_t (variable de 8 bits).
 * Tipo de variable de salida: unit8_t (variable de 8 bits).
 ===========================================================================================================*/
uint8_t BCD_a_Decimal (uint8_t numero)
{
  return ((numero >> 4) * 10 + (numero & 0x0F));  // Retornamos la variable "numero" desplazado 4 posiciones a la izquierda, multiplicado por 10 más "numero" &&  1111.
}
/*==========================================================================================================
 * Nombre: Decimal_a_BCD.
 * Fucion: Convierte un número decimal a BCD.
 * Parametro de entrada: numero.
 * Tipo de variable de entrada: unit8_t (variable de 8 bits).
 * Tipo de variable de salida: unit8_t (variable de 8 bits).
 ===========================================================================================================*/
uint8_t Decimal_a_BCD (uint8_t numero) 
{
    return (((numero / 10) << 4) + (numero % 10));// Retornamos la decena de la variable "numero" desplazado 4 bits a la derecha y las unidades de la variable "numero" en el nibble menos significativo
}
/*==========================================================================================================
 * Nombre: Reloj_Calendario.
 * Fucion: Visualización de HORA Y FECHA atravez de la pantalla LCD.
 * Tipo de variable de entrada: void.
 * Tipo de variable de salida: void.
 ===========================================================================================================*/
void Reloj_Calendario (void)
{
    static char Time[] = "TIME: 00:00:00";  // Arreglo estático char "Time". 
    static char Date[] = "DATE: 00/00/2000";// Arreglo estático char "Date".
  
    seconds = BCD_a_Decimal(seconds);       // Llamamos a la función de conversión de BCD a Decimal.
    minutes = BCD_a_Decimal(minutes);       // Llamamos a la función de conversión de BCD a Decimal.
    hours   = BCD_a_Decimal(hours);         // Llamamos a la función de conversión de BCD a Decimal.
    day     = BCD_a_Decimal(day);           // Llamamos a la función de conversión de BCD a Decimal.
    month   = BCD_a_Decimal(month);         // Llamamos a la función de conversión de BCD a Decimal.
    year    = BCD_a_Decimal(year);          // Llamamos a la función de conversión de BCD a Decimal.
    
    Time[6] =  hours   / 10 + '0';          // Guardamos las decenas de la variable "hours" en la posición 6 del arreglo "Time".
    Time[7] =  hours   % 10 + '0';          // Guardamos las unidades de la variable "hours" en la posición 7 del arreglo "Time".
    Time[9] =  minutes / 10 + '0';          // Guardamos las decenas de la variable "minutes" en la posición 9 del arreglo "Time".
    Time[10]=  minutes % 10 + '0';          // Guardamos las unidades de la variable "minutes" en la posición 10 del arreglo "Time".
    Time[12]=  seconds / 10 + '0';          // Guardamos las decenas de la variable "seconds" en la posición 12 del arreglo "Time".
    Time[13]=  seconds % 10 + '0';          // Guardamos las unidades de la variable "secods" en la posición 13 del arreglo "Time".
    
    Date[6] =  day     / 10 + '0';          // Guardamos las decenas de la variable "day" en la posición 6 del arreglo "Date".
    Date[7] =  day     % 10 + '0';          // Guardamos las unidades de la variable "day" en la posición 7 del arreglo "Date".
    Date[9] =  month   / 10 + '0';          // Guardamos las decenas de la variable "month" en la posición 9 del arreglo "Date".
    Date[10]=  month   % 10 + '0';          // Guardamos las unidades de la variable "month" en la posición 10 del arreglo "Date".
    Date[14]=  year    / 10 + '0';          // Guardamos las decenas de la variable "year" en la posición 14 del arreglo "Date".
    Date[15]=  year    % 10 + '0';          // Guardamos las unidades de la variable "year" en la posición 15 del arreglo "Date".
    
    LCD_Goto(1, 1);                         // Cursor en fila 1, columna 1.
    LCD_Print(Time);                        // Imprimimos en la pantalla LCD el valor del arreglo "Time".
    LCD_Goto(1, 2);                         // Cursor en fila 2, columna 1.
    LCD_Print(Date);                        // Imprimimos en pantalla LCD el valor del arreglo "Date".
    
    buffer_TX[25] = Time[6];                // Guardamos las decenas de las horas de la posicion 6 del arreglo time en la posicion 25 de la cadena de la consola.
    buffer_TX[26] = Time[7];                // Guardamos las unidades de las horas de la posicion 7 del arreglo time en la posicion 26 de la cadena de la consola.
    buffer_TX[28] = Time[9];                // Guardamos las decenas de los min de la posicion 9 del arreglo time en la posicion 28 de la cadena de la consola.
    buffer_TX[29] = Time[10];               // Guardamos las unidades de los min de la posicion 10 del arreglo time en la posicion 29 de la cadena de la consola.
    buffer_TX[31] = Time[12];               // Guardamos las decenas de los seg de la posicion 12 del arreglo time en la posicion 31 de la cadena de la consola.
    buffer_TX[32] = Time[13];               // Guardamos las unidades de los min de la posicion 13 del arreglo time en la posicion 32 de la cadena de la consola.
    
    buffer_TX[40] = Date[6];                // Guardamos las decenas de los dias de la posicion 6 del arreglo date en la posicion 40 de la cadena de la consola.
    buffer_TX[41] = Date[7];                // Guardamos las unidades de los dias de la posicion 7 del arreglo date en la posicion 41 de la cadena de la consola.
    buffer_TX[43] = Date[9];                // Guardamos las decenas de los meses de la posicion 9 del arreglo date en la posicion 43 de la cadena de la consola.
    buffer_TX[44] = Date[10];               // Guardamos las unidades de los meses de la posicion 10 del arreglo date en la posicion 44 de la cadena de la consola.
    buffer_TX[46] = Date[14];               // Guardamos las decenas de los años de la posicion 14 del arreglo date en la posicion 46 de la cadena de la consola.
    buffer_TX[47] = Date[15];               // Guardamos las unidades de los años de la posicion 15 del arreglo date en la posicion 47 de la cadena de la consola.
}
/*==========================================================================================================
 * Nombre: Establecer_Hora.
 * Fucion: Establece la FECHA y HORA desde el I2C para luego llamar la funcion Reloj_Calendario.
 * Tipo de variable de entrada: void.
 * Tipo de variable de salida: void.
 ===========================================================================================================*/
void Establecer_Hora (void)
{
    I2C_Start();                        // Llamamos a la función Start.
    I2C_Write(0xD0);                    // Escribimos en SSPBUF la dirección de DS1307 1101000 + 0 de escritura.
    I2C_Write(0);                       // Dirección de segundos.
    I2C_ReStart();                      // Llamamos a la función ReStart.
    I2C_Write(0xD1);                    // Escribimos en SSPBUF la dirección de DS1307 1101000 +1 de lectura.
    seconds=I2C_Read();                 // Cargamos la variable "seconds" con el valor de SSPBUF.
    I2C_Ack();                          // ACK.
    minutes=I2C_Read();                 // Cargamos la variable "minutes" con el valor de SSPBUF.
    I2C_Ack();                          // ACK.
    hours=I2C_Read();                   // Cargamos la variable "hours" con el valor de SSPBUF.
    I2C_Ack();                          // ACK.
    I2C_Read();                         // Valor no leido.
    I2C_Ack();                          // ACK.
    day=I2C_Read();                     // Cargamos la variable "day" con el valor de SSPBUF.
    I2C_Ack();                          // ACK.
    month=I2C_Read();                   // Cargamos la variable "month" con el valor de SSPBUF.
    I2C_Ack();                          // ACK.
    year=I2C_Read();                    // Cargamos la variable "year" con el valor de SSPBUF.
    I2C_NO_Ack();                       // NO ACK.
    I2C_Stop();                         // Llamamos a la función Stop.
       
    Reloj_Calendario();                 // Llamamos a la función Reloj_Calendario().
    
    Encender_Act(Obtener_Temperatura());                 // Llama la funcion para encender los actuadores.
    __delay_ms(50);                     // Delay de 500 milisegundos.
}
/*==========================================================================================================
 * Nombre: Mostrar_Temperatura.
 * Fucion: Obtiene la temperatura, la muestra atravez de la pantalla LCD y la envia a la funcion Encender_Act.
 * Tipo de variable de entrada: void.
 * Tipo de variable de salida: void.
 ===========================================================================================================*/
void Mostrar_Temperatura (void)
{
    static char temperatura [4];        // Arreglo estatico char "temperatura".    
    float temp = Obtener_Temperatura(); // Variable flotante de la temperatura.
    
    LCD_Goto(1,1);                      // Cursor en fila 1, columna 1.
    LCD_Print("Temperatura:");          // Imprimimos en la pantalla LCD "Temperatura:".
    sprintf(temperatura,"%.2f",temp);   // Funcion sprintf para guardar en el arreglo temperatura los datos de temp.
    LCD_Goto(6,2);                      // Cursor en fila 2, columna 6.
    LCD_Print(temperatura);             // Imprimimos en la pantalla LCD el valor del arreglo "temperatura".
    LCD_Goto(12,2);                     // Cursor en fila 2, columna 12.
    LCD_Print("C");                     // Imprimimos en la pantalla LCD "C".

    Encender_Act(temp);                 // Llama la funcion para encender los actuadores.
    
    buffer_TX[6] = temperatura[0];      // Guardamos las decenas de la temperatura de la posicion 0 del arreglo temperatura en la posicion 6 de la cadena de la consola.
    buffer_TX[7] = temperatura[1];      // Guardamos las unidades de la temperatura de la posicion 1 del arreglo temperatura en la posicion 7 de la cadena de la consola.
    buffer_TX[9] = temperatura[3];      // Guardamos el primer valor despues de la coma de la temperatura de la posicion 3 del arreglo temperatura en la posicion 9 de la cadena de la consola.
    buffer_TX[10] = temperatura[4];     // Guardamos el segundo valor despues de la coma de la temperatura de la posicion 4 del arreglo temperatura en la posicion 10 de la cadena de la consola.
    
    __delay_ms(50);                     // Delay de 50 milisegundos.
}
/*==========================================================================================================
 * Nombre: Obtener_Temperatura.
 * Fucion: Obtiene y convierte el voltaje que envia el LM35 por el puerto RA0/AN0.
 * Tipo de variable de entrada: void.
 * Parametro de salida: temp.
 * Tipo de variable de salida: float.
 ===========================================================================================================*/
float Obtener_Temperatura (void)
{
    float temp = 0;                     // Variable flotante de la temperatura.
    int value_adc = 0;                  // Variable entera para valor del voltaje arrojado por el lm35.

    value_adc = ADC_Read(0);            // Guardamos en la variable value_adc en valor del puerto analogico 0.
    temp = value_adc;                   // Guardamos en la variable temp el valor de value_adc.
    temp = (temp * 500.0) / 1023.0;     // Usamos la ecuacion para convertirlo el voltaje en lectura de grados.
    
    return temp;                        // Retorna la temperatura.
}
/*==========================================================================================================
 * Nombre: Imprimir_Cadena.
 * Fucion: Imprime los datos de temperatura, fecha y hora en la consola de salida.
 * Tipo de variable de entrada: void.
 * Tipo de variable de salida: void.
 ===========================================================================================================*/
void Imprimir_Cadena(int a, int b)
{
    for (int i = a; i < b; i++) 
    {
        // Espera a que el registro de transmisión este disponible o vacio.
        while (!TXSTAbits.TRMT) {
        }
        // Escribe el dato que se enviará a través de TX.
            TXREG = buffer_TX[i];
    }
}
/*==========================================================================================================
 * Nombre: Encender_Act.
 * Fucion: Recibe la temperatura para evaluarla y decidir que actuadores encender.
 * Parametro de entrada: temp.
 * Tipo de variable de entrada: float.
 * Tipo de variable de salida: void.
 ===========================================================================================================*/
void Encender_Act(float temp)
{
    if(temp>=32 && temp<=34.9)          // Condicional para verificar la temperatura correcta.
    {
        RB2 = 0;                        // Envia 0 logico por el puerto RB2 del PIC18F4550 apaga dispensador.
        RB3 = 1;                        // Envia 1 logico por el puerto RB3 del PIC18F4550 enciende led verde.
        RB4 = 0;                        // Envia 0 logico por el puerto RB4 del PIC18F4550 apaga led rojo.
        RB5 = 0;                        // Envia 0 logico por el puerto RB5 del PIC18F4550 apaga bombillas.
    }
    else                                // En caso de no ser la temperatura correcta entra aaqui..
    {
        if(temp > 34.9)                 // Condicional para verificar si temperatura esta por encima de la correcta.
        {
            RB2 = 1;                    // Envia 1 logico por el puerto RB2 del PIC18F4550 enciende dispensador.
            RB5 = 0;                    // Envia 0 logico por el puerto RB5 del PIC18F4550 apaga bombillas.
        } 
        else                            //  En caso de de que la temperatura este por debajo de la correcta entra aqui.
        {
            RB2 = 0;                    // Envia 0 logico por el puerto RB2 del PIC18F4550 apaga dispensador.
            RB5 = 1;                    // Envia 1 logico por el puerto RB5 del PIC18F4550 enciende bombillas.
        }
        RB3 = 0;                        // Envia 1 logico por el puerto RB5 del PIC18F4550 apaga led verde.
        RB4 = 1;                        // Envia 1 logico por el puerto RB5 del PIC18F4550 enciende led rojo.
    }
}
/*==========================================================================================================
 * Main del proyecto.
 ===========================================================================================================*/
void main(void)                             // Función Principal.
{
    I2C_Init(100000);                       // Inicializamos la comunicación I2C con una frecuencia de 100KHz. para el DS1307.
    LCD_Begin();                            // Inicializamos la pantalla LCD 16x2.
    ADC_Init();                             // Inicializa en ADC o las funciones de conversion analogodigital.

    TRISBbits.RB2 = 0;                      // Pin RB2 como salida digital para dispensador.
    TRISBbits.RB3 = 0;                      // Pin RB3 como salida digital para led verde.
    TRISBbits.RB4 = 0;                      // Pin RB4 como salida digital para led rojo.
    TRISBbits.RB5 = 0;                      // Pin RB5 como salida digital para bombillas.
    
    //Configura Fosc = 8Mhz interno.
    OSCCONbits.IRCF = 0b111;                // Configura oscilador interno (FOSC = 8Mhz).
    OSCCONbits.SCS = 0b10;                  // Fuente de Fosc del sistema = interno.
    
     //Configura UART a 9600 baudios.
    TRISCbits.RC6 = 0;                      // Pin RC6 como salida digital para TX.
    TXSTAbits.TX9 = 0;                      // Modo-8bits.
    TXSTAbits.TXEN = 1;                     // Habilita Transmisión.
    TXSTAbits.SYNC = 0;                     // Modo-Asíncrono.
    TXSTAbits.BRGH = 0;                     // Modo-Baja Velocidad de Baudios.
    BAUDCONbits.BRG16 = 0;                  // Baudios modo-8bits.
    RCSTAbits.SPEN = 1;                     // Hbilita el Módulo SSP como UART.
    SPBRG = (unsigned char) (((_XTAL_FREQ / 9600) / 64) - 1); //baudios  = 9600.
    
    int cont = 0;                           // Variable entera contador.
    
    while(1)                                // Bucle infinito.
    {
        cont = 400;                         // Establece el contador en 400.
        while(cont>0)                       // Ciclo while para mientras en contador sea mayor a 0 se mostrada la temperatura.
        {
            Mostrar_Temperatura();          // Call de la funcion Mostrar_Temperatura.
            cont = cont - 5;                // Disminuye el contador en 5.
        }
        Imprimir_Cadena(0,18);              // Call de la funcion Imprimir_Cadena para la temperatura.
        
        LCD_Clear();                        // Call de la funcion para limpiar la pantalla LCD.
        __delay_ms(500);                    // Delay de 500 milisegundos.

        cont = 200;                         // Establece el contador en 200.
        while(cont>0)                       // Ciclo while para mientras en contador sea mayor a 0 se mostrada la fecha.
        {                      
            Establecer_Hora();              // Call de la funcion Establecer_Hora.
            cont = cont - 5;                // Disminuye el contador en 5.
        }
        Imprimir_Cadena(18,51);             // Call de la funcion Imprimir_Cadena para la hora y fecha.
        
        LCD_Clear();                        // Call de la funcion para limpiar la pantalla LCD.
        __delay_ms(500);                    // Delay de 500 milisegundos.
    }
}