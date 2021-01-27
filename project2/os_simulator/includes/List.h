#ifndef STRUCTURES_LIST_H
#define STRUCTURES_LIST_H

/**
 * Creates a list with a unique identifier. The identifier is a UUID which is
 * used to do any operations on the list.
 * @return A unique identifier for the list.
 */
const char* createNewList();

/**
 * Destroys the list from memory so it is no longer accessible.
 * @param listIdentifier identifier of the list to destroy.
 */
void destroyList(const char* listIdentifier);

/**
 * Add an item to the list, the item must be a void pointer. You can make a
 * pointer to any object and cast it to a void pointer. It will add the item to
 * end of the list.
 *
 * @param listIdentifier The list identifier for the list you want to add to.
 * @param item The item to add to the list.
 */
void addToList(const char* listIdentifier, void* item);

/**
 * Add an item to the list at the specified index, the item must be a void
 * pointer. You can make a pointer to any object and cast it to a void pointer.
 * It will add the item to location in the list that is specified.
 *
 * @param listIdentifier
 * @param index
 * @param item
 */
void addToListAtIndex(const char* listIdentifier, int index, void* item);

/**
 * Remove an item from the list, the item must be a void pointer. You can make
 * a pointer to any object and cast it to a void pointer. It will remove the
 * item specified from the list.
 *
 * @param listIdentifier The list identifier for the list you want remove from.
 * @param item The item to be removed from the list.
 */
void removeFromList(const char* listIdentifier, void* item);

/**
 * Remove an item from the list at a given index. The function will return the
 * item removed.
 *
 * @param listIdentifier The list identifier for the list you want remove from.
 * @param index The index to remove.
 * @return The item removed from the list.
 */
void* removeFromListAtIndex(const char* listIdentifier, int index);

/**
 * Get the size of the list.
 *
 * @param listIdentifier The list identifier for the list you want remove from.
 * @return The number of items in the list.
 */
int listSize(const char* listIdentifier);

/**
 * Returns an item from a list at the given index.
 *
 * @param listIdentifier The list identifier for the list you want get from.
 * @param index The index of the item you want to get.
 * @return The item at the given index.
 */
void* listGet(const char* listIdentifier, int index);

/**
 * Returns an item from a list at the given index.
 *
 * @param listIdentifier The list identifier for the list you want get from.
 * @param item The item you want to get.
 * @return The item you want to get.
 */
void* listGet(const char* listIdentifier, void* item);

/**
 * Sorts a list based on the function given, the function takes in two void
 * pointers and return true if the first item is less than or equal to the
 * second, it returns false otherwise.
 *
 * @param listIdentifier The list to sort
 * @param func A function which returns true if the first item is less than or
 * equal to the second, false otherwise.
 */
void sortList(const char* listIdentifier, bool (*func)(void*, void*));

#endif  // WAIT_THIS_IS_NOT_A_HEADER_GUARD_PUT_ME_IN_YOUR_LOCK_CPP_FOR_A_SURPRISE
