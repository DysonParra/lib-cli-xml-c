/*
 * @fileoverview {FileName} se encarga de realizar tareas especificas.
 *
 * @version             1.0
 *
 * @author              Dyson Arley Parra Tilano <dysontilano@gmail.com>
 * Copyright (C) Dyson Parra
 *
 * @History v1.0 --- La implementacion de {FileName} fue realizada el 31/07/2022.
 * @Dev - La primera version de {FileName} fue escrita por Dyson A. Parra T.
 */
#include "xml.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
LANGUAGE DLLIMPORT CALLING void deleteXmlItemChilds(XmlItem** toErase);
CALLING XmlItem* findRoot(XmlItem* firstXmlItem);
CALLING int findMaxLevel(XmlItem* firstXmlItem, int level);
CALLING void printArray(char* array);
CALLING void sprintArray(char* output, char* input);
CALLING void printDebugAllXmlItems(FILE* file, XmlItem* firstXmlItem, int childs, int brothers, int tab1, int tab2);
CALLING void printAllXmlItems(FILE* file, XmlItem* firstXmlItem, int childs, int brothers, int tab);
CALLING char isLetter(char c);
CALLING char isNumber(char c);
CALLING void nextWord(FILE* file, char* word);
CALLING int obtainXmlItems(FILE* file, XmlItem* header, XmlItem* body, int status, char* word);

#if (!defined STATIC_LIB_) && (!defined NO_DLL_MAIN_)
/**
 * Entrada principal de la dll (creado si no se indica que es biblioteca estática)
 *
 * @param instance identificador del módulo.
 * @param reason   razón de la llamada.
 * @param reserved argumento reservado.
 * @return {@code TRUE} si se completó exitosamente la llamada, caso contrario {@code FALSE}.
 */
BOOL APIENTRY DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved) {
    /*
     * Evalúa la razón de la llamada a la dll.
     */
    switch (reason) {
        /*
         * Se cargó la dll en el proceso actual.
         */
        case DLL_PROCESS_ATTACH:
            // printf("%s\n", "DLL_PROCESS_ATTACH");
            break;

        /*
         * Se liberó la dll en el proceso actual.
         */
        case DLL_PROCESS_DETACH:
            // printf("%s\n", "DLL_PROCESS_DETACH");
            break;

        /*
         * Se cargó la dll en un nuevo hilo.
         */
        case DLL_THREAD_ATTACH:
            // printf("%s\n", "DLL_THREAD_ATTACH");
            break;

        /*
         * Se liberó la dll en el nuevo hilo.
         */
        case DLL_THREAD_DETACH:
            // printf("%s\n", "DLL_THREAD_DETACH");
            break;
    }

    return TRUE;
}
#endif

/**
 * FIXME: Definición de {@code newXmlItem}.
 * Crea un nuevo XmlItem y asociarlo a un XmlItem parámetro brother que dirá si es un hermano (diferente a 0), o un hijo (0).
 */
LANGUAGE DLLIMPORT CALLING XmlItem* newXmlItem(char* name, char* value, int type, XmlItem* ref, int brother) {
    XmlItem* nuevo;                                             // Declara un nuevo XmlItem.
    nuevo = (XmlItem*)malloc(sizeof(XmlItem));                  // Inicializa el XmlItem en el heap.

    nuevo->level = 1;                                           // Inicializa el campo level en 1 por defecto.

    nuevo->numberChilds = 0;                                    // Inicializa el campo numberChilds de nuevo en 0.
    nuevo->numberAttributes = 0;                                // Inicializa el campo numberAttributes de nuevo en 0.

    nuevo->type = type;                                         // A nuevo le pone su campo type en el type indicado.
    nuevo->father = NULL;                                       // A nuevo le pone su padre en NULL.
    nuevo->prevBrother = NULL;                                  // A nuevo le pone su anterior hermano en NULL.
    nuevo->nextBrother = NULL;                                  // A nuevo le pone su siguiente hermano en NULL.
    nuevo->firstChild = NULL;                                   // A nuevo le pone su primer hijo en NULL.
    nuevo->firstXmlAttribute = NULL;                            // A nuevo le pone su primer atributo en NULL.

    int nameSize = 0;                                           // Crea variable que dirá la cantidad de caracteres del nombre del XmlItem a crear.
    int valueSize = 0;                                          // Crea variable que dirá la cantidad de caracteres del valor del XmlItem a crear.

    if (ref == NULL && name == NULL)                            // Si no se indicó referencia ni nombre (Raíz).
        name = (char*)"root";                                   // Dice que el nombre será root.

    if (name != NULL) {                                         // Si se indicó nombre.
        nameSize = strlen(name) + 1;                            // A nameSize le lleva la cantidad de letras de name.
        nuevo->name = (char*)malloc(nameSize * sizeof(char));   // Reserva espacio en el heap para el nombre.
        memcpy(nuevo->name, name, nameSize);                    // Asigna nombre al XmlItem actual.
    } else
        nuevo->name = (char*)calloc(1, sizeof(char));           // Pone como nombre el caracter nulo.

    if (value != NULL) {                                        // Si se indicó valor.
        valueSize = strlen(value) + 1;                          // A valueSize le lleva la cantidad de letras de value.
        nuevo->value = (char*)malloc(valueSize * sizeof(char)); // Reserva espacio en el heap para el valor.
        memcpy(nuevo->value, value, valueSize);                 // Asigna valor al XmlItem actual.
    } else
        nuevo->value = (char*)calloc(1, sizeof(char));          // Pone como valor el caracter nulo.

    if (ref != NULL) {                                          // Si se indicó un XmlItem de referencia.
        if (brother) {                                          // Si se indicó que se creará un hermano al XmlItem de referencia.
            nuevo->level = ref->level;                          // A nuevo le pone el mismo nivel que su hermano.
            XmlItem* last = ref;                                // Crea XmlItem apuntando a ref.

            while (last->nextBrother != NULL)                   // Mientras no encuentre al último hemano de ref.
                last = last->nextBrother;                       // A last le lleva su siguiente hermano.

            nuevo->prevBrother = last;                          // A nuevo le pone su anterior hermano como last.
            last->nextBrother = nuevo;                          // A last le pone su siguiente hermano como nuevo.

            if (ref->father != NULL) {                          // Si ref tiene padre.
                nuevo->father = ref->father;                    // A nuevo le pone el mismo padre que tiene ref.
                nuevo->father->numberChilds++;                  // Aumenta la cantidad de hijos del padre de nuevo.
            }
        } else {                                                // Si se indicó que se creará un hijo al XmlItem de referencia.
            nuevo->level = ref->level + 1;                      // A nuevo le pone un nivel más que su padre.
            nuevo->father = ref;                                // Pone ref como el padre de nuevo.
            nuevo->father->numberChilds++;                      // Aumenta la cantidad de hijos del padre de nuevo.

            if (ref->firstChild == NULL)                        // Si ref no tiene hijos.
                ref->firstChild = nuevo;                        // Pone que nuevo es el primer hijo de ref.
            else {                                              // Si ref ya tenía hijos.
                XmlItem* child = ref->firstChild;               // Crea XmlItem apuntando al primer hijo de ref.
                while (child->nextBrother != NULL)              // Mientras no encuentre al último hermano de child (último hijo de ref).
                    child = child->nextBrother;                 // A child le lleva su siguiente hermano.

                nuevo->prevBrother = child;                     // Pone que el anterior hermano de nuevo es child.
                child->nextBrother = nuevo;                     // Pone que el siguiente hermano de child es nuevo.
            }
        }
    }

    return nuevo;                                               // Devuelve el XmlItem creado.
}

/**
 * FIXME: Definición de {@code deleteXmlItemChilds}.
 * Borra y libera la memoria de un XmlItem y todos sus descendientes.
 */
LANGUAGE DLLIMPORT CALLING void deleteXmlItemChilds(XmlItem** toErase) {
    XmlItem* root = NULL;                                       // Crea un nuevo XmlItem.
    if (toErase != NULL)                                        // Si toErase no es NULL.
        root = *toErase;                                        // A root le lleva lo apuntado por el apuntador a un apuntador a XmlItem toErase.

    if (root != NULL) {                                         // Si root es un XmlItem válido.
        while (root->numberChilds)                              // Mientras root tenga hijos.
            deleteXmlItemChilds(&(root->firstChild));           // Ejecuta el borrado de forma recursiva.

        if (root->prevBrother != NULL)                          // Si root no es el primer hermano.
            root->prevBrother->nextBrother = root->nextBrother; // Liga el anterior hermano de root con el siguiente hermano de root.

        if (root->nextBrother != NULL)                          // Si root no es el último hermano.
            root->nextBrother->prevBrother = root->prevBrother; // Liga el siguiente hermano de root con el anterior hermano de root.

        if (root->father != NULL) {                             // Si root tiene padre (root no es la raíz).
            root->father->numberChilds--;                       // Disminuye la cantidad de hijos del padre de root.

            if (root->father->firstChild == root)               // Si root es el primer hijo.
                root->father->firstChild = root->nextBrother;   // Dice que ahora el primer hijo es el siguiente hermano de root.
        }

        while (root->firstXmlAttribute != NULL)                 // Mientras no borre todos los XmlAttributes de root.
            deleteXmlAttribute(&root->firstXmlAttribute);       // Borra el primer atributo de root.

        root->level = 0;                                        // Pone el nivel de root en cero para indicar que ya no es un XmlItem válido.
        free(root);                                             // Libera la memoria ocupada por root.
    }
}

/**
 * FIXME: Definición de {@code deleteXmlItem}.
 * Borra y libera la memoria de un JsonItem y todos sus descendientes, y deja el json item apuntando a NULL.
 */
