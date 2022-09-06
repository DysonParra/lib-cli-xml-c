#ifndef XML_H_

#define XML_H_

/* Includes for the header file */

#ifdef __cplusplus
#define LANGUAGE extern "C"
#elif defined __GNUC__
#define LANGUAGE extern
#endif

#ifdef XML_STATIC_
#define DLLIMPORT
#elif defined BUILDING
#define DLLIMPORT __declspec(dllexport)
#else
#define DLLIMPORT __declspec(dllimport)
#endif

#define CALLING __cdecl
//#define CALLING __stdcall

/* Macros and definitions for the source file. */
#ifdef XML_DEFINITIONS_
#define elif else if
#define null NULL
#define String char*
#endif

DLLIMPORT typedef struct XmlItem XmlItem;
DLLIMPORT typedef struct XmlItem XmlItem;
DLLIMPORT typedef struct XmlAttribute XmlAttribute;
LANGUAGE DLLIMPORT CALLING XmlItem* newXmlItem(char* name, char* value, int type, XmlItem* ref, int brother);
LANGUAGE DLLIMPORT CALLING void deleteXmlItem(XmlItem** toErase);
LANGUAGE DLLIMPORT CALLING XmlAttribute* newXmlAttribute(char* name, char* value, XmlItem* item);
LANGUAGE DLLIMPORT CALLING void deleteXmlAttribute(XmlAttribute** toErase);
LANGUAGE DLLIMPORT CALLING void setXmlItemName(XmlItem* aux, char* name);
LANGUAGE DLLIMPORT CALLING void setXmlItemValue(XmlItem* aux, char* value);
LANGUAGE DLLIMPORT CALLING XmlItem* findXmlItem(XmlItem* father, int nargs, ...);
LANGUAGE DLLIMPORT CALLING void printDebugXmlItem(char* fileSource, char* openType, XmlItem* root, int childs);
LANGUAGE DLLIMPORT CALLING void printXmlItem(char* fileSource, char* openType, XmlItem* item, int childs);
LANGUAGE DLLIMPORT CALLING void printXmlFile(char* fileSource, char* openType, XmlItem* root);
LANGUAGE DLLIMPORT CALLING XmlItem* getXmlItems(char* fileSource);

struct XmlItem {
    /* Es el nivel del XmlItem. */
    int level;
    /* Es donde inicia el nombre del XmlItem. */
    char* name;
    /* Es donde inicia el valor del XmlItem. */
    char* value;
    /* Es el tipo de XmlItem (1 = xml, 2 = DOCTYPE, 3 = tag). */
    int type;
    /* Es la cantidad de hijos del XmlItem. */
    int numberChilds;
    /* Es la cantidad de atributos del XmlItem. */
    int numberAttributes;
    /* Es el padre del XmlItem. */
    XmlItem* father;
    /* Es el anterior hermano de XmlItem. */
    XmlItem* prevBrother;
    /* Es el próximo hermano del XmlItem. */
    XmlItem* nextBrother;
    /* Es el primer hijo del XmlItem. */
    XmlItem* firstChild;
    /* Es el primer atributo del XmlItem. */
    XmlAttribute* firstXmlAttribute;
};

struct XmlAttribute {
    /* Es donde inicia el nombre del XmlAttribute. */
    char* name;
    /* Es donde inicia el valor del XmlAttribute. */
    char* value;
    /* Es el XmlItem al que pertenece el XmlAttribute. */
    XmlItem* father;
    /* Es el anterior hermano de XmlAttribute. */
    XmlAttribute* prevBrother;
    /* Es el próximo hermano del XmlAttribute. */
    XmlAttribute* nextBrother;
};

#if !defined BUILDING
#undef LANGUAGE
#undef DLLIMPORT
#undef CALLING
#endif

#endif
