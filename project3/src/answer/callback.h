#ifndef VIRTUALMEMFRAMEWORKC_CALLBACK_H
#define VIRTUALMEMFRAMEWORKC_CALLBACK_H

/**
 * This method is called by systemInit. Any initialization you need to do can be done here in this method. This is provided
 * primarily for initializing your data structures and setting initial data.
 */
void shutdownCallback();

/**
 * This method is called by systemShutdown. Any shutdown that needs to be done should be done here so that you can
 * clean up memory or reset anything that needs be reset before the next test.
 */
void startupCallback();
#endif //VIRTUALMEMFRAMEWORKC_CALLBACK_H