LANGUAGE DLLIMPORT CALLING void deleteXmlItem(XmlItem** toErase) {
    deleteXmlItemChilds(toErase);   // Borra los hijos de toErase.
    *toErase = NULL;                // Indica que toErase ahora apunta a NULL.
}

/**
 * FIXME: Definición de {@code newXmlAttribute}.
 * Crea un nuevo XmlAttribute y asociarlo a un XmlItem parámetro.
 */
LANGUAGE DLLIMPORT CALLING XmlAttribute* newXmlAttribute(char* name, char* value, XmlItem* item) {
    XmlAttribute* nuevo = NULL;                                 // Declara un nuevo XmlAttribute.
    nuevo = (XmlAttribute*)malloc(sizeof(XmlAttribute));        // Inicializa el XmlAttribute en el heap.

    nuevo->father = NULL;                                       // A nuevo le pone su padre en NULL.
    nuevo->prevBrother = NULL;                                  // A nuevo le pone su anterior hermano en NULL.
    nuevo->nextBrother = NULL;                                  // A nuevo le pone su siguiente hermano en NULL.

    if (name != NULL) {                                         // Si se indicó nombre.
        int nameSize = strlen(name) + 1;                        // Crea variable con la cantidad de letras de name.
        nuevo->name = (char*)malloc(nameSize * sizeof(char));   // Reserva espacio en el heap para el nombre.
        memcpy(nuevo->name, name, nameSize);                    // Asigna nombre al XmlItem actual.
    } else
        nuevo->name = (char*)calloc(1, sizeof(char));           // Pone como nombre el caracter nulo.

    if (value != NULL) {                                        // Si se indicó valor.
        int valueSize = strlen(value) + 1;                      // Crea variable con la cantidad de letras de value.
        nuevo->value = (char*)malloc(valueSize * sizeof(char)); // Reserva espacio en el heap para el valor.
        memcpy(nuevo->value, value, valueSize);                 // Asigna valor al XmlItem actual.
    } else
        nuevo->value = (char*)calloc(1, sizeof(char));          // Pone como valor el caracter nulo.

    if (item != NULL) {                                         // Si se indicó un XmlItem al cual llevarle el atributo a crear.
        nuevo->father = item;                                   // Pone item como el padre de nuevo.
        nuevo->father->numberAttributes++;                      // Aumenta la cantidad de atributos del XmlItem asociado a nuevo.

        if (item->firstXmlAttribute == NULL)                    // Si item no tiene atributos.
            item->firstXmlAttribute = nuevo;                    // Pone que nuevo es el primer atributo de item.
        else {                                                  // Si item ya tenía atributos.
            XmlAttribute* child = item->firstXmlAttribute;      // Crea XmlAttribute apuntando al primer atributo de item.
            while (child->nextBrother != NULL)                  // Mientras no encuentre al último hermano de child (último atributo de item).
                child = child->nextBrother;                     // A child le lleva su siguiente hermano.

            nuevo->prevBrother = child;                         // Pone que el anterior hermano de nuevo es child.
            child->nextBrother = nuevo;                         // Pone que el siguiente hermano de child es nuevo.
        }
    }

    return nuevo;                                               // Devuelve el XmlAttribute creado.
}

/**
 * FIXME: Definición de {@code deleteXmlAttribute}.
 * Borra y libera la memoria de un XmlItem y todos sus descendientes.
 */
LANGUAGE DLLIMPORT CALLING void deleteXmlAttribute(XmlAttribute** toErase) {
    XmlAttribute* attrib = NULL;                                            // Crea un nuevo XmlAttribute.

    if (toErase != NULL)                                                    // Si toErase no es NULL.
        attrib = *toErase;                                                  // A attrib le lleva lo apuntado por el apuntador a un apuntador a XmlAttribute toErase.

    if (attrib != NULL) {                                                   // Si attrib es un XmlAttribute válido.
        if (attrib->prevBrother != NULL)                                    // Si attrib no es el primer hermano.
            attrib->prevBrother->nextBrother = attrib->nextBrother;         // Liga el anterior hermano de attrib con el siguiente hermano de attrib.

        if (attrib->nextBrother != NULL)                                    // Si attrib no es el último hermano.
            attrib->nextBrother->prevBrother = attrib->prevBrother;         // Liga el siguiente hermano de attrib con el anterior hermano de attrib.

        if (attrib->father != NULL) {                                       // Si attrib tiene padre (attrib esta asociado a algún XmlItem).
            attrib->father->numberAttributes--;                             // Disminuye la cantidad de atributos del XmlItem asociado a attrib.

            if (attrib->father->firstXmlAttribute == attrib)                // Si attrib es el primer atributo.
                attrib->father->firstXmlAttribute = attrib->nextBrother;    // Dice que ahora el primer atributo es el siguiente hermano de attrib.
        }

        free(attrib);                                                       // Libera la memoria ocupada por attrib.
        *toErase = NULL;                                                    // Indica que toErase ahora apunta a NULL.
    }
}

/**
 * FIXME: Definición de {@code setXmlItemName}.
 * Cambia el campo name de un XmlItem por el de un array especificado.
 */
LANGUAGE DLLIMPORT CALLING void setXmlItemName(XmlItem* aux, char* name) {
    int nameSize = (int)(strlen(name) + 1);                             // Crea entero que tendrá la longitud del array name.

    if (nameSize > (int)(strlen(aux->name) + 1))                        // Si la longitud del nuevo name es mayor que el anterior.
        aux->name = (char*)realloc(aux->name, nameSize * sizeof(char)); // Redimensiona el espacio en el heap del nombre de aux.

    memcpy(aux->name, name, nameSize);                                  // Copia el valor de name en el campo name de aux.
}

/**
 * FIXME: Definición de {@code setXmlItemValue}.
 * Cambia el campo value de un XmlItem por el de un array especificado.
 */
LANGUAGE DLLIMPORT CALLING void setXmlItemValue(XmlItem* aux, char* value) {
    int valueSize = (int)(strlen(value) + 1);                               // Crea entero que tendrá la longitud del array value.
    if (valueSize > (int)(strlen(aux->value) + 1))                          // Si la longitud del nuevo value es mayor que el anterior.
        aux->value = (char*)realloc(aux->value, valueSize * sizeof(char));  // Redimensiona el espacio en el heap del valor de aux.

    memcpy(aux->value, value, valueSize);                                   // Copia el valor de value en el campo value de aux.
}

/**
 * FIXME: Definición de {@code findXmlItem}.
 * Encuentra un XmlItem en un XmlItem father.
 */
LANGUAGE DLLIMPORT CALLING XmlItem* findXmlItem(XmlItem* father, int nargs, ...) {
    va_list ap;                                     // Crea una lista de parámetros.
    va_start(ap, nargs);                            // Inicializa la lista de parámetros.
    char* params[nargs];                            // Crea array para almacenar los parámetros.

    for (int i = 0; i < nargs; i++)                 // Recorre la lista de parámetros.
        params[i] = va_arg(ap, char*);              // Almacena el parámetro en params.

    va_end(ap);                                     // Finaliza la lista de parámetros.

    // for (int i =0; i< nargs; i++)                // Recorre el array con los parámetros.
    //   printf("Arg %d = \"%s\"\n", i, params[i]); // Muestra el parámetro actual.

    XmlItem* aux = NULL;                            // Crea XmlItem para recorrer los hijos del XmlItem parámetro.

    if (father != NULL)                             // Si el XmlItem parámetro no es NULL.
        aux = father->firstChild;                   // A aux le lleva el hijo del primer XmlItem parámetro.

    int findXmlItem = 1;                            // Crea variable que indicará si se encontró el XmlItem con el nombre especificado.

    for (int i = 0; i < nargs; i++) {               // Recorre los parámetros.
        while (aux != NULL) {                       // Recorre aux para compararlo con el parámetro actual.
            if (strcmp(aux->name, params[i]))       // Si el nombre del XmlItem actual no es igual al del parámetro actual.
                findXmlItem = 0;                    // Pone findXmlItem en 0 indicando que no encontró el XmlItem.

            if (findXmlItem) {                      // Si el XmlItem fue encontrado.
                if (i != nargs - 1)                 // Si este no era el último parámetro.
                    aux = aux->firstChild;          // A aux le lleva su primer hijo.
                break;                              // Sale del while.
            } else                                  // Si no encontró el XmlItem.
                findXmlItem = 1;                    // Reinicia findXmlItem para que buesque en el próximo hermano.

            aux = aux->nextBrother;                 // A aux le lleva su siguiente hermano.
        }

        if (aux == NULL)                            // Si recorrió todos los hermanos de aux y no encontró el XmlItem con nombre igual al parámetro.
            break;                                  // Sale del for.
    }

    return aux;                                     // Devuelve la posición de memoria donde encontró el XmlItem solicitado.
}

/**
 * FIXME: Definición de {@code findRoot}.
 * Obtiene la raíz de un XmlItem.
 */
CALLING XmlItem* findRoot(XmlItem* firstXmlItem) {
    XmlItem* aux;                   // Crea apuntador a un XmlItem para recorrer los hermanos del XmlItem.
    aux = firstXmlItem;             // A aux le lleva una copia del XmlItem parámetro.

    if (aux != NULL)                // Si el XmlItem parámetro no está vacío.
        while (aux->father != NULL) // Recorre los padres de aux.
            aux = aux->father;      // Pasa al padre de aux.

    return aux;                     // Devuelve la raíz de firstXmlItem.
}

/**
 * FIXME: Definición de {@code findMaxLevel}.
 * Obtiene la mayor profundidad de un XmlItem.
 */
