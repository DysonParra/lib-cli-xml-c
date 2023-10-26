/*
 * @fileoverview    {FileName}
 *
 * @version         2.0
 *
 * @author          Dyson Arley Parra Tilano <dysontilano@gmail.com>
 *
 * @copyright       Dyson Parra
 * @see             github.com/DysonParra
 *
 * History
 * @version 1.0     Implementation done.
 * @version 2.0     Documentation added.
 */
#include "xml.h"
#include "tester.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Entrada principal del sistema.
 *
 * @param argc cantidad de argumentos pasados en la linea de comandos.
 * @param argv argumentos de la linea de comandos.
 * @return {0} si el programa se ejecutó correctamente.
 */
int main(int argc, char** argv) {

    char output[256] = "";
    // memcpy(output, (char*)"out.tmp", 8);

    XmlItem* root = NULL;
    root = getXmlItems((char*)"test/file.xml");
    printDebugXmlItem(output, "w+", root, 1);
    printXmlItem(output, "a", root, 1);
    printXmlFile(output, "a", root);

    /*-
    XmlItem* root = newXmlItem(NULL, NULL, 3, NULL, 0);
    XmlItem* aux1;
    //XmlItem* aux2;
    //XmlItem* aux3;
    //XmlItem* aux4;

    aux1 = newXmlItem((char*)"firstName", (char*)"firstVal", 3, root, 0);
    newXmlItem((char*)"secName", (char*)"secVal", 3, aux1, 0);
    newXmlItem((char*)"thirdName", (char*)"thirdVal", 3, root, 0);
    newXmlItem((char*)"fourthName", (char*)"", 3, root, 0);
    //printDebugXmlItem((char*)"", "a", root, 1);

    //XmlAttribute* atb1;
    //XmlAttribute* atb2;
    //XmlAttribute* atb3;
    //XmlAttribute* atb4;

    //atb1 =
    newXmlAttribute((char*)"firstAttName",  (char*)"firstAttVal", root);
    //atb2 =
    newXmlAttribute((char*)"secAttName",    (char*)"secAttVal", aux1);
    //atb3 =
    newXmlAttribute((char*)"thirdAttName",  (char*)"thirdAttVal", aux1);
    //atb4 =
    newXmlAttribute((char*)"fouthAttName",  (char*)"fouthAttVal", aux1);
    //printDebugXmlItem((char*)"", "a", root, 1);

    char array[200] = "123456789";
    setName(root, "abcde");
    setValue(root, array);

    char output[50] = "";
    //memcpy(output, (char*)"out0.tmp", 9);
    printDebugXmlItem(output, "a", root, 1);
    //printXmlItem((char*)"", "a", root, 1);
    */
    /*-
    printf("%p\n", aux1);
    deleteXmlItem(&aux1);
    printf("%p\n", aux1);
    printDebugXmlItem(output "a",, root, 1);
    */

    /*-
    printf("%p\n", atb1);
    deleteXmlAttribute(&atb1);
    printf("%p\n", atb1);
    printDebugXmlItem(output, "a", root, 1);
    */
    /*-
    XmlItem* aux4 = findXmlItem(root, 1, (char*)"third name");
    printDebugXmlItem((char*)"", "a", aux4, 1);
    */

    /*-
    char input[500] = "abc\n\n\nc\t\te";
    char output[500] = "";
    printf("%s\n", input);
    printArray(input);
    printf("\n");
    sprintArray(output, input);
    printf("%s", output);
    */

    return 0;
}
