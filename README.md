# pic-ir-nec
Microchip decodificador de protocolo IR NEC (Control remoto)

Desarrollado para XC8 y calibrado para PIC12F629

Protocolo NEC: 
- https://www.sbprojects.net/knowledge/ir/nec.php
- https://techdocs.altium.com/display/FPGA/NEC+Infrared+Transmission+Protocol

Caracteristicas:
- Modo bloqueante
- Modo no bloqueante
- Se puede calibrar para cualquier microcontrolador

Requerimientos:
- Timer
- Para modo no bloqueante se requiere activar interrupcion en pin

Directorios:
- Src = Contiene el codigo fuente de la libreria y dos ejemplos, bloqueante y no bloqueante
- Proteus = Contiene los archivos para simulador Isis de proteus

Ejemplo de calibracion
![alt text](https://raw.githubusercontent.com/gemunet/pic-ir-nec/master/ex-calibration.png)