CALLING int findMaxLevel(XmlItem* firstXmlItem, int level) {
    XmlItem* aux;                                               // Crea apuntador a un XmlItem para recorrer los hermanos del XmlItem.
    aux = firstXmlItem;                                         // A aux le lleva una copia del XmlItem parámetro.

    if (aux != NULL)                                            // Si el XmlItem parámetro no está vacío.
        while (aux != NULL) {                                   // Recorre los hermanos de aux.
            if (aux->level >= level) {                          // Si el nivel de aux es mayor que level.
                level = aux->level;                             // A level le lleva el nivel de aux.
                if (aux->firstXmlAttribute != NULL)             // Si aux tiene atrributos.
                    level++;                                    // Aumenta en uno el nivel.
            }
            if (aux->numberChilds)                              // Si aux tiene hijos.
                level = findMaxLevel(aux->firstChild, level);   // Busca el nivel de cada uno de los hijos de aux.

            aux = aux->nextBrother;                             // Pasa al siguiente hermano de aux.
        }

    return level;                                               // Devuelve el mayor nivel encontrado.
}

/**
 * FIXME: Definición de {@code printArray}.
 * Imprime un array sin los caracteres de escape.
 */
CALLING void printArray(char* array) {
// Crea macro para imprimir caracteres de escape.
#define printChar(character)      \
    case character:               \
        printf("%s", #character); \
        break

    for (int i = 0; i < strlen(array); i++) {   // Recorre el array de entrada.
        switch (array[i]) {                     // Evalua el caracter actual.
            printChar('\n');                    // Imprime el caracter de escape.
            printChar('\r');                    // Imprime el caracter de escape.
            printChar('\t');                    // Imprime el caracter de escape.
            printChar('\a');                    // Imprime el caracter de escape.
            printChar('\b');                    // Imprime el caracter de escape.
            printChar('\?');                    // Imprime el caracter de escape.
            printChar('\v');                    // Imprime el caracter de escape.
            // printChar('\"');                 // Imprime el caracter de escape.

            default:                            // Si no es un caracter de escape.
                printf("%c", array[i]);         // Imprime el caracter actual.
                break;                          // Termina de evaluar el caracter actual.
        }
    }

#undef printChar                                // Borra macro para imprimir caracteres de escape.
}

/**
 * FIXME: Definición de {@code sprintArray}.
 * Copia un array de entrada en otro sin los caracteres de escape.
 */
CALLING void sprintArray(char* output, char* input) {
// Crea macro para imprimir caracteres de escape.
#define printChar(character)                                \
    case character:                                         \
        sprintf(&output[strlen(output)], "%s", #character); \
        break

    for (int i = 0; i < strlen(input); i++) {                       // Recorre el array de entrada.
        switch (input[i]) {                                         // Evalua el caracter actual.
            printChar('\n');                                        // Imprime el caracter de escape.
            printChar('\r');                                        // Imprime el caracter de escape.
            printChar('\t');                                        // Imprime el caracter de escape.
            printChar('\a');                                        // Imprime el caracter de escape.
            printChar('\b');                                        // Imprime el caracter de escape.
            printChar('\?');                                        // Imprime el caracter de escape.
            printChar('\v');                                        // Imprime el caracter de escape.
            // printChar('\"');                                     // Imprime el caracter de escape.

            default:                                                // Si no es un caracter de escape.
                sprintf(&output[strlen(output)], "%c", input[i]);   // Imprime el caracter actual.
                break;                                              // Termina de evaluar el caracter actual.
        }
    }

#undef printChar                                                    // Borra macro para imprimir caracteres de escape.
}

/**
 * FIXME: Definición de {@code printDebugAllXmlItems}.
 * Muestra un XmlItem, childs dice si imprime todos sus hijos y brother dice si imprime todos sus hermanos, tab1 indica la cantidad de tabulaciones antes de imprimir cada XmlItem.
 */
CALLING void printDebugAllXmlItems(FILE* file, XmlItem* firstXmlItem, int childs, int brothers, int tab1, int tab2) {
    char printer[1000] = "";                                                                    // Crea variable para almacenar lo que se va a imprimir ya sea en un archivo o en pantalla.

#define print(...) sprintf(&printer[strlen(printer)], __VA_ARGS__)                              // crea macro almacenar la salida de sprintf en printer.
//#define prtArray(input) sprintf(&printer[strlen(printer)], "%s", input)                       // crea macro almacenar la salida de sprintArray en printer.
#define prtArray(input) sprintArray(&printer[strlen(printer)], input)                           // crea macro almacenar la salida de sprintArray en printer.

    XmlItem* aux = firstXmlItem;                                                                // Crea apuntador a un XmlItem para recorrer los hermanos del XmlItem parámetro.
    XmlAttribute* attrib = NULL;                                                                // Crea apuntador a un XmlAttribute para recorrer los atributos de los hermanos del XmlItem parámetro.

    int printPointers = 1;                                                                      // Variable que dirá si se imprimen punteros.

    if (aux == NULL || aux->level < 1) {                                                        // Si el XmlItem parámetro está vacío o es un XmlItem inválido (borrado).
        if (file == NULL)                                                                       // Si el apuntador al archivo no es válido.
            printf("No XmlItem\n");                                                             // Imprime que no hay XmlItem.
        else                                                                                    // Si el apuntador a archivo indicado es válido.
            fprintf(file, "No XmlItem\n");                                                      // Imprimer en el archivo que no hay XmlItem.
    } else {                                                                                    // Si el XmlItem parámetro no está vacío.
        while (aux != NULL) {                                                                   // Recorre los hermanos de aux.
            printer[0] = 0;                                                                     // Pone vacío el array con los valores a imprimir.
            print("%2d  ", aux->level);                                                         // Imprime el nivel de aux.

            if (printPointers) {                                                                // Si se indicó imprimir punteros.
                for (int i = 0; i < (tab1)*9; i++)                                              // Recorre 9 veces parámetro tab1.
                    print(" ");                                                                 // Imprime un espacio.

                print("%p %p %p %p  ", aux->father, aux, aux->prevBrother, aux->nextBrother);   // Imprime direcciones de memoria del XmlItem anterior, el actual, el siguiente y el padre.

                for (int i = 0; i < (tab2)*9; i++)                                              // Recorre 9 veces parámetro tab2.
                    print(" ");                                                                 // Imprime un espacio.

                // print("(%p,%p)", aux->name, aux->value);                                     // Imprime dirección de memoria del campo name y del campo value.
                print("%2d  ", aux->level);                                                     // Imprime el nivel de aux.
            }

            print("\"%2d childs\"  ", aux->numberChilds);                                       // Imprime cantidad de hijos del XmlItem actual.
            print("\"%2d attrib\"  ", aux->numberAttributes);                                   // Imprime cantidad de atributos del XmlItem actual.

            switch (aux->type) {                                                                // Evalúa el tipo de XmlItem que es.
                case 1:                                                                         // Si el item es del tipo 1.
                    print("\"Xml\"  ");                                                         // Imprime el tipo de XmlItem actual.
                    break;                                                                      // Termina de evaluar el tipo de item actual.

                case 2:                                                                         // Si el item es del tipo 2.
                    print("\"Doc\"  ");                                                         // Imprime el tipo de XmlItem actual.
                    break;                                                                      // Termina de evaluar el tipo de item actual.

                case 3:                                                                         // Si el item es del tipo 3.
                    print("\"Tag\"  ");                                                         // Imprime el tipo de XmlItem actual.
                    break;                                                                      // Termina de evaluar el tipo de item actual.
            }

            if (aux->name != NULL)                                                              // Si el XmlItem tiene nombre asignado.
                print("'%s'  ", aux->name);                                                     // Imprime nombre del XmlItem actual.
            else                                                                                // Si el XmlItem no tiene un nombre asignado.
                print("''  ");                                                                  // Imprime dos comillas vacías indicando que el XmlItem no tiene nombre.

            if (aux->value != NULL) {                                                           // Si el XmlItem tiene valor asignado.
                print("'");                                                                     // Imprime comillas.
                prtArray(aux->value);                                                           // Imprime valor del XmlItem actual.
                print("'");                                                                     // Imprime comillas.
            } else                                                                              // Si el XmlItem no tiene un valor asignado.
                print("''");                                                                    // Imprime dos comillas vacías indicando que el XmlItem no tiene valor.
            print("  ");                                                                        // Imprime espacios.

            print("\n");                                                                        // Imprime salto de línea.

            if (file == NULL)                                                                   // Si no se indicó un archivo de salida.
                printf("%s", printer);                                                          // Muestra el contenido del array con la información del XmlItem actual en pantalla.
            else                                                                                // Si se indicó un archivo de salida.
                fprintf(file, "%s", printer);                                                   // Escribe el contenido del array con la información del XmlItem actual en el archivo.

            attrib = aux->firstXmlAttribute;                                                    // Almacena el primer attribute de aux en attrib.
            while (attrib != NULL) {
                printer[0] = 0;                                                                 // Pone vacío el array con los valores a imprimir.
                print(" -  ");                                                                  // Imprime el nivel del XmlAttribute.

                if (printPointers) {                                                            // Si se indicó imprimir punteros.
                    for (int i = 0; i < (tab1 + 1) * 9; i++)                                    // Recorre 9 veces parámetro tab1.
                        print(" ");                                                             // Imprime un espacio.

                    print("%p %p ", attrib->father, attrib);                                    // Imprime direcciones de memoria del XmlAttribute anterior y el actual.
                    print("%p %p  ", attrib->prevBrother, attrib->nextBrother);                 // Imprime direcciones de memoria del XmlAttribute siguiente y el padre.
                    for (int i = 0; i < (tab2 - 1) * 9; i++)                                    // Recorre 9 veces parámetro tab2.
                        print(" ");                                                             // Imprime un espacio.

                    for (int i = 0; i < 4; i++)                                                 // Recorre el ciclo las veces indicadas.
                        print("-");                                                             // Imprime el caracter indicado.
                }

                for (int i = 0; i < 31; i++)                                                    // Recorre el ciclo las veces indicadas.
                    print("-");                                                                 // Imprime el caracter indicado.

                if (attrib->name != NULL)                                                       // Si el XmlAttribute tiene nombre asignado.
                    print("  '%s'", attrib->name);                                              // Imprime nombre del XmlAttribute actual.
                else                                                                            // Si el XmlAttribute no tiene un nombre asignado.
                    print("  '");                                                               // Imprime dos comillas vacías indicando que el XmlAttribute no tiene nombre.

                if (attrib->value != NULL) {                                                    // Si el XmlAttribute tiene valor asignado.
                    print("  '\"");                                                             // Imprime comillas.
                    prtArray(attrib->value);                                                    // Imprime valor del XmlAttribute actual.
                    print("\"'");                                                               // Imprime comillas.
                } else                                                                          // Si el XmlAttribute no tiene un valor asignado.
                    print("''");                                                                // Imprime dos comillas vacías indicando que el XmlAttribute no tiene valor.
                print("  ");                                                                    // Imprime espacios.

                print("\n");                                                                    // Imprime salto de línea.

                if (file == NULL)                                                               // Si no se indicó un archivo de salida.
                    printf("%s", printer);                                                      // Muestra el contenido del array con la información del XmlAttribute actual en pantalla.
                else                                                                            // Si se indicó un archivo de salida.
                    fprintf(file, "%s", printer);                                               // Escribe el contenido del array con la información del XmlAttribute actual en el archivo.

                attrib = attrib->nextBrother;                                                   // Pasa al siguiente atributo de aux.
            }

            if (aux->numberChilds && childs)                                                    // Si aux tiene hijos y se indicó imprimir hijos.
                printDebugAllXmlItems(file, aux->firstChild, 1, 1, tab1 + 1, tab2 - 1);         // Imprime cada uno de los hijos de aux.

            if (brothers)                                                                       // Si se indicó imprimir hermanos.
                aux = aux->nextBrother;                                                         // Pasa al siguiente hermano de aux.
            else                                                                                // Si se indicó no imprimir hermanos.
                break;                                                                          // Sale del ciclo.
        }
    }

#undef print                                                                                    // Borra la macro para almacenar la salida de sprintf en printer.
#undef prtArray                                                                                 // Borra la macro para almacenar la salida de sprintArray en printer.
}

/**
 * FIXME: Definición de {@code printDebugXmlItem}.
 * Invoca al método que imprime los XmlItem en modo debug.
 */
LANGUAGE DLLIMPORT CALLING void printDebugXmlItem(char* fileSource, char* openType, XmlItem* root, int childs) {
    FILE* file = fopen(fileSource, openType); // Crea un archivo en la ruta indicada por fileSource.

    if (!childs || root == NULL)                                                                            // Si indicó no imprimir los hijos del XmlItem o el XmlItem está vacío.
        printDebugAllXmlItems(file, root, childs, 0, 0, 0);                                                 // Imprime los XmlItem.
    else                                                                                                    // Si indicó imprimir los hijos del XmlItem.
        printDebugAllXmlItems(file, root, childs, 0, 0, findMaxLevel(root->firstChild, 0) - root->level);   // Imprime los XmlItem.

    if (file == NULL)                                                                                       // Si el apuntador al archivo no es válido.
        printf("\n");                                                                                       // Imprime salto de línea en pantalla.
    else                                                                                                    // Si el apuntador a archivo indicado es válido.
        fprintf(file, "\n");                                                                                // Imprime salto de línea en el archivo.

    if (file != NULL)                                                                                       // Si se pudo abrir el archivo.
        fclose(file);                                                                                       // Cierra el archivo indicado por fileSource.
}

/**
 * FIXME: Definición de {@code printAllXmlItems}.
 * Muestra un XmlItem, childs dice si imprime todos sus hijos y brother dice si imprime todos sus hermanos, tab indica la cantidad de tabulaciones antes de imprimir cada XmlItem.
 */
CALLING void printAllXmlItems(FILE* file, XmlItem* firstXmlItem, int childs, int brothers, int tab) {
    char printer[1000] = "";                                                        // Crea variable para almacenar lo que se va a imprimir ya sea en un archivo o en pantalla.

#define print(...) sprintf(&printer[strlen(printer)], __VA_ARGS__)                  // crea macro almacenar la salida de sprintf en printer.
#define prtArray(input) sprintf(&printer[strlen(printer)], "%s", input)             // crea macro almacenar la salida de sprintArray en printer.
    //#define prtArray(input) sprintArray(&printer[strlen(printer)], input)         // crea macro almacenar la salida de sprintArray en printer.

    XmlItem* aux = firstXmlItem;                                                    // Crea apuntador a un XmlItem para recorrer los hermanos del XmlItem parámetro.
    XmlAttribute* attrib = NULL;                                                    // Crea apuntador a un XmlAttribute para recorrer los atributos de los hermanos del XmlItem parámetro.
    char noName[20] = "unknownName";                                                // Crea array que tendrá el nombre para los XmlItem que no tengan nombre.

    if (aux == NULL || aux->level < 1) {                                            // Si el XmlItem parámetro está vacío o es un XmlItem inválido (borrado).
        if (file == NULL)                                                           // Si el apuntador al archivo no es válido.
            printf("No \n");                                                        // Imprime que no hay XmlItem.
        else                                                                        // Si el apuntador a archivo indicado es válido.
            fprintf(file, "No XmlItem\n");                                          // Imprime en el archivo que no hay XmlItem.
    } else {                                                                        // Si el XmlItem parámetro no está vacío.
        while (aux != NULL) {                                                       // Recorre los hermanos de aux.
            printer[0] = 0;                                                         // Pone vacío el array con los valores a imprimir.

            for (int i = 0; i < (tab * 2); i++)                                     // Recorre el parámetro tab.
                print(" ");                                                         // Imprime un espacio.

            switch (aux->type) {                                                    // Evalúa el tipo de XmlItem actual.
                case 1:                                                             // Si el tipo de XmlItem es xml.
                    print("<?");                                                    // Imprime el inicio de los XmlItem xml.
                    break;                                                          // Termina de evaluar el tipo de XmlItem.

                case 2:                                                             // Si el tipo de XmlItem es doc.
                    print("<!");                                                    // Imprime el inicio de los XmlItem doc.
                    break;                                                          // Termina de evaluar el tipo de XmlItem.

                case 3:                                                             // Si el tipo de XmlItem es tag.
                    print("<");                                                     // Imprime el inicio de los XmlItem tag.
                    break;                                                          // Termina de evaluar el tipo de XmlItem.
            }

            if (aux->name == NULL || aux->name[0] == 0)                             // Si el XmlItem actual no tiene nombre asignado.
                print("%s", noName);                                                // Imprime nombre genérico para los XmlItem sin nombre.
            else                                                                    // Si el XmlItem tiene nombre asignado.
                print("%s", aux->name);                                             // Imprime nombre del XmlItem actual.

            attrib = aux->firstXmlAttribute;                                        // Obtiene el primer atributos del XmlItem actual.

            while (attrib != NULL) {                                                // Recorre los atributos del XmlItem actual.
                if (attrib->name != NULL && attrib->name[0] != 0) {                 // Si el XmlAttribute tiene nombre asignado.
                    print(" %s", attrib->name);                                     // Imprime nombre del XmlAttribute actual.

                    if (attrib->value != NULL && attrib->value[0] != 0) {           // Si el XmlAttribute tiene valor asignado.
                        print("=\"");                                               // Imprime un igual.
                        prtArray(attrib->value);                                    // Imprime valor del XmlAttribute actual.
                        print("\"");                                                // Imprime comillas.
                    }
                }

                attrib = attrib->nextBrother;                                       // Pasa al siguiente atributo del XmlItem.
            }

            if ((aux->value != NULL && aux->value[0] != 0) || aux->numberChilds) {  //  Si el XmlItem actual tiene valor asignado o tiene almenos un hijo.
                print(">");                                                         // Imprime cierre de la etiqueta de apertura.
                prtArray(aux->value);                                               // Imprime valor del XmlItem actual.
            } else                                                                  // Si el XmlItem no tiene un valor asignado y no tiene hijos.
                switch (aux->type) {                                                // Evalúa el tipo de XmlItem actual.
                    case 1:                                                         // Si el tipo de XmlItem es xml.
                        print("?>");                                                // Imprime el final de los XmlItem xml.
                        break;                                                      // Termina de evaluar el tipo de XmlItem.

                    case 2:                                                         // Si el tipo de XmlItem es doc.
                        print(">");                                                 // Imprime el final de los XmlItem doc.
                        break;                                                      // Termina de evaluar el tipo de XmlItem.

                    case 3:                                                         // Si el tipo de XmlItem es tag.
                        print("/>");                                                // Imprime el final de los XmlItem tag.
                        break;                                                      // Termina de evaluar el tipo de XmlItem.
                }

            if (aux->numberChilds && childs)                                        // Si aux tiene hijos y se indicó imprimir hijos.
                print("\n");                                                        // Imprime salto de línea.

            if (file == NULL)                                                       // Si no se indicó un archivo de salida.
                printf("%s", printer);                                              // Muestra el contenido del array con la información del XmlAttribute actual en pantalla.
            else                                                                    // Si se indicó un archivo de salida.
                fprintf(file, "%s", printer);                                       // Escribe el contenido del array con la información del XmlAttribute actual en el archivo.

            if (aux->numberChilds && childs)                                        // Si aux tiene hijos y se indicó imprimir hijos.
                printAllXmlItems(file, aux->firstChild, 1, 1, tab + 1);             // Imprime cada uno de los hijos de aux.

            printer[0] = 0; // Pone vacío el array con los valores a imprimir.

            if ((aux->value != NULL && aux->value[0] != 0) || aux->numberChilds) {  // Si el XmlItem actual tiene valor asignado o tiene almenos un hijo.
                if (aux->numberChilds) {                                            // Si aux tiene al menos un hijo.
                    for (int i = 0; i < (tab * 2); i++)                             // Recorre el parámetro tab.
                        print(" ");                                                 // Imprime un espacio.
                }
                if (aux->name == NULL || aux->name[0] == 0)                         // Si el XmlItem actual no tiene nombre asignado.
                    print("</%s>", noName);                                         // Imprime nombre genérico para los XmlItem sin nombre en etiqueta de cierre.
                else                                                                // Si el XmlItem tiene nombre asignado.
                    print("</%s>", aux->name);                                      // Imprime nombre del XmlItem actual en etiqueta de cierre.
            }

            print("\n");                                                            // Imprime salto de línea.

            if (file == NULL)                                                       // Si no se indicó un archivo de salida.
                printf("%s", printer);                                              // Muestra el contenido del array con la información del XmlAttribute actual en pantalla.
            else                                                                    // Si se indicó un archivo de salida.
                fprintf(file, "%s", printer);                                       // Escribe el contenido del array con la información del XmlAttribute actual en el archivo.

            if (brothers)                                                           // Si se indicó imprimir hermanos.
                aux = aux->nextBrother;                                             // Pasa al siguiente hermano de aux.
            else                                                                    // Si se indicó no imprimir hermanos.
                break;                                                              // Sale del ciclo.
        }
    }

#undef print                                                                        // Borra la macro para almacenar la salida de sprintf en printer.
#undef prtArray                                                                     // Borra la macro para almacenar la salida de sprintArray en printer.
}

/**
 * FIXME: Definición de {@code printXmlItem}.
 * Invoca al método que imprime los XmlItem.
 */
LANGUAGE DLLIMPORT CALLING void printXmlItem(char* fileSource, char* openType, XmlItem* root, int childs) {
    FILE* file = fopen(fileSource, openType);   // Crea un archivo en la ruta indicada por fileSource.
    printAllXmlItems(file, root, childs, 0, 0); // Imprime los XmlItem.

    if (file == NULL)                           // Si el apuntador al archivo no es válido.
        printf("\n");                           // Imprime salto de línea en pantalla.
    else                                        // Si el apuntador a archivo indicado es válido.
        fprintf(file, "\n");                    // Imprime salto de línea en el archivo.

    if (file != NULL)                           // Si se pudo abrir el archivo.
        fclose(file);                           // Cierra el archivo indicado por fileSource.
}

/**
 * FIXME: Definición de {@code printXmlFile}.
 * Imprime un XmlItem con un archivo xml cargado.
 */
LANGUAGE DLLIMPORT CALLING void printXmlFile(char* fileSource, char* openType, XmlItem* root) {
    FILE* file = fopen(fileSource, openType);                                                       // Crea un archivo en la ruta indicada por fileSource.

    if (root == NULL) {                                                                             // Si el XmlItem parámetro está vacío.
        if (file == NULL)                                                                           // Si el apuntador al archivo no es válido.
            printf("Invalid XmlFile\n");                                                            // Imprime que no es un XmlItem con un archivo xml en pantalla.
        else                                                                                        // Si el apuntador a archivo indicado es válido.
            fprintf(file, "Invalid XmlFile\n");                                                     // Imprime que no es un XmlItem con un archivo xml en el archivo.
    } else {                                                                                        // Si el XmlItem parámetro no está vacío.
        XmlItem* header = findXmlItem(root, 1, (char*)"header");                                    // Crea XmlItem para imprimir el encabezado del archivo.
        XmlItem* body = findXmlItem(root, 1, (char*)"body");                                        // Crea XmlItem para imprimir el cuerpo del archivo.

        if (header == NULL || body == NULL || root->numberChilds != 2 || body->numberChilds != 1) { // Si el archivo xml no tiene encabezado, no tiene cuerpo, no tiene únicamente un hijo o el XmlItem raíz no tiene únicamente dos hijos.
            if (file == NULL)                                                                       // Si el apuntador al archivo no es válido.
                printf("Invalid XmlFile\n");                                                        // Imprime que no es un XmlItem con un archivo xml en pantalla.
            else                                                                                    // Si el apuntador a archivo indicado es válido.
                fprintf(file, "Invalid XmlFile\n");                                                 // Imprime que no es un XmlItem con un archivo xml en el archivo.
        } else {                                                                                    // Si el xml si tiene cuerpo.
            int invalid = 0;                                                                        // Crea variable que di´ra si luego de recorrer el encabezado el XmlItem sigue siendo válido.
            header = header->firstChild;                                                            // A header le lleva su primer hijo.

            while (header != NULL) {                                                                // Recorre todos los XmlItem del encabezado.
                if (header->numberChilds || (header->value != NULL && header->value[0] != 0)) {     // Si el XmlItem actuel tiene hijos o algún valor asignado.
                    invalid = 1;                                                                    // Indica que el XmlItem no es válido.
                    break;                                                                          // Sale del while.
                }
                header = header->nextBrother;                                                       // Pasa al siguiente XmlItem del encabezado.
            }

            if (invalid) {                                                                          // Si luego de recorrer el encabezado se encontró que no es un archivo xml válido.
                if (file == NULL)                                                                   // Si el apuntador al archivo no es válido.
                    printf("Invalid XmlFile\n");                                                    // Imprime que no es un XmlItem con un archivo xml en pantalla.
                else                                                                                // Si el apuntador a archivo indicado es válido.
                    fprintf(file, "Invalid XmlFile\n");                                             // Imprime que no es un XmlItem con un archivo xml en el archivo.
            } else {                                                                                // Si el XmlItem sigue siendo válido luego de recorrer el encabezado.
                header = findXmlItem(root, 1, (char*)"header")->firstChild;                         // Obtiene el primer XmlItem del encabezado.

                if (header != NULL)                                                                 // Si el encabezado tiene almenos un XmlItem.
                    printAllXmlItems(file, header, 1, 1, 0);                                        // Imprime todos los XmlItem del encabezado.

                printAllXmlItems(file, body->firstChild, 1, 0, 0);                                  // Imprime el cuerpo del archivo xml y todos sus hijos.
            }
        }
    }

    if (file == NULL)                                                                               // Si el apuntador al archivo no es válido.
        printf("\n");                                                                               // Imprime salto de línea en pantalla.
    else                                                                                            // Si el apuntador a archivo indicado es válido.
        fprintf(file, "\n");                                                                        // Imprime salto de línea en el archivo.

    if (file != NULL)                                                                               // Si se pudo abrir el archivo.
        fclose(file);                                                                               // Cierra el archivo indicado por fileSource.
}

/**
 * FIXME: Definición de {@code isLetter}.
 * Evalúa si un caracter es una letra. Devuelve 1 caso afirmativo o caso contrario 0.
 */
CALLING char isLetter(char c) {
    if ((c >= 65 && c <= 90) || (c >= 97 && c <= 122))  // Si el caracter es una letra.
        return 1;                                       // Devuelve 1 indicando que es una letra.
    else                                                // Si el caracter no es una letra.
        return 0;                                       // Devuelve 0 indicando que no es una letra.
}

/**
 * FIXME: Definición de {@code isNumber}.
 * Evalúa si un caracter es un número o un signo. Devuelve 1 caso afirmativo o caso contrario 0.
 */
CALLING char isNumber(char c) {
    if ((c >= 48 && c <= 57) || c == '+' || c == '-' || c == '.' || c == 'e' || c == 'E')   // Si el caracter es un número.
        return 1;                                                                           // Devuelve 1 indicando que es un número.
    else                                                                                    // Si el caracter no es un número.
        return 0;                                                                           // Devuelve 0 indicando que no es un número.
}

/**
 * FIXME: Definición de {@code nextWord}.
 * Obtiene la siguiente palabra del archivo xml.
 */
CALLING void nextWord(FILE* file, char* word) {
    // printf("%I32d\n", ftell(file));                                              // Muestra la posición del cursor del archivo.
    word[0] = 0;                                                                    // Pone vacía el array donde se almacaenará la palabra.
    int end = -1;                                                                   // Crea variable para almacenar la posición del último caracter de la palabra.

    if (!feof(file)) {                                                              // Si no ha terminado de recorrer el archivo.
        word[0] = fgetc(file);                                                      // Obtiene el primer caracter del archivo.
        // printf("'%c'", word[0]);                                                 // Muestra el primer caracter obtenido.

        switch (word[0]) {                                                          // Evalúa el primer caracter obtenido.
            case '<':                                                               // Si el primer caracter es un menor que.
                word[1] = fgetc(file);                                              // Obtiene el segundo caracter.
                switch (word[1]) {                                                  // Evalúa el segundo caracter.
                    case '?':                                                       // Si el segundo caracter es un signo de interrogación.
                    case '!':                                                       // Si el segundo caracter es un signo de admiración.
                    case '/':                                                       // Si el segundo caracter es un slash.
                        end = 1;                                                    // Indica que el final de la palabra es en la posición uno.
                        break;                                                      // Termina de evaluar el segundo caracter.

                    default:                                                        // Si el segundo caracter no es ninguno de los indicados.
                        end = 0;                                                    // Indica que el final de la palabra es en la posición cero.
                        fseek(file, -1, SEEK_CUR);                                  // Retrocede el cursor del archivo una posición.
                        break;                                                      // Termina de evaluar el segundo caracter.
                }
                break;                                                              // Termina de evaluar el primer caracter.

            case '?':                                                               // Si primer caracter es un signo de interrogación.
            case '/':                                                               // Si primer caracter es un slash.
                word[1] = fgetc(file);                                              // Obtiene el segundo caracter.
                switch (word[1]) {                                                  // Evalúa el segundo caracter.
                    case '>':                                                       // Si el segundo caracter es un mayor que.
                        end = 1;                                                    // Indica que el final de la palabra es en la posición uno.
                        break;                                                      // Termina de evaluar el segundo caracter.

                    default:                                                        // Si el segundo caracter no es n mayor que.
                        end = 0;                                                    // Indica que el final de la palabra es en la posición cero.
                        fseek(file, -1, SEEK_CUR);                                  // Retrocede el cursor del archivo una posición.
                        break;                                                      // Termina de evaluar el segundo caracter.
                }
                break;                                                              // Termina de evaluar el primer caracter.

            case '\t':                                                              // Si el primer caracter es una tabulación.
            case '\n':                                                              // Si el primer caracter es un salto de línea.
            case ' ':                                                               // Si el primer caracter es un espacio.
                word[0] = ' ';                                                      // Pone el primer caracter como un espacio.
                break;                                                              // Termina de evaluar el primer caracter.

            case '_':                                                               // Si el primer caracter es un guión bajo.
            case '\"':                                                              // Si el primer caracter son comillas.
                break;                                                              // Termina de evaluar el primer caracter.

            default:                                                                // Si el primer caracter no es ninguno de los indicados.
                if (!isLetter(word[0]))                                             // Si el primer caracter no es una letra.
                    end = 0;                                                        // Indica que el final de la palabra es en la posición cero.
                break;                                                              // Termina de evaluar el primer caracter.
        }
    }

    if (end == -1) {                                                                // Si no se indicó el fin de la palabra luego de obtener el primer caracter.
        for (int i = 1; i < 5000; i++) {                                            // Recorre el archivo.
            word[i] = fgetc(file);                                                  // Obtiene el siguiente caracter del archivo.
            // printf("'%c'", word[i]);                                             // Muestra el caracter obtenido.

            switch (word[i]) {                                                      // Evalúa el caracter obtenido.
                case '\t':                                                          // Si el caracter actual es una tabulación.
                case '\n':                                                          // Si el caracter actual es un salto de línea.
                case ' ':                                                           // Si el caracter actual es un espacio.
                    word[i] = ' ';                                                  // Cambia el caracter obtenido por un espacio.
                    switch (word[0]) {                                              // Evalúa el primer caracter.
                        case ' ':                                                   // Si el primer caracter es un espacio.
                        case '\"':                                                  // Si el primer caracter son comillas.
                            break;                                                  // Termina de evaluar el primer caracter.

                        default:                                                    // Si el primer caracter no es ninguno de los indicados.
                            end = i - 1;                                            // Indica que el final de la palabra es en la posición anterior.
                            fseek(file, -1, SEEK_CUR);                              // Retrocede el cursor del archivo una posición.
                            break;                                                  // Termina de evaluar el primer caracter.
                    }
                    break;                                                          // Termina de evaluar el caracter actual.

                case '\"':                                                          // Si el caracter actual son comillas.
                    switch (word[0]) {                                              // Evalúa el primer caracter.
                        case '\"':                                                  // Si el primer caracter son comillas.
                            end = i;                                                // Indica que el final de la palabra es en la posición actual.
                            break;                                                  // Termina de evaluar el primer caracter.

                        default:                                                    // Si el primer caracter no son comillas.
                            end = i - 1;                                            // Indica que el final de la palabra es en la posición anterior.
                            fseek(file, -1, SEEK_CUR);                              // Retrocede el cursor del archivo una posición.
                            break;                                                  // Termina de evaluar el primer caracter.
                    }
                    break;                                                          // Termina de evaluar el caracter actual.

                case EOF:                                                           // Si el caracter actual es fin de archivo.
                    end = i - 1;                                                    // Indica que el final de la palabra es en la posición anterior.
                    break;                                                          // Termina de evaluar el caracter actual.

                default:                                                            // Si el caracter actual no es ninguno de los indicados.
                    switch (word[0]) {                                              // Evalúa el primer caracter.
                        case '\"':                                                  // Si el primer caracter son comillas.
                            break;                                                  // Termina de evaluar el primer caracter.

                        case ' ':                                                   // Si el primer caracter es un espacio.
                            end = i - 1;                                            // Indica que el final de la palabra es en la posición anterior.
                            fseek(file, -1, SEEK_CUR);                              // Retrocede el cursor del archivo una posición.
                            break;                                                  // Termina de evaluar el primer caracter.

                        default:                                                    // Si el primer caracter no es ninguno de los indicados.
                            switch (word[i]) {                                      // Evalúa el caracter actual.
                                case '-':                                           // Si el caracter actual es un guión.
                                case '_':                                           // Si el caracter actual es un guión bajo.
                                case '.':                                           // Si el caracter actual es un punto.
                                case ':':                                           // Si el caracter actual son dos puntos.
                                    break;                                          // Termina de evaluar el caracter actual.

                                default:                                            // Si el caracter actual no es ninguno de los indicados.
                                    if (!isLetter(word[i]) && !isNumber(word[i])) { // Si el caracter actual no es una letra ni un número.
                                        end = i - 1;                                // Indica que el final de la palabra es en la posición anterior.
                                        fseek(file, -1, SEEK_CUR);                  // Retrocede el cursor del archivo una posición.
                                    }
                                    break;                                          // Termina de evaluar el caracter actual.
                            }
                            break;                                                  // Termina de evaluar el primer caracter.
                    }
                    break;                                                          // Termina de evaluar el caracter actual.
            }

            if (end != -1)                                                          // Si con el caracter actual se obtuvo el final de la palabra.
                break;                                                              // Sale del ciclo.
        }
    }
    // printf("\n");
    word[end + 1] = 0;                                                              // Marca el fin de la palabra encontrada.
}

/**
 * FIXME: Definición de {@code obtainXmlItems}.
 * Obtiene los XmlItem de un archivo, header indica un XmlItem donde irá el encabezado del xml, y body un XmlItem donde irá el cuerpo del archivo xml. (devuelve el último estado obtenido).
 */
CALLING int obtainXmlItems(FILE* file, XmlItem* header, XmlItem* body, int status, char* word) {
    XmlItem* item = NULL;           // Variable usada para obtener la posición de los XmlItem que se vayan creando.
    char itemValue[2000] = "";      // Variable usada para almacenar el valor de cada item encontrado.
    char attribName[250] = "";      // Variable usada para almacenar el nombre de cada atributo encontrado.
    char attribValue[250] = "";     // Variable usada para almacenar el valor de cada atributo encontrado.
    int prevStatus = status;        // variable usada para almacenar el estado anterior de status.

// Crea macro para obtener el próximo estado del array.
#define updateStatus(stat, space, opXml, opDoc, opEndTag, opStartTag, clXml, clVoidTag, clTag, name, iqual, quotation, other) \
    case stat:                                                                                                                \
        switch (word[0]) {                                                                                                    \
            case ' ':                                                                                                         \
            case EOF:                                                                                                         \
                status = space;                                                                                               \
                break;                                                                                                        \
                                                                                                                              \
            case '<':                                                                                                         \
                switch (word[1]) {                                                                                            \
                    case '?':                                                                                                 \
                        status = opXml;                                                                                       \
                        break;                                                                                                \
                                                                                                                              \
                    case '!':                                                                                                 \
                        status = opDoc;                                                                                       \
                        break;                                                                                                \
                                                                                                                              \
                    case '/':                                                                                                 \
                        status = opEndTag;                                                                                    \
                        break;                                                                                                \
                                                                                                                              \
                    default:                                                                                                  \
                        status = opStartTag;                                                                                  \
                        break;                                                                                                \
                }                                                                                                             \
                break;                                                                                                        \
                                                                                                                              \
            case '?':                                                                                                         \
                if (word[1] == '>')                                                                                           \
                    status = clXml;                                                                                           \
                else                                                                                                          \
                    status = other;                                                                                           \
                break;                                                                                                        \
                                                                                                                              \
            case '/':                                                                                                         \
                if (word[1] == '>')                                                                                           \
                    status = clVoidTag;                                                                                       \
                else                                                                                                          \
                    status = other;                                                                                           \
                break;                                                                                                        \
                                                                                                                              \
            case '>':                                                                                                         \
                status = clTag;                                                                                               \
                break;                                                                                                        \
                                                                                                                              \
            case '=':                                                                                                         \
                status = iqual;                                                                                               \
                break;                                                                                                        \
                                                                                                                              \
            case '\"':                                                                                                        \
                if (word[strlen(word) - 1] == '\"')                                                                           \
                    status = quotation;                                                                                       \
                else                                                                                                          \
                    status = other;                                                                                           \
                break;                                                                                                        \
                                                                                                                              \
            default:                                                                                                          \
                if (isLetter(word[0]) || word[0] == '_')                                                                      \
                    status = name;                                                                                            \
                else                                                                                                          \
                    status = other;                                                                                           \
                break;                                                                                                        \
        }                                                                                                                     \
        break;

    // printf("\nNew recursive execution\n");

    while (!feof(file) && status) {
        nextWord(file, word);                                         // Obtiene la siguiente palabra.
        prevStatus = status;                                          // Almacena el estado anterior.
        switch (status) {                                             // Evalúa el estado actual.
            updateStatus(1, 2, 3, 9, 0, 16, 0, 0, 0, 0, 0, 0, 0);     // Si el estado actual es "start status".
            updateStatus(2, 2, 3, 9, 0, 16, 0, 0, 0, 0, 0, 0, 0);     // Si el estado actual es "space at start".
            updateStatus(3, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0);      // Si el estado actual es "open xml".
            updateStatus(4, 4, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0);      // Si el estado actual es "xml name".
            updateStatus(5, 5, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0);      // Si el estado actual es "xml attrib".
            updateStatus(6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0);      // Si el estado actual es "waiting xml attrib value".
            updateStatus(7, 7, 0, 0, 0, 0, 8, 0, 0, 5, 0, 0, 0);      // Si el estado actual es "xml attrib value".
            updateStatus(8, 8, 3, 9, 0, 16, 0, 0, 0, 0, 0, 0, 0);     // Si el estado actual es "close xml".
            updateStatus(9, 0, 0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0);     // Si el estado actual es "open doctype".
            updateStatus(10, 10, 0, 0, 0, 0, 0, 0, 0, 11, 0, 0, 0);   // Si el estado actual es "doctype name".
            updateStatus(11, 11, 0, 0, 0, 0, 0, 0, 15, 12, 0, 0, 0);  // Si el estado actual es "doctype type".
            updateStatus(12, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 0);   // Si el estado actual es "doctype visibility".
            updateStatus(13, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 0);   // Si el estado actual es "dtd language".
            updateStatus(14, 14, 0, 0, 0, 0, 0, 0, 15, 0, 0, 0, 0);   // Si el estado actual es "dtd source".
            updateStatus(15, 15, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0);   // Si el estado actual es "close doctype".
            updateStatus(16, 0, 0, 0, 0, 0, 0, 0, 0, 17, 0, 0, 0);    // Si el estado actual es "open start tag".
            updateStatus(17, 17, 0, 0, 0, 0, 0, 22, 21, 18, 0, 0, 0); // Si el estado actual es "start tag name".
            updateStatus(18, 18, 0, 0, 0, 0, 0, 0, 0, 0, 19, 0, 0);   // Si el estado actual es "tag attrib".
            updateStatus(19, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20, 0);   // Si el estado actual es "waiting tag attrib value".
            updateStatus(20, 20, 0, 0, 0, 0, 0, 22, 21, 18, 0, 0, 0); // Si el estado actual es "tag attrib value".
            updateStatus(21, 21, 0, 0, 23, 16, 0, 0, 0, 0, 0, 0, 0);  // Si el estado actual es "close start tag".
            updateStatus(22, 22, 0, 0, 23, 16, 0, 0, 0, 0, 0, 0, 0);  // Si el estado actual es "close void tag".
            updateStatus(23, 0, 0, 0, 0, 0, 0, 0, 0, 24, 0, 0, 0);    // Si el estado actual es "open end tag".
            updateStatus(24, 24, 0, 0, 0, 0, 0, 0, 25, 0, 0, 0, 0);   // Si el estado actual es "end tag name".
            updateStatus(25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);     // Si el estado actual es "close end tag".
            updateStatus(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);      // Si el estado actual es "error".
        }

        /*-
        printf("%d %d ", prevStatus, status);                                                   // Muestra el estado anterior y el actual.
        printf("'%s'\n", word);                                                                 // Muestra la palabra obtenida.
        */

        switch (status) {                                                // Eavlua el estado obtenido con la palabra actual.
            case 4:                                                      // Si el estado actual es "xml name".
                if (word[0] != ' ') {                                    // Si la palabra obtenida no fue un espacio.
                    if (!strcmp((char*)"xml", word)) {                   // Si el nombre de la etiqueta es "xml".
                        if (ftell(file) == 5)                            // Si el cursor está al principio del archivo.
                            item = newXmlItem(word, NULL, 1, header, 0); // Crea un nuevo XmlItem en el encabezado.
                        else                                             // Si el cursor no está al inicio del archivo.
                            status = 0;                                  // Indica que el archivo xml no es válido.
                    }
                    elif (!strcmp((char*)"xml-stylesheet", word))    // Si el nombre de la etiqueta es "xml-stylesheet".
                        item = newXmlItem(word, NULL, 1, header, 0); // Crea un nuevo XmlItem en el encabezado.
                    else                                             // Si el nombre de la etiqueta no es "xml" ni "xml-stylesheet".
                        status = 0;                                  // Indica que el archivo xml no es válido.

                    // printDebugXmlItem(NULL, "a", header->father, 1);                          // Muestra todos los XmlItem obtenidos hasta ahora.
                }
                break; // Termina de evaluar el estado obtenido.

            case 5:                                             // Si el estado actual es "xml attrib".
            case 18:                                            // Si el estado actual es "tag attrib".
                if (word[0] != ' ')                             // Si la palabra obtenida no fue un espacio.
                    memcpy(attribName, word, strlen(word) + 1); // Almacena el nombre del atributo actual.
                break;                                          // Termina de evaluar el estado obtenido.

            case 7:                                              // Si el estado actual es "xml attrib value".
                if (word[0] != ' ') {                            // Si la palabra obtenida no fue un espacio.
                    memcpy(attribValue, &word[1], strlen(word)); // Almacena el valor del atributo actual.
                    attribValue[strlen(word) - 2] = 0;           // Marca el fin del valor del atributo actual.

                    if (!strcmp((char*)"xml", item->name))                          // Si el nombre de la etiqueta actual es "xml".
                        switch (item->numberAttributes) {                           // Evalúa la cantidad de atributos.
                            case 0:                                                 // Si aún no tiene atributos.
                                if (!strcmp((char*)"version", attribName))          // Si el nombre del atributo es "version".
                                    newXmlAttribute(attribName, attribValue, item); // Crea un nuevo atributo al XmlItem actual.
                                else                                                // Si el nombre del atributo no es "version".
                                    status = 0;                                     // Indica que el archivo xml no es válido.
                                break;                                              // Termina de evaluar la cantidad de atributos.

                            case 1:                                                 // Si ya tiene un atributo.
                                if (!strcmp((char*)"encoding", attribName))         // Si el nombre del atributo es "encoding".
                                    newXmlAttribute(attribName, attribValue, item); // Crea un nuevo atributo al XmlItem actual.
                                else                                                // Si el nombre del atributo no es "encoding".
                                    status = 0;                                     // Indica que el archivo xml no es válido.
                                break;                                              // Termina de evaluar la cantidad de atributos.

                            case 2:                                                 // Si ya tiene dos atributos.
                                if (!strcmp((char*)"standalone", attribName))       // Si el nombre del atributo es "standalone".
                                    newXmlAttribute(attribName, attribValue, item); // Crea un nuevo atributo al XmlItem actual.
                                else                                                // Si el nombre del atributo no es "standalone".
                                    status = 0;                                     // Indica que el archivo xml no es válido.
                                break;                                              // Termina de evaluar la cantidad de atributos.
                        }
                    else                                                // Si el nombre de la etiqueta actual es "xml-stylesheet".
                        newXmlAttribute(attribName, attribValue, item); // Crea un nuevo atributo al XmlItem actual.

                    // printDebugXmlItem(NULL, "a", header->father, 1);                          // Muestra todos los XmlItem obtenidos hasta ahora.
                }
                break; // Termina de evaluar el estado obtenido.

            case 10:                                                 // Si el estado actual es "doctype name".
                if (word[0] != ' ') {                                // Si la palabra obtenida no fue un espacio.
                    if (!strcmp((char*)"DOCTYPE", word))             // Si el nombre de la etiqueta es "DOCTYPE".
                        item = newXmlItem(word, NULL, 2, header, 0); // Crea un nuevo XmlItem en el encabezado.
                    else                                             // Si el nombre de la etiqueta no es "DOCTYPE".
                        status = 0;                                  // Indica que el archivo xml no es válido.

                    // printDebugXmlItem(NULL, "a", header->father, 1);                          // Muestra todos los XmlItem obtenidos hasta ahora.
                }
                break; // Termina de evaluar el estado obtenido.

            case 11:                                   // Si el estado actual es "doctype type".
                if (word[0] != ' ') {                  // Si la palabra obtenida no fue un espacio.
                    newXmlAttribute(word, NULL, item); // Crea un nuevo atributo al XmlItem actual.
                    // printDebugXmlItem(NULL, "a", header->father, 1);                          // Muestra todos los XmlItem obtenidos hasta ahora.
                }
                break; // Termina de evaluar el estado obtenido.

            case 12:                                       // Si el estado actual es "doctype visibility".
                if (word[0] != ' ') {                      // Si la palabra obtenida no fue un espacio.
                    if (!strcmp((char*)"PUBLIC", word))    // Si el nombre de la etiqueta es "PUBLIC".
                        newXmlAttribute(word, NULL, item); // Crea un nuevo atributo al XmlItem actual.
                    else                                   // Si el nombre de la etiqueta no es "PUBLIC".
                        status = 0;                        // Indica que el archivo xml no es válido.

                    // printDebugXmlItem(NULL, "a", header->father, 1);                          // Muestra todos los XmlItem obtenidos hasta ahora.
                }
                break; // Termina de evaluar el estado obtenido.

            case 13:                                   // Si el estado actual es "dtd language".
                if (word[0] != ' ') {                  // Si la palabra obtenida no fue un espacio.
                    newXmlAttribute(word, NULL, item); // Crea un nuevo atributo al XmlItem actual.
                    // printDebugXmlItem(NULL, "a", header->father, 1);                          // Muestra todos los XmlItem obtenidos hasta ahora.
                }
                break; // Termina de evaluar el estado obtenido.

            case 14:                                   // Si el estado actual es "dtd source".
                if (word[0] != ' ') {                  // Si la palabra obtenida no fue un espacio.
                    newXmlAttribute(word, NULL, item); // Crea un nuevo atributo al XmlItem actual.
                    // printDebugXmlItem(NULL, "a", header->father, 1);                          // Muestra todos los XmlItem obtenidos hasta ahora.
                }
                break; // Termina de evaluar el estado obtenido.

            case 16: // Si el estado actual es "open start tag".

                if (body->father->father == NULL && body->numberChilds) { // Si está en el item que apunta al cuerpo del xml y el cuerpo del xml ya tenía una raíz.
                    status = 0;                                           // Indica que el archivo xml no es válido.
                    break;                                                // Sale del case.
                }
                item = newXmlItem(NULL, NULL, 3, body, 0); // Crea un nuevo XmlItem en el cuerpo.
                // printDebugXmlItem(NULL, "a", header->father, 1);                              // Muestra todos los XmlItem obtenidos hasta ahora.
                status = obtainXmlItems(file, header, item, status, word); // Inicia una ejecución recursiva.

                // printf("\nCome back %d %d\n", prevStatus, status);

                if (status)              // Si no se obtuvo un estado de error en la ejecución recursiva.
                    status = prevStatus; // Vuelve al estado antes de entrar a la ejecucuón recursiva.

                if (status == 21) { // Si antes de la ejcución recursiva el estado era "close start tag".

                    sprintf(itemValue, "%s\n", body->value); // Almacena el valor actual del item en itemValue.
                    /*-
                    printf("Value 1-1: -");                                                     // Muestra el value antes de actualizarlo.
                    printArray(itemValue);
                    printf("-\n");
                    printf("'Size: %d'\n", (int)(strlen(itemValue)));
                    */

                    for (int i = (int)(strlen(itemValue)); i < 1000000; i++) { // Obtiene todos los caracteres que hayan en la etiqueta.
                        itemValue[i] = fgetc(file);                            // Obtiene el siguiente caracter del archivo.
                        // printf("%c\n", itemValue[i]);                                         // Muestra el caracter obtenido.
                        if (feof(file)) { // Si terminó de recorrer el archivo.
                            status = 0;   // Indica que el xml no es válido.
                            break;        // Sale del ciclo.
                        }
                        elif (itemValue[i] == '<') {   // Si encuentra indicador de apertura de etiqueta.
                            itemValue[i] = 0;          // Indica que el final del valor del item es en la posición actual.
                            fseek(file, -1, SEEK_CUR); // Retrocede el cursor del archivo una posición.
                            break;                     // Sale del ciclo.
                        }
                    }

                    /*-
                    printf("Value 1-2: -");                                                     // Muestra el value luego de actualizarlo.
                    printArray(itemValue);
                    printf("-\n");
                    */
                    setXmlItemValue(body, itemValue); // Actualiza el valor del XmlItem actual.
                    // printDebugXmlItem(NULL, "a", body, 0);                                    // Muestra el item actual.
                }

                break; // Termina de evaluar el estado obtenido.

            case 17:                            // Si el estado actual es "start tag name".
                if (word[0] != ' ') {           // Si la palabra obtenida no fue un espacio.
                    setXmlItemName(body, word); // Pone nombre al XmlItem actual.
                    // printDebugXmlItem(NULL, "a", header->father, 1);                          // Muestra todos los XmlItem obtenidos hasta ahora.
                }
                break; // Termina de evaluar el estado obtenido.

            case 20:                                                // Si el estado actual es "tag attrib value".
                if (word[0] != ' ') {                               // Si la palabra obtenida no fue un espacio.
                    memcpy(attribValue, &word[1], strlen(word));    // Almacena el valor del atributo actual.
                    attribValue[strlen(word) - 2] = 0;              // Marca el fin del valor del atributo actual.
                    newXmlAttribute(attribName, attribValue, body); // Crea un nuevo atributo al XmlItem actual.
                    // printDebugXmlItem(NULL, "a", header->father, 1);                          // Muestra todos los XmlItem obtenidos hasta ahora.
                }
                break; // Termina de evaluar el estado obtenido.

            case 21: // Si el estado actual es "close start tag".

                sprintf(itemValue, "%s", body->value); // Almacena el valor actual del item en itemValue.
                /*-
                printf("Value 2-1: -");                                                         // Muestra el value antes de actualizarlo.
                printArray(itemValue);
                printf("-\n");
                printf("Size: '%d'\n", (int)(strlen(itemValue)));
                */

                for (int i = (int)(strlen(itemValue)); i < 1000000; i++) { // Obtiene todos los caracteres que hayan en la etiqueta.
                    itemValue[i] = fgetc(file);                            // Obtiene el siguiente caracter del archivo.
                    // printf("%c", itemValue[i]);                                               // Muestra el caracter obtenido.
                    if (feof(file)) { // Si terminó de recorrer el archivo.
                        status = 0;   // Indica que el xml no es válido.
                        break;        // Sale del ciclo.
                    }
                    elif (itemValue[i] == '<') {   // Si encuentra indicador de apertura de etiqueta.
                        itemValue[i] = 0;          // Indica que el final del valor del item es en la posición actual.
                        fseek(file, -1, SEEK_CUR); // Retrocede el cursor del archivo una posición.
                        break;                     // Sale del ciclo.
                    }
                }

                /*-
                printf("Value 2-2: -");                                                         // Muestra el value luego de actualizarlo.
                printArray(itemValue);
                printf("-\n");
                */

                setXmlItemValue(body, itemValue); // Actualiza el valor del XmlItem actual.
                // printDebugXmlItem(NULL, "a", body, 0);                                        // Muestra el item actual.
                break; // Termina de evaluar el estado obtenido.

            case 24:                              // Si el estado actual es "end tag name".
                if (word[0] != ' ') {             // Si la palabra obtenida no fue un espacio.
                    if (strcmp(body->name, word)) // Si el nombre de la etiqueta de cierre actual no es igual al de apertura.
                        status = 0;               // Indica que el archivo xml no es válido.

                    // printf("Valid: \"%s\" \"%s\"\n", body->name, word);                       // Muestra si el tag de cierre es igual al de apertura.

                    for (int i = (int)(strlen(body->value) - 1); i >= 0; i--) { // Recorre los caractereres del valor del item.
                        switch (body->value[i]) {                               // Evalúa el caracter actual.
                            case '\t':                                          // Si el caracter actual es una tabulación.
                            case '\n':                                          // Si el caracter actual es un salto de línea.
                            case ' ':                                           // Si el caracter actual es un espacio.
                                body->value[i] = 0;                             // Marca el fin del valor en el caracter actual.
                                break;                                          // Termina de evaluar el caracter actual.

                            default:    // Si el caracter actual no es ninguno de los caracteres indicados.
                                i = -1; // Pone i en -1 para que salga del ciclo.
                                break;  // Termina de evaluar el caracter actual.
                        }
                    }
                }

                body->value = (char*)realloc(body->value, (strlen(body->value) + 1) * sizeof(char)); // Redimesiona el tamaño del valor del item.
                // printDebugXmlItem(NULL, "a", body, 0);                                        // Muestra el item actual.
                break; // Termina de evaluar el estado obtenido.

            case 22:           // Si el estado actual es "close void tag".
            case 25:           // Si el estado actual es "close end tag".
                return status; // Vuelve a la anterior ejecución recursiva.
        }
    }

    // printf("Final status: %d\n", status);

    if (body->father->father != NULL || !body->numberChilds || !status) { // Si terminó de recorrer el archivo y no está en la raíz del cuerpo, la raíz del cuerpo no tiene hijos o se obtuvo un estado inválido.
        // printf("Error 1 in xml\n");
        status = 0;       // Indica que el archivo xml no es válido.
    } else                // Si se terminó de recorrer el archivo y está en la raíz del cuerpo.
        switch (status) { // Evalúa el último estado obtenido.
            case 1:       // Si el último estado obtenido fue "start status".
            case 2:       // Si el último estado obtenido fue "space at start".
            case 8:       // Si el último estado obtenido fue "close xml".
            case 15:      // Si el último estado obtenido fue "close doctype".
                // printf("Valid xml\n");
                break; // Termina de valuar el último estado.

            default:        // Si el último estado obtenido no fue ninguno de los indicados.
                status = 0; // Indica que el archivo xml no es válido.
                // printf("Error 2 in xml\n");
                break; // Termina de valuar el último estado.
        }

    return status; // Devuelve el último estado obtenido.
}

/**
 * FIXME: Definición de {@code getXmlItems}.
 * Crea XmlItem en base a un array con la ruta del archivo xml.
 */
LANGUAGE DLLIMPORT CALLING XmlItem* getXmlItems(char* fileSource) {
    FILE* file = fopen(fileSource, "r+");                       // Abre el archivo indicado por fileSource.
    XmlItem* root = NULL;                                       // Crea el XmlItem que tendrá la raíz del archivo xml leído.
    XmlItem* header = NULL;                                     // Crea el XmlItem que tendrá el encabezado del archivo xml leído.
    XmlItem* body = NULL;                                       // Crea el XmlItem que tendrá el cuerpo del archivo xml leído.
    int status = 0;                                             // Crea variable que tendrá el último estado obtenido.

    if (file != NULL) {                                         // Si el archivo existe.
        char word[500] = "";                                    // Crea array para almacenar cada palabra del xml.

        root = newXmlItem(NULL, NULL, 3, NULL, 0);              // Crea la raíz de los XmlItem.
        header = newXmlItem((char*)"header", "", 3, root, 0);   // Crea el XmlItem que tendrá el encabezado.
        body = newXmlItem((char*)"body", "", 3, root, 0);       // Crea el XmlItem que tendrá el cuerpo.
        status = obtainXmlItems(file, header, body, 1, word);   // Obtiene los XmlItem del archivo indicado por fileSource.

        if (!status)                                            // Si el último estado obtenido fue de error.
            deleteXmlItem(&root);                               // Borra todos los XmlItem.
    }

    fclose(file);                                               // Cierra el archivo indicado por fileSource.
    return root;                                                // Devuelve la raíz de los XmlItem obtenidos.
}
