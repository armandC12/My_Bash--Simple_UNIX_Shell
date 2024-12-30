<p align="center">
  <a href="https://example.com/">
    <img src="https://upload.wikimedia.org/wikipedia/commons/thumb/4/41/CABJ70.png/138px-CABJ70.png" alt="Logo" width=72 height=72>
  </a>

  <h3 align="center">Lab1 : Mybash</h3>

  <p align="center">
    Sistemas Operativos 2022
    <br>
    <a href="[https://reponame/issues/new?template=bug.md](https://docs.google.com/document/d/1PT_Q5gnSODGS0kbaEvy7onlZe7M2GtjZNo5gi-RCUQM/edit#)">Consignas</a>
    ·
    <a href="https://bitbucket.org/sistop-famaf/so22lab1g17/src/master/">Repositorio</a>
  </p>
</p>


## Table of contents

  - [Introducción](#introducción-1)
  - [Modularización](#modularización)
  - [Técnicas de programación](#técnicas-de-programación)
  - [Desarrollo](#desarrollo)
  - [Concluciones](#concluciones)


## Introducción

Éste informe trata sobre el trabajo realizado para la primer entrega del laboratorio, **Laboratorio 1 - Mybash**, en la cual empezamos con la creación de nuestro propio shell de Linux. Somos el grupo n° 17, integrado por:

- Fernando Alvarado, fernando.alvarado@mi.unc.edu.ar
- Gonzalo Martyn, gonzalo.martyn@mi.unc.edu.ar
- Ivan Ponzio, ivan.ponzio@mi.unc.edu.ar
- Armando Carral, armando.carral@mi.unc.edu.ar

## Modularizacion

Para la realización del shell, decidimos seguir con la modularización en 6 partes, planteada por los profesores en el enunciado. 

### **Mybash:**
    El modulo principal es el llamado **_mybash_**, en el cual se encuentra la llamada general a nuestro shell. Se obtiene input del usuario a traves de la entrada estándar. Ésta representa un comando simple, o dos unidos por un pipe ("|").



![Estructura Pipeline](/Imagenes/EstructuraPipeline.jpeg "Estructura Pipeline")
![Estructura Scommand](/Imagenes/EstructuraScommand.jpeg "Estructura Scommand")


### **Command:**
    En el modulo command se presentan las herramientas para la conversión y el manejo de TADs (scommand, de comando simple, y spipeline para pipelines). Dentro de este modulo, se implementaron distintas funciones para la creación, destrucción, agregado de cadenas, quita y consulta del primer elemento, establecimiento y consulta de las redirecciones, y funciones de debugging.

### **Parser:**
    El modulo parser, provisto por la cátedra, brinda instrumentos para la interpretación de la entrada estándar, transformándola a una del tipo parser. 

### **Parsing:**
    Continuamos con el modulo parsing, donde implementamos las funciones para el procesamiento e individualización del comando, a partir del tipo parser. Para ello, dividimos en 2 funciones distintas, una que parsea los comandos simples y otra los pipelines. 

### **Builtin:**
    En el modulo builtin se busca detectar si un scommand contiene un comando interno, y si un pipeline contiene un solo comando y es interno, a su vez, también se implementa una función para efectuar tal comando.

### **Execute:**
    Una vez parseados los comandos y creada la instancia TAD, planteamos los dos módulos que se encargarán de la ejecución. El modulo execute, es el creado para la ejecución de los comandos, a traves de distintas llamadas al sistema. 




## Tecnicas de Programacion


Decidimos utilizar la librería GLib, más específicamente el tipo GSList, para las listas, ya que puede ser polimorfica y tomar el tipo que le sea conveniente, sea una lista de comandos o lista de cadenas. A la hora de asegurar el correcto funcionamiento de la shell, para el caso de pre y post condiciones de cada una de las funciones implementadas, utilizamos la función assert de la librería de C. Para el caso de las llamadas al sistema, cada una de las funciones que las efectúe, esta preparada para el manejo de errores, y, según las fallas posibles que surjan, imprime el error en pantalla, cancela la operación, aborta el programa, y/o prueba de vuelta.
A la hora del manejo de memoria, había que ser cuidadosos, sobre todo con el uso de las distintas funciones de GSList. Para cada vez que se utilizaban funciones que pedían memoria, se libera la correspondiente, y en otros casos como en el de la función strextra que se nos fue otorgada, modularizamos creando otra función que libere la memoria que ésta última dejaba. 


## Desarrollo

 Comenzamos a implementar nuestra _mybash_:
-  Primero con el modulo scommand, el cual cada uno implemento cinco funciones teniendo que usar la libreria GLib, proporcionada por los profesores. Luego, juntamos la implementaciones y testeamo si dicha funciones estaban bien implementadas, para eso ejecutamos los test proporcionados. Por ultimo chequeamos los memory leaks, el cual tuvimos bastante, pero los solucionamos al final despues de finalizar los demas modulos.


- En modulo parsing creamos dos funciones:
  - La funcion **parse_scommand**, la cual es una funcion estatica,que devuelve un scommand. Lo que hace la funcion es fijarse si el parser es un argume to normal o de entrada/salida. Si el argumento es normal, entonces lo agrega al  scommand, sino llamamos a la funcion scommand_set_redir_in/out (dependiendo si es entrada o salida) y asi hasta que se topa con un pipe ("|").
  Si falla algo y no se leyo un scommand se libera recursos
  
  - La funcion **parse_pipeline**, toma un Parser y devuelve un Pipeline, que lo que hace es unir los scommand que da la funcion parse_scommand y agregarlas en el pipeline que devuelve la funcion, mediante las funciones de parser.h.
  una vez no tengamos mas scommand que agregar o no haya mas pipe, verifica que hay despues de eso, por ejemplo si hay un & o otros comandos invalidos, si hay un & el programa va a esperar, caso contrario continua, por ultimo si el resultado del pipeline es vacio, se destruye.

- En el modulo builtin teniamos tres funciones a implementar:
  - Las funciones **builtin_is_internal** y builtin_run las implementamos con sentencias, la primera funcion tenie que chequear si el comando corresponde a los comando internos cd, help y exit. La segunda funcion recibe un comando y verifica si se corresponde a cd, help o exit para luego ingresar a ese caso y hacer la ejecucion del mismo.

  - En la función **builtin_alone**, hicimos uso de las funciones pipeline_length para corrobar si pipeline tiene un solo comando, y builtin_is_internal para saber si es un comando interno.


- Para el execute teníamos que usar las llamadas al sistema para ejecutar  
  todos los comandos contenidos en un pipeline.
  Lo primero que hace la función execute_pipeline es chequear si el comando es interno y si es uno solo. 
  Si lo es, hace uso de la función que corre comandos internos, implementada en builtin.c .
 
  En el caso de que no sea interno, se analiza si el pipeline tiene un solo comando y de ser así, se arma el vector de argumentos y se hace un fork.
  El hijo realiza un execvp y el padre espera en el caso de tener que hacerlo.

  Cuando el pipeline tiene dos argumentos se crea un pipe haciendo uso de la llamada al sistema con dicho nombre. 
  Se conecta la salida estandar del primer hijo a la punta de escritura del pipe con dup2, se arma vector de argumentos y se hace un excevp.
  El proceso padre vuelve a hacer otro fork. En el segundo hijo se configura para que la entrada estandar lea en la punta de lectura del pipe.
  Comunicando así a ambos procesos; el output del primero se convierte en el input del segundo.
  En ambos hijos se setean las redirecciones independientemente.

  Empezamos una generalización para pipes de n scommand, (código en la rama multiple_pipes del repositorio). No alcanzamos a terminarla, por lo que el último caso de
  la función execute_pipeline printea que no puede ejecutar mas de 2 scommand y devuelve el prompt.
![Execute](/Imagenes/Execute.jpeg "Execute")


## Conclusiones

A modo de conclusión, queremos decir que nos encontramos satisfechos con el trabajo realizado. Pudimos resolver en conjunto y de forma individual, cada uno de los problemas que vimos surgir durante la prueba de cada unidad y general de nuestra shell, sean de errores de ejecución, como de manejo de memoria, siendo de mucha ayuda la guía por parte de los distintos profesores de la cátedra. Por otro lado, nos sentimos cómodos trabajando con bitbucket y no vimos muchas dificultades al respecto.

## Thanks

Some Text

