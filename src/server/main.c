#include <libwebsockets.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_MESSAGE_SIZE 1024

// Structure to store data for each WebSocket session
struct per_session_data {
    int role;
    int inform_role_change;
    int message_sent;
    int last_sent;
};

struct GLOBAL_DATA {
    int packet_id;
    int total_clients;
    char message[MAX_MESSAGE_SIZE];
    int has_response;
    char response[MAX_MESSAGE_SIZE];
} global_data = {0};

// Callback function for WebSocket server messages
int callback(struct lws *wsi, enum lws_callback_reasons reason, struct per_session_data *user, void *in, size_t len) {
    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED: // Handle new connection event
            // Add a timer to send a timestamp every second
            user->role = 0;
            user->inform_role_change = 0;
            user->message_sent = 0;
            user->last_sent = 0;
            global_data.total_clients++;
            lws_callback_on_writable(wsi);
            break;

        case LWS_CALLBACK_RECEIVE: // Handle incoming data event
            // Print the received message
            char* message = (char *)malloc(len + 1);
            strncpy(message, in, len);
            message[len] = '\0';


            if (strcmp(message, "PASSWORD") == 0) {
                user->role = 1;
                user->inform_role_change = 1;
            } else if (user->role){
                global_data.packet_id++;
                sprintf(global_data.message, "%s", message);
                lws_callback_on_writable_all_protocol(lws_get_context(wsi), lws_get_protocol(wsi));
            }

            char prefix[1024];
            memcpy(prefix, message, 7);
            if (strcmp(prefix, "RESULT_") == 0){
                global_data.has_response = 1;
                memcpy(global_data.response, message, len);
            }

            user->message_sent++;

            break; 

        case LWS_CALLBACK_SERVER_WRITEABLE: // Handle send data event
            // Send the timestamp
            if (user->inform_role_change) {
                char message[20];
                sprintf(message, "ROLE___:%d", user->role);
                lws_write(wsi, message, 9, LWS_WRITE_TEXT);
                user->inform_role_change = 0;
            } else if (user->last_sent != global_data.packet_id && !user->role) {
                lws_write(wsi, global_data.message, strlen(global_data.message), LWS_WRITE_TEXT);
                user->last_sent = global_data.packet_id;
            } else if (user->role && global_data.has_response) {
                lws_write(wsi, global_data.response, strlen(global_data.response), LWS_WRITE_TEXT);
                global_data.has_response = 0;
            } else {           
                time_t t = time(NULL);
                char timestamp[100];
                sprintf(timestamp, "TIME___:%ld", t);
                lws_write(wsi, timestamp, strlen(timestamp), LWS_WRITE_TEXT);
            }
            break;
        
        case LWS_CALLBACK_CLOSED: // Handle close connection event
            global_data.total_clients--;
            break;
        // Other events omitted
    }
    return 0;
}

int main(int argc, char **argv) {
    global_data.total_clients = 0;
    global_data.packet_id = 0;
    global_data.has_response = 0;
    // Create the WebSocket protocol
    static struct lws_protocols protocols[] = {
        {
            "wss", // Protocol name, should match the WebSocket protocol in the frontend code
            callback, // Callback function pointer
            sizeof(struct per_session_data), // Size of data for each session (connection)
            0, // No additional protocol parameters
            NULL, NULL, NULL
        },
        { NULL, NULL, 0, 0 } // Protocol list ends with NULL
    };

    // Create the WebSocket context
    struct lws_context_creation_info info = {
        .port = 3001, // Listening port number
        .protocols = protocols, // Protocol list
        .options = LWS_SERVER_OPTION_VALIDATE_UTF8 | LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT,
        .ssl_cert_filepath = "../cert/server.crt",
        .ssl_private_key_filepath = "../cert/server.key"
    };
    struct lws_context *context = lws_create_context(&info);

    // Check if WebSocket context creation was successful
    if (!context) {
        printf("Failed to create WebSocket context.\n");
        return -1;
    }

    // Enter the loop and wait for WebSocket connections
    while (1) {
        lws_service(context, 50);
    }

    // Clean up and close the WebSocket context
    lws_context_destroy(context);

    return 0;
}