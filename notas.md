# Notas

- `ID_TYPE ID '=' VALUE ';'` para asignar el valor de funciones
- ID en los getter y setter tiene que ser solo tipo tag

Operaciones agregadas
- ASSIGNMENT
  - ` SET_PROPERTY FROM ID '=' VALUE`
  - `SET_NAMED_PROPERTY STRING FROM ID '=' VALUE`
  - `APPEND_CHILD FROM ID '=' ID`
- VALUE
  - `GET_PROPERTY FROM ID`
  - `GET_NAMED_PROPERTY STRING_LITERAL FROM ID`
tag p;
tag t = getHeader();
set body from t = "hola";
set attribute "hola from t = "hola;

tag p = {name: "h1", body: "hola"};
string h = "Hola";