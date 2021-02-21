# G27_Logitech_PPM_Out
Programa para generar señal PPM desde un arduino con una shield USB Host
para controlar un coche de radiocontrol con el volante Logitech G27
La señal PPM sale por el pin 2 del arduino hacia el puerto trainer de la emisora RC
El volante controla el canal 1 de radiocontrol
El acelerador controla el canal 2 del RC desde 1,5 ms hasta 2020 ms
El freno controla el canal 2 del RC desde 1,5 hasta 0,98 ms
El freno tiene prioridad sobre el acelerador

Por Francisco Carabaza
21/02/2021

Este programa usa la siguiente librería:
https://github.com/FabLabAQ/ArduinoG27
