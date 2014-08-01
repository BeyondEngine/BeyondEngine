#ifndef BEYOND_ENGINE_SKELETALANIMATION_JSON_H_INCLUDE
#define BEYOND_ENGINE_SKELETALANIMATION_JSON_H_INCLUDE

#ifdef __cplusplus
extern "C" {
#endif


#define MALLOC(TYPE,COUNT) ((TYPE*)malloc(sizeof(TYPE) * COUNT))
#define CALLOC(TYPE,COUNT) ((TYPE*)calloc(COUNT, sizeof(TYPE)))
#define NEW(TYPE) CALLOC(TYPE,1)

    /* Gets the direct super class. Type safe. */
#define SUPER(VALUE) (&VALUE->super)

    /* Cast to a super class. Not type safe, use with care. Prefer SUPER() where possible. */
#define SUPER_CAST(TYPE,VALUE) ((TYPE*)VALUE)

    /* Cast to a sub class. Not type safe, use with care. */
#define SUB_CAST(TYPE,VALUE) ((TYPE*)VALUE)

    /* Casts away const. Can be used as an lvalue. Not type safe, use with care. */
#define CONST_CAST(TYPE,VALUE) (*(TYPE*)&VALUE)

    /* Gets the vtable for the specified type. Not type safe, use with care. */
#define VTABLE(TYPE,VALUE) ((_##TYPE##Vtable*)((TYPE*)VALUE)->vtable)

    /* Frees memory. Can be used on const types. */
#define FREE(VALUE) free((void*)VALUE)

    /* Allocates a new char[], assigns it to TO, and copies FROM to it. Can be used on const types. */
#define MALLOC_STR(TO,FROM) strcpy(CONST_CAST(char*, TO) = (char*)malloc(strlen(FROM) + 1), FROM)

#ifdef __STDC_VERSION__
#define FMOD(A,B) fmodf(A, B)
#else
#define FMOD(A,B) (float)fmod(A, B)
#endif


/* Json Types: */
#define Json_False 0
#define Json_True 1
#define Json_NULL 2
#define Json_Number 3
#define Json_String 4
#define Json_Array 5
#define Json_Object 6

/* The Json structure: */
typedef struct Json {
    struct Json* next;
    struct Json* prev; /* next/prev allow you to walk array/object chains. Alternatively, use getSize/getItem */
    struct Json* child; /* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */

    int type; /* The type of the item, as above. */
    int size; /* The number of children. */

    const char* valueString; /* The item's string, if type==Json_String */
    int valueInt; /* The item's number, if type==Json_Number */
    float valueFloat; /* The item's number, if type==Json_Number */

    const char* name; /* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
} Json;

/* Supply a block of JSON, and this returns a Json object you can interrogate. Call Json_dispose when finished. */
Json* Json_create (const char* value);

/* Delete a Json entity and all subentities. */
void Json_dispose (Json* json);

/* Get item "string" from object. Case insensitive. */
Json* Json_getItem (Json* json, const char* string);
const char* Json_getString (Json* json, const char* name, const char* defaultValue);
float Json_getFloat (Json* json, const char* name, float defaultValue);
int Json_getInt (Json* json, const char* name, int defaultValue);

/* For analysing failed parses. This returns a pointer to the parse error. You'll probably need to look a few chars back to make sense of it. Defined when Json_create() returns 0. 0 when Json_create() succeeds. */
const char* Json_getError (void);

#ifdef __cplusplus
}
#endif

#endif /* SPINE_JSON_H_ */
