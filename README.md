# PipoCompiler

PipoCompiler es un compilador implementado con Lex y Yacc para el lenguaje PipoScript.

PipoScript es un lenguaje tipado con sintaxis basada en C. Cuenta con 3 tipos de datos principales, números enteros, cadenas de caracteres y etiquetas. Cuenta con funcionalidades como gran parte de las operaciones aritméticas ofrecidas por C, funciones nativas para el manejo de strings, capacidad para generar y separar funcionalidades disjuntas en funciones y a lo largo de múltiples archivos.

Una explicacion mas detallada del lenguaje y el proceso de desarrollo se puede encontrar en `Informe.pdf`.

## Authors

- [Brandy Tobias](https://github.com/tobiasbrandy)
- [Pannunzio Faustino](https://github.com/Fpannunzio)
- [Sagues Ignacio](https://github.com/isagues)
- [Vazquez Ignacio](https://github.com/igvazquez)

## Estructura del codigo

Dentro de la carpeta raiz se encuentran unicamente los archivos necesarios para la utilizacion de Lex y Yacc, `pipoScript.l` y `pipoScript.y` respectivamente. Estos archivos definen las reglas a ser utilizadas en el proceso de analisis léxico y sintáctico realizado por el compilador.

Ademas de estos 2 archivos, el compilador esta formado por 4 modulos.

### Compiler Utils

Si bien los archivos ya mencionados son lo minimo necesario para la utilizacion de Lex y Yacc, para poder hacer el manejo y procesado de una segunda pasada de compilacion es necesario definir y utilizar estructuras mas complejas. Es por esto que consideramos modularizar el codigo y realizar un manejo mas claro de las distintas estructuras.

Estos modulos estan compuestos por

- Args Handler -> Utilizado para recibir e interpretar los parametros pasados al compilador.
- AST -> Implementacion propia y a medida de un Abstract Syntax Tree.
- Function y Symbol Table -> Estructuras para el manejo de variables, funciones y sus respectivos valores.
- Input Files Handler -> Simple implementacion de una [cola](https://en.wikipedia.org/wiki/Queue_(abstract_data_type)) para el manejo y procesado de los archivos fuentes para compilar.
- Lex y Yacc Utils -> Pequeñas funcionalidades utilizadas por Lex y Yacc separadas de los archivos originales en pos de claridad.

### Pipo Script Functions

Sumado a las funciones de compilacion, las cuales tienen un caracter mas general, existen otras que son necesarias exclusivamente para el manejo de PipoScript. Estas ultimas se encuentran en el modulo `pipoScriptFunctions`. Consta de 2 partes principales. 

Por un lado se encuentra el manejo de tags, estructura de datos central ofrecida por el lenguaje. Para esto es necesario definir la estructura de datos subyacente e implementar funciones para manipularla de manera simple y correcta. 

Por otro lado, se encuentra el manejo de strings. Para liberar de la responsabilidad del manejo de la memoria al usuario a la hora de utilizar strings es necesario tener una estructura lo administre. Principalmente que se encargue de almacenar, manipular y, cuando termina la ejecucion, liberar los recursos necesarios.

### Utils

Este modulo consta de utilidades de caracter general. Actualmente la unica funcionalidad ofrecida es `errorHandling` que provee funciones para cortar la ejecucion de manera centralizada y asegurandose de liberar todos los recursos. Ademas, ofrece funciones wrappers para solicitar memoria dinamica y cortar ejecucion en caso de error.
### Libraries

Teniendo en cuenta que no es necesario reinventar la rueda, muchas veces es util aprovechar y utilizar librerias externas. Si bien en este caso, la unica libreria empleada es [KHash](https://github.com/attractivechaos/klib/blob/master/khash.h), consideramos que el codigo que no es de nuestra autoria deberia estar claramente separado. 

## Compilacion

Teniendo en cuenta la estructura del codigo ya detallada, se evidencia la necesidad de utilizar una herramienta que asista en el proceso de compilacion. Para esto se utilizo GNU Make y se definio una serie de recetas para llevar a cabo este proceso de manera automatica.

Para compilar todo el compilador, valga al redundancia, se utiliza `make all` o `make`. Esta rutina se encargara de generar y ejecutar a Lex y Yacc y posteriormente, compilar los archivos resultantes junto a todos los modulos ya detallados. Ademas, se ofrecen algunas opciones a la hora compilar.

### Compilador

Por default se utiliza como compilador cc. Este puede ser modificado definiendo la variable de entorno `CC` con el nombre del compilador a utilizar. Esto puede realizarse exportando la variable `export CC=clang` o definiendola al momento de ejecutar la rutina de compilacion `CC=clang make`.

### Flags de compilacion

Para asegurarnos que el proceso de compilacion se realice de manera correcta sin generar errores se definieron una serie de flags de compilacion obligatorios. Estos son `-Wall -pedantic -Wno-newline-eof -D_POSIX_C_SOURCE -std=c99 -I$(PWD)`. Sumado a los flags obligatorios, se utiliza otra serie de flags utiles para el desarrollo del proyecto, estos son `-ggdb -fsanitize=address -fno-omit-frame-pointer`. Teniendo en cuenta que estos pueden no ser de interes, es posible deshabilitarlos definiendo la variable `XFLAGS` vacia o, en caso de querer agregar flags de compilacion propios, con aquellos que sean de interes.

### Limpieza

Ademas de `make all` se ofrece `make clean` para hacer la limpieza de todos los archivos generados en el proceso de compilacion.

## Utilizacion del compilador

Por defecto el ejecutable del compilador se generara en la carpeta raiz y se llamara `pipoCompiler`. Aunque, esto puede ser modificado definiendo la variable de entorno COMPILER si se desea generar el compilador con otro nombre. Para su utilizacion basta con llamar al mismo pasando como parametro los archivos fuentes a compilar. Como resultado, en caso de exito, se obtendra un unico archivo HTML llamado `index.html`, a menos que se especifique otro nombre de salida.

En caso de querer tener un mayor control en el proceso de compilacion, el compilador ofrece una serie de parametros que pueden ser utilizados a la hora de ejecutarlo. La lista de los mismos junto a su explicacion puede ser obtenida ejecutando `./pipoCompiler -h` o accediendo al man del compilador (`pipoCompiler.8`).

Tener en cuenta que, para que los archivos fuentes de compilacion sean considerados validos, estos tienen que tener nombre terminado en `.pipo`.

Un ejemplo de compilacion seria `./pipoCompiler -o profile.html main.pipo cssHelper.pipo myLibrary.pipo`.

## Ejemplos

Para hacer una pequeña demostracion de las capacidades del lenguaje y el correcto funcionamiento del compilador se incluye en la carpeta `examples` una serie de ejemplos listos para ser procesados por el compilador. De todas formas, para hacer este proceso un poco mas sencillo, se ofrece como rutina de Make `make examples`. Esta se encarga de compilar de manera correcta todos los ejemplos y generar el archivo HTML de salida con el nombre correspondiente. 

> **Pro tip:** si se desea compilar un programa con multiples archivos, basta con generar una nueva carpeta en el modulo de examples con el nombre de preferencia y aprovechar la rutina `make examples` para compilar por nosotros.

Teniendo en cuenta que puede ser de interes compilar los ejemplos pero utilizando algun parametro propio al compilador, existe la variable de entorno `P_FLAGS`. Basta con agregar los parametros deseados a la misma y seran utilizados en la compilacion de cada uno de los ejemplos. Cabe aclarar que el unico argumento que no deberia usarse es `-o` ya que la rutina lo utiliza para poder generar todos los archivos de salida sin producir colisiones.

## Testing

Teniendo en cuenta el caracter experimental del lenguaje, no tenemos ningun framework especifico para realizar testing. Es por esto que se implemento un pequeño script. Este se encarga de correr los archivos encontrados en la carpeta `errors` y verificar que todos hayan finalizado con codigo de error. Si bien no es la mejor forma de testear, esta herramienta nos permite tener una rapida verificacion acerca de si hay algo que deberia estar siendo detectado como error y no esta ocurriendo.

En caso de querer correrlo se ofrece la rutina `make errors` la cual se encarga de correr el mismo. Ademas, al igual que para los ejemplos, se ofrece la variable `P_FLAGS` para ser utilizados por el compilador.
