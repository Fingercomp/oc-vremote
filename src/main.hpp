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
    CLOSING
};

// The settings changed at runtime
namespace programSettings {
    State state = State::INITIALIZING;
}
