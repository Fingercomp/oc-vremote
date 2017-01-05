#pragma once

#include <queue>


enum class State {
    // Non-threaded environment; initializing the program
    INITIALIZING,
    // Threaded environment; listening for connections
    WAITING_FOR_CONNECTION,
    // Threaded environment; a client connected, waiting for user confirmation
    CONNECTION_ATTEMPT,
    // Threaded environment; a client connected, waiting for authorization and InitialData message
    AUTHORIZATION,
    // Threaded environment; a client connected and authorized, exchaning the data
    CONNECTED,
    // Threaded environment; signalling threads to end
    CLOSING,
    // Threaded environment; some critical error occured, e.g., could not set up a listener
    ERROR,
    // Threaded environment; a client hasn't responded to the Ping message in time
    TIMEOUT
};

