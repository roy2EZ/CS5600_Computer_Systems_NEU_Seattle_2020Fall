/**
 * This is a set of macros that allows you to create and manipulate maps. These
 * are all macros as the backing classes are templates. You can specify the key
 * type for your maps but the value types are all void *. All maps are
 * idenitified by strings. Below is an example of using the following macros.
 *
 * //Create a map called foo which has a key type of const char*
 * const char* foo = CREATE_MAP(char*);
 *
 * // Put a value in the map
 * char* key ="test";
 * char* value = "testValue";
 * PUT_IN_MAP(char*, foo, key, (void*)value);
 *
 * // Check if value exists in map
 * bool inMap = MAP_CONTAINS(char*, foo, key);
 *
 * // Get a value from a map
 * char* copyOfValue = GET_FROM_MAP(char*, foo, key);
 *
 * The last macro will run a function on every element in the map. The function
 * must return void and take two arguments. The first argument is of the same
 * type as the key of the map, the second is void pointer can take the value.
 * See example below above macro.
 *
 */

#ifndef FRAMEWORK_MAP_H
#define FRAMEWORK_MAP_H
#include "structures/MapManager.h"

/**
 * Creates a map and returns a UUID as a const char* to uniquely identify a map.
 * @param keyType The type of the key for this map.
 */
#define CREATE_MAP(keyType) MapManager<keyType>::getInstance()->create()

/**
 * Puts a value in a map.
 * @param keyType The type of the key for this map.
 * @param mapIdentifier The unique string that identifies the map you want to
 * put a value in.
 * @param key A key of type keyType to put into the map.
 * @param value A value of type void* to be put in the map associated with the
 * key given.
 */
#define PUT_IN_MAP(keyType, mapIdentifier, key, value) \
    MapManager<keyType>::getInstance()->put(mapIdentifier, key, value);

/**
 * Removes a value from a map.
 * @param keyType The type of the key for this map.
 * @param mapIdentifier The unique string that identifies the map you want to
 * remove from.
 * @param key A key of type keyType to remove from the map.
 * @return The value that was removed from the map.
 */
#define REMOVE_FROM_MAP(keyType, mapIdentifier, key) \
    MapManager<keyType>::getInstance()->remove(mapIdentifier, key);

/**
 * Gets a value from a map.
 * @param keyType The type of the key for this map.
 * @param mapIdentifier The unique string that identifies the map you want to
 * get from.
 * @param key A key of type keyType to get from the map.
 * @return The value associated with the given key.
 */

#define GET_FROM_MAP(keyType, mapIdentifier, key) \
    MapManager<keyType>::getInstance()->get(mapIdentifier, key);

/**
 * Checks if a map holds a given key.
 * @param keyType The type of the key for this map.
 * @param mapIdentifier The unique string that identifies the map you want to
 * check.
 * @param key A key of type keyType to check the map.
 * @return true if the key is in the map, false otherwise
 */
#define MAP_CONTAINS(keyType, mapIdentifier, key) \
    MapManager<keyType>::getInstance()->contains(mapIdentifier, key);

/**
 * Executes a function on every value in a map.
 * @param keyType The type of the key for this map.
 * @param mapIdentifier The unique string that identifies the map you want to
 * check.
 * @param func A function that takes two arguments, one of keytype, the other of
 * void* to take the value and returns nothing.
 *
 * For example if you have a function such as:
 * void foo(char* key, void* value) {
 *  int* val = (int*)value;
 *  *val = *val+5;
 * }
 *
 * const char* map = CREATE_MAP(char*);
 * int* first = malloc(sizeof(int));
 * *first = 5;
 * int* second = malloc(sizeof(int));
 * *second = 10;
 * PUT_IN_MAP(char*, map, "first", first);
 * PUT_IN_MAP(char*, map, "second", second);
 * EXEC_FUNC_ON_MAP(char*, map, foo);
 *
 * The above code puts two values in the map and then increases every value
 * by 5.
 */
#define EXEC_FUNC_ON_MAP(keyType, mapIdentifier, func) \
    MapManager<keyType>::getInstance()->execOnMap(mapIdentifier, func);

#endif