[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/eiNgq3fR)
[![Open in Visual Studio Code](https://classroom.github.com/assets/open-in-vscode-2e0aaae1b6195c2367325f4f02e2d04e9abb55f0b24a779b69b11b9e10269abc.svg)](https://classroom.github.com/online_ide?assignment_repo_id=22046681&assignment_repo_type=AssignmentRepo)
# TrashBot – Caneca clasificadora automática de residuos (Manuelito)
<!-- Cambiar el titulo "Proyecto" por el nombre del proyecto -->

## Integrantes


* [David Enrique Barón Rubio](https://github.com/Dai-En)
* [Daniel Andrés Ramirez Morales](https://github.com/daramirezmor)
* [Indaliria Valentina Cardona Corredor](https://github.com/valentinacardona07)

<!-- Califico el informe unicamente a los integrantes que esten referenciados aqui y en el informe (PDF) -->

Indice:

- [TrashBot – Caneca clasificadora automática de residuos (Manuelito)](#trashbot--caneca-clasificadora-automática-de-residuos-manuelito)
  - [Integrantes](#integrantes)
  - [Descripción](#descripción)
    - [Planteamiento base del proyecto](#planteamiento-base-del-proyecto)
    - [Enfoque y funcionamiento general](#enfoque-y-funcionamiento-general)
    - [Diseño mecánico y estructura física](#diseño-mecánico-y-estructura-física)
    - [Arquitectura electrónica y sistema embebido](#arquitectura-electrónica-y-sistema-embebido)
    - [Sensores y criterios de clasificación](#sensores-y-criterios-de-clasificación)
    - [Actuadores e interfaz de usuario](#actuadores-e-interfaz-de-usuario)
    - [Lógica de control y programación](#lógica-de-control-y-programación)
    - [Evidencia de funcionamiento](#evidencia-de-funcionamiento)
  - [Informe](#informe)
  - [Implementación](#implementación)
    - [Video de funcionamiento](#video-de-funcionamiento)
    - [Diagrama de flujo de la lógica de la aplicación](#diagrama-de-flujo-de-la-lógica-de-la-aplicación)
    - [Diagrama de bloques del procesador](#diagrama-de-bloques-del-procesador)
    - [Fuentes del proyecto (archivo xsa, Master.xdc y códigos .c utilizados)](#fuentes-del-proyecto-archivo-xsa-masterxdc-y-códigos-c-utilizados)

## Descripción


### Planteamiento base del proyecto

TrashBot, o llamado de cariño por el grupo “Manuelito”, es una caneca clasificadora automática de residuos sólidos en la fuente, diseñada e implementada como un proyecto académico integrador. El sistema permite identificar y separar residuos metálicos, orgánicos y reciclables (principalmente plástico y papel) de manera automática, reduciendo errores humanos en la disposición de los desechos y promoviendo una gestión más eficiente y responsable de los residuos desde su origen.

El desarrollo del proyecto surge a partir de la problemática observada en hogares, instituciones educativas y espacios públicos, donde la separación de residuos suele realizarse de forma incorrecta o incompleta. Esta situación provoca la contaminación de materiales reciclables y el aumento de residuos enviados a rellenos sanitarios. Frente a este contexto, TrashBot se plantea como una solución tecnológica de bajo costo, accesible y replicable, orientada a apoyar la separación en la fuente y a reforzar la educación ambiental mediante el uso de tecnología.

Para su implementación se integraron los conocimientos adquiridos en la asignatura Electrónica Digital II, tanto a nivel teórico como práctico, incluyendo el diseño de sistemas digitales, arquitecturas SoC, comunicación mediante buses, lógica secuencial, control de periféricos y desarrollo de software embebido sobre FPGA. El proyecto busca, además, demostrar la aplicación práctica de estos conceptos en un sistema real y funcional.


### Enfoque y funcionamiento general

TrashBot opera como un sistema autónomo en el cual la clasificación del residuo se realiza en tiempo real a partir de la lectura de sensores físicos y la ejecución de una lógica de decisión secuencial. El usuario únicamente deposita el residuo en la abertura superior, y el sistema se encarga automáticamente de identificar su tipo y dirigirlo al compartimiento correspondiente.

El enfoque adoptado evita el uso de técnicas complejas como visión artificial y prioriza una solución robusta y sencilla basada en sensores físicos y control digital. Esta decisión facilita su implementación en entornos educativos, reduce costos y hace que el funcionamiento del sistema sea más fácil de entender, mantener y replicar.

### Diseño mecánico y estructura física

El diseño mecánico del sistema fue desarrollado en Onshape, donde se modeló una estructura cilíndrica compuesta por un contenedor principal rotatorio con tres subdivisiones internas, correspondientes a cada tipo de residuo. El contenedor se encuentra acoplado a un sistema de piñón y eje accionado por un motor paso a paso, el cual permite rotarlo en incrementos de 120 grados para alinear el compartimiento adecuado.

Adicionalmente, se diseñó una base inferior destinada a alojar la FPGA y la electrónica de control, una tapa superior para regular el ingreso del residuo y diversas piezas auxiliares para la fijación de sensores y actuadores. Todas las piezas fueron fabricadas mediante impresión 3D en PLA y, durante el montaje final, se realizaron ajustes manuales para asegurar la correcta alineación mecánica, el paso del cableado y la lectura adecuada de los sensores.

### Arquitectura electrónica y sistema embebido

El núcleo del sistema es un SoC Zynq-7000 perteneciente a la tarjeta Zybo Z7, el cual integra un procesador ARM y una FPGA en un mismo dispositivo. El procesador ARM se encarga de ejecutar el software de control y gestionar la lógica general del sistema, mientras que la FPGA facilita la interconexión de periféricos y señales externas mediante el bus AXI.

La arquitectura fue diseñada en Vivado mediante un diagrama de bloques que incluye módulos AXI GPIO para la lectura de sensores y el control de actuadores, AXI IIC para la comunicación con la pantalla LCD, bloques de memoria BRAM para almacenamiento interno y un sistema de reset que garantiza una inicialización sincronizada y estable. La asignación de pines físicos se realizó mediante el archivo Master.xdc y el control de los periféricos se implementó a través de acceso a registros por memoria mapeada.

### Sensores y criterios de clasificación

La clasificación de los residuos se realiza mediante un esquema jerárquico de sensores. Un sensor inductivo permite detectar residuos metálicos a partir de inducción electromagnética y tiene la mayor prioridad dentro de la lógica de decisión, asegurando una identificación rápida y confiable de este tipo de material.

Para la identificación de residuos orgánicos se utiliza un sensor de humedad DHT22, el cual se integra al sistema a través de un Arduino Uno que actúa como etapa intermedia. Este microcontrolador se encarga de procesar la medición de humedad y convertirla en una señal digital basada en un umbral previamente definido.

Por otro lado, la identificación de residuos reciclables se realiza mediante un sensor de color TCS3200/TCS230. Este sensor mide las componentes Clear, Red, Green y Blue del objeto detectado, y la clasificación se basa en rangos empíricos y comparaciones relativas entre canales, lo que permite identificar el residuo de forma confiable y reducir falsos positivos.


### Actuadores e interfaz de usuario

El sistema cuenta con un motor paso a paso encargado de rotar el contenedor principal en pasos equivalentes a 120 grados, alineando el compartimiento correspondiente al residuo detectado. Adicionalmente, se utiliza un servomotor que acciona el mecanismo de apertura y cierre para la descarga del residuo, cuya señal de control se genera por software simulando una señal PWM.

Como interfaz de usuario, el sistema incorpora una pantalla LCD 16x2 con comunicación I2C. En esta pantalla se muestra información relevante como el estado del sistema, el tipo de residuo detectado y mensajes de inicialización y operación, permitiendo al usuario comprender fácilmente lo que está ocurriendo durante el proceso de clasificación.

### Lógica de control y programación

La programación del sistema se desarrolló en Vitis utilizando lenguaje C y una estructura modular. Cada sensor y actuador se implementa en archivos independientes, mientras que el archivo principal coordina la lógica general del sistema.

El ciclo de operación inicia con la inicialización del hardware, los sensores y la pantalla LCD. Posteriormente, el sistema realiza la lectura continua de los sensores y detecta eventos mediante flancos de subida, evitando múltiples clasificaciones de un mismo residuo. Dependiendo del tipo de residuo identificado, el sistema ejecuta la secuencia de acciones correspondiente: rotación del contenedor y accionamiento del servomotor para residuos metálicos y orgánicos, o accionamiento directo del servomotor para residuos reciclables. Finalmente, se actualiza la información mostrada en la pantalla y el sistema retorna a un estado de espera.

### Evidencia de funcionamiento

El funcionamiento del sistema fue validado experimentalmente y documentado mediante un video, en el cual se evidencia la correcta detección, clasificación y depósito de los distintos tipos de residuos. El video se encuentra disponible en una sección posterior de este documento.


---

<!-- Descripción general y lo mas completa posible del proyecto" -->

## Informe

El siguiente link envía al informe realizado sobre el planteamiento y desarrollo final de este proyecto, el cual se encuentra como archivo pdf en la carpeta src del repositorio:

[Informe final](/src/archivos_importantes/Proyecto_informe_final__TrashBot.pdf)


<!-- Link que permita acceder al Informe, el cual debe estar subido a este repositorio -->

---
## Implementación

### Video de funcionamiento
El siguiente link permite ir al video donde se demuestra el funcionamiento del proyecto:

[Video de funcionamiento](/src/archivos_importantes/Prueba_de_funcionamiento_proyecto_final_Trashbot.mp4)

Alternativamente, el video está disponible en el siguiente link de youtube:

[Video de funcionamiento en YT](https://youtube.com/shorts/hHHT8F48_9s)

### Diagrama de flujo de la lógica de la aplicación

El siguiente es el diagrama de flujo diseñado por el equipo para plantear la lógica de elección y funcionamiento del proyecto:

![alt text](<src/archivos_importantes/Diagrama de fllujo Manuelito.svg>)

Aunque a lo largo del desarrollo del proyecto se hicieron ajustes en el funcionamiento e inclusive la jerarquía, la lógica general del funcionamiento se mantuvo.

El diagrama también se encuentra disponible en la carpeta src en los siguientes links:

[Diagrama de flujo en svg](/src/archivos_importantes/Diagrama%20de%20fllujo%20Manuelito.svg)

[Diagrama de flujo en png](/src/archivos_importantes/Diagrama%20de%20flujo%20Manuelito.png)

### Diagrama de bloques del procesador 

El diagrama de bloques generado en vivado para el procesador se encuentra disponible en pdf en el siguiente link:

[Diagrama de bloques del procesador](/src/archivos_importantes/Trashbot_diagrama_de_bloques.pdf)

### Fuentes del proyecto (archivo xsa, Master.xdc y códigos .c utilizados)

En la carpeta src del repositorio que se encuentra a continuación se podrán ver todos los archivos mencionados a lo largo de este informe en una carpeta llamada "archivos importantes", y particularmente los códigos y archivos generadores.

En el xdc se encuentra cómo se definieron los pines, con pequeños comentarios indicando qué es cada cual. El archivo xsa presente es el utilizado para generar el entorno de vitis en el que se hizo la aplicación del proyecto.

Finalmente y seguramente lo más importante, se encuentran los códigos .c que se usaron para cada uno de los periféricos y el main del proyecto. En este caso no se realizaron archivos .h para el funcionamiento, sin embargo se realizó un pequeño código en arduino para el correcto funcionamiento del sensor de humedad (Profe no nos baje, la necesidad nos obligó :sob:,
 y solo fue una cosita de nada), el cual también está disponible en el repositorio.


[Fuentes y códigos del proyecto](/src)

<!-- Video explicativo del funcionamiento del proytecto -->


<!-- CREAR UN DIRECTORIO CON EL NOMBRE "src" DONDE INVLUYAN LAS FUENTE (.c Y .h) QUE CREARON PARA EL PROOYECTO-->

<!-- NO OLVIDAD SUBIR EL PDF GENERADOR EN DEL BLOCK DESIGN-->
