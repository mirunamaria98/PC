#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

//functie ce parseaza in format json username-ul si parola
char * parsare_json_username_password(char * username, char *password) {

    JSON_Value *value = json_value_init_object();
    JSON_Object *object = json_value_get_object(value);
    json_object_set_string(object,"username",username);
    json_object_set_string(object,"password",password);
    return json_serialize_to_string(value);    


}
//functie ce parseaza in format json informatiile unei carti ce urmeaza sa fie adaugata
char * parsare_json_book(char* title, char* author, char* genre, int page_number, char* publisher) {
    JSON_Value *value = json_value_init_object();
    JSON_Object *object = json_value_get_object(value);
    json_object_set_string(object,"title",title);
    json_object_set_string(object,"author",author);
    json_object_set_string(object,"genre",genre);
    json_object_set_number(object,"page_count",(double)page_number);
    json_object_set_string(object,"publisher",publisher);
    return json_serialize_to_string(value);
}

char *compute_get_request(char *host, char *url, char *query_params,
                            char *cookies, char * token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "HOST: %s " ,host);
    compute_message(message, line);

    if(token != NULL) {
        sprintf(line, "Authorization: Basic %s", token);
        compute_message(message, line);
    }

    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    if (cookies != NULL) {
        sprintf(line,"Cookie: %s", cookies);
        compute_message(message, line);
    }
    // Step 4: add final new line
    compute_message(message, "");
    free(line);
    return message;
}

char *compute_post_request(char *host, char *url, char* content_type, char **body_data,
                            int body_data_fields_count, char *cookies,char* token)
{
 char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    
    // Step 2: add the host
    sprintf(line, "HOST: %s", host);
    compute_message(message, line);
    /* Step 3: add necessary headers (Content-Type and Content-Length are mandatory)
            in order to write Content-Length you must first compute the message size
    */
    sprintf(line, "Content-Type: %s",content_type);
    compute_message(message, line);
    int length = 0;
    if(body_data_fields_count == 2){
        body_data_buffer = parsare_json_username_password(body_data[0] ,body_data[1]);
    }
    else{
        body_data_buffer = parsare_json_book(body_data[0] ,body_data[1],
            body_data[2],atoi(body_data[3]),body_data[4]);
    }
    length = strlen(body_data_buffer);
    sprintf(line, "Content-Length: %d",(length));
    compute_message(message, line);

    // Step 4 (optional): add cookies
     if(token != NULL)
    {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    if (cookies != NULL) {
            sprintf(line, "Cookie: %s",cookies);
            compute_message(message, line);
    }
    // Step 5: add new line at end of header
     compute_message(message, "");

    // Step 6: add the actual payload data
    memset(line, 0, LINELEN);
    compute_message(message, body_data_buffer);

    free(line);
    free(body_data_buffer);
    return message;
}
char *compute_delete_request(char *host, char *url, char *query_params,
                            char *cookies, char * token)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Step 1: write the method name, URL, request params (if any) and protocol type
    if (query_params != NULL) {
        sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // Step 2: add the host
    sprintf(line, "HOST: %s " ,host);
    compute_message(message, line);

    if(token != NULL) {
        sprintf(line, "Authorization: Basic %s", token);
        compute_message(message, line);
    }

    // Step 3 (optional): add headers and/or cookies, according to the protocol format
    if (cookies != NULL) {
        sprintf(line,"Cookie: %s", cookies);
        compute_message(message, line);
    }
    // Step 4: add final new line
    compute_message(message, "");
    free(line);
    return message;
}