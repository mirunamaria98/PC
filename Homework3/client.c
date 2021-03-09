#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"
//STOIAN MIRUNA MARIA 325CB
//functie ce cauta cooki-ul in raspunsul primit la login
int get_cookies(char str[], char *s1)
{
    char *cookie = strstr(str, "Set-Cookie");
    if(cookie == NULL) 
        return 0;
    char *cook1 = strtok(cookie, ";");

    strcpy(s1, cook1 + strlen("Set-Cookie: "));
    return 1;
}
//cauta cookiul in raspunsul primit la enter_library
void parsare_json_token(char *response, char* token) {
	char *json = strchr(response,'{');
	JSON_Value *value = json_parse_string(json);
	JSON_Object *object = json_value_get_object(value);
	char *token_p = (char*) json_object_get_string(object, "token");
	if(token_p != NULL) strcpy(token, token_p);
    free(token_p);
    free(object);
    free(value);

}
int main(int argc, char *argv[])
{
    char *message = NULL;
    char *response= NULL;
    int sockfd;
    char* buff = (char*)malloc(BUFLEN*sizeof(char)); 
    char *token;
    char copieResponse[BUFLEN];
    char *cookie1 ;
    char* username = (char*)malloc(BUFLEN*sizeof(char)); 
   	char* password = (char*)malloc(BUFLEN*sizeof(char)); 
   	char* username1 = (char*)malloc(BUFLEN*sizeof(char)); 

   	sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
    while(1) {
    	//conexiunea se va termina atunci cand se va citi de la tastatuta exit
    	scanf("%s", buff);
    	if(strncmp (buff,"exit",4) == 0) {
    		break;
    	}
    	//inregistreaza un nou client
    	if(strcmp(buff,"register") == 0) {
    		int body_data_count = 2;
    		char ** body_data_buff = (char**) malloc(body_data_count* sizeof(char*));
    		printf("username=");
    		scanf("%s", username);
    		printf("password=" );
    		scanf("%s", password);

			body_data_buff[0] = (char*)malloc(sizeof(char));
    		strcpy(body_data_buff[0],username);
   			body_data_buff[1] = (char*)malloc(sizeof(char));
    		strcpy(body_data_buff[1],password);    		

    		sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
		    message = compute_post_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com", 
		    	"/api/v1/tema/auth/register","application/json",body_data_buff,body_data_count,NULL,NULL);
		    puts(message);
		    send_to_server(sockfd, message);
		    response = receive_from_server(sockfd);
		    puts(response);

		    free(body_data_buff[0]);
		    free(body_data_buff[1]);
		    free(body_data_buff);
            free(message);
            free(response);

    	}
    	//realizeaza logarea unui client deja inregistrat
    	else if(strcmp(buff,"login") == 0) {

    		int body_data_count = 2;
    		char ** body_data_buff = (char**) malloc(body_data_count* sizeof(char*));
    		printf("username=");
    		scanf("%s", username);
    		printf("password=" );
    		scanf("%s", password);

    		if(strcmp(username,username1) == 0) {
    			printf("YOU ARE ALREADY LOGGED IN\n");
    			continue;
    		}

    		token = calloc (BUFLEN, sizeof (char));
    		cookie1 = calloc (BUFLEN, sizeof (char));


			body_data_buff[0] = (char*)malloc(sizeof(char));
    		strcpy(body_data_buff[0],username);
   			body_data_buff[1] = (char*)malloc(sizeof(char));
    		strcpy(body_data_buff[1],password);    		

    		sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
		    message = compute_post_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
		     "/api/v1/tema/auth/login","application/json",body_data_buff,body_data_count,NULL,NULL);
		    puts(message);
		    send_to_server(sockfd, message);
		    response = receive_from_server(sockfd);
		    puts(response);

		    strcpy(copieResponse,response);
		    if(get_cookies(copieResponse,cookie1) == 0) {
                printf("INCORECT CREDENTIALS \n");
                continue;
            }
		    strcpy(username1,username);

		    free(body_data_buff[0]);
		    free(body_data_buff[1]);
		    free(body_data_buff);
            free(message);
            free(response);

    	}
    	//se realizeaza accesul in library
    	else if(strcmp(buff,"enter_library") == 0) {
    		if(cookie1 == NULL) {
    			printf("YOU ARE NOT LOGGED IN\n");
    			continue;
    		}
    		sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
    		message = compute_get_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
    		 "/api/v1/tema/library/access","application/json",cookie1,NULL);
		    puts(message);
		    send_to_server(sockfd, message);
		    response = receive_from_server(sockfd);
		    puts(response);
		    strcpy(copieResponse,response);
            free(message);
            free(response);

    	}
    	//adaugarea unei carti noi
    	else if(strcmp(buff,"add_book") == 0) {
    		parsare_json_token(copieResponse,token);
    		if(token == NULL){
    			printf("YOU ARE NOT LOGGED IN\n");
    			continue;
    		}
    		int body_data_count = 5;
    		char ** body_data_buff = (char**) malloc(body_data_count* sizeof(char*));
    		char* title = (char*)malloc(BUFLEN*sizeof(char)); 
    		char* author = (char*)malloc(BUFLEN*sizeof(char)); 
    		char* genre = (char*)malloc(BUFLEN*sizeof(char)); 
    		char* publisher = (char*)malloc(BUFLEN*sizeof(char)); 
    		char* page_count = (char*)malloc(BUFLEN*sizeof(char)); 
    		printf("title=");
    		scanf("%s", title);
    		printf("author=");
    		scanf("%s", author);
    		printf("genre=");
    		scanf("%s", genre);
    		printf("page_count=");
    		scanf("%s", page_count);
    		if(atoi(page_count) < 0 || atoi(page_count) == 0) {
    			printf("NOT A VALID PAGE NUMBER. START OVER WITH ADD_BOOK\n");
    			continue;
    		}
    		printf("publisher=");
    		scanf("%s", publisher);
    		body_data_buff[0] = (char*)malloc(sizeof(char));
    		strcpy(body_data_buff[0],title);
    		body_data_buff[1] = (char*)malloc(sizeof(char));
    		strcpy(body_data_buff[1],author);
    		body_data_buff[2] = (char*)malloc(sizeof(char));
    		strcpy(body_data_buff[2],genre);
    		body_data_buff[3] = (char*)malloc(sizeof(char));
    		strcpy(body_data_buff[3],page_count);
    		body_data_buff[4] = (char*)malloc(sizeof(char));
    		strcpy(body_data_buff[4],publisher);

    		sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
    		message = compute_post_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
    		 "/api/v1/tema/library/books","application/json",body_data_buff,body_data_count,NULL,token);
		    puts(message);
		    send_to_server(sockfd, message);
		    response = receive_from_server(sockfd);
		    puts(response);
		    strcpy(copieResponse,response);

		    for(int i=0;i< 5; i++ ){
		    	free(body_data_buff[i]);
		    }
		    free(body_data_buff);
		    free(title);
		    free(author);
		    free(genre);
		    free(page_count);
		    free(publisher);
            free(message);
            free(response);

    	}
    	//afisarea tutror cartilor existente
    	else if(strcmp(buff,"get_books") == 0) {
    		if(token == NULL){
    			printf("YOU ARE NOT LOGGED IN\n");
    			continue;
    		}
    		parsare_json_token(copieResponse,token);
    		sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
    		message = compute_get_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
    		 "/api/v1/tema/library/books","application/json",NULL,token);
		    puts(message);
		    send_to_server(sockfd, message);
		    response = receive_from_server(sockfd);
		    puts(response);
		    strcpy(copieResponse,response);
            free(message);
            free(response);
    	}
    	//se afiseaza informatii despre cartea cu id-ul citit de la tastatura
    	//daca id-ul nu este gasit sau acesta este incorect se va afisa mesaj de eroare
    	else if(strcmp(buff,"get_book") == 0) {
    		parsare_json_token(copieResponse,token);
    		if(token == NULL){
    			printf("YOU ARE NOT LOGGED IN\n");
    			continue;
    		}
    		char id[BUFLEN] ;
    		printf("id=");
    		scanf("%s", id);

    		char string[BUFLEN] ="/api/v1/tema/library/books/";
    		strcat(string,id);
    		printf("string %s\n", string);
    		sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
    		message = compute_get_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
    		 string,"application/json",NULL,token);
		    puts(message);
		    send_to_server(sockfd, message);
		    response = receive_from_server(sockfd);
		    puts(response);
		    strcpy(copieResponse,response);
            free(message);
            free(response);

    	}
    	//sterge cartea cu id-ul citit de la tastatura
    	else if(strcmp(buff,"delete_book") == 0) {
    		parsare_json_token(copieResponse,token);
    		if(token == NULL){
    			printf("YOU ARE NOT LOGGED IN\n");
    			continue;
    		}
    		char id[BUFLEN] ;
    		printf("id=");
    		scanf("%s", id);

    		char string[BUFLEN] ="/api/v1/tema/library/books/";
    		strcat(string,id);
    		sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
    		message = compute_delete_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
    		string,"application/json",NULL,token);
		    puts(message);
		    send_to_server(sockfd, message);
		    response = receive_from_server(sockfd);
		    puts(response);
		    strcpy(copieResponse,response);
            free(message);
            free(response);
    	}
        else if(strcmp(buff,"logout") == 0) {
            token = NULL;
            free(token);
            sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
             "/api/v1/tema/auth/logout","application/json",cookie1,NULL);
            puts(message);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            puts(response);
            strcpy(copieResponse,response);
            cookie1 = NULL;
            free(cookie1);
            username1 = NULL;
            free(username1);
            username1 = (char*)malloc(BUFLEN*sizeof(char)); 
            free(message);
            free(response);


        }
    	//comanda data de la tastaura gresita
    	else {
    		printf("INVALID COMMAND\n");
    		continue;
    	}


	}

	free(username);
	free(username1);
	free(password);
	//free(message);
	free(token);
	free(cookie1);
	//free(response);
	free(buff);
	close(sockfd);
    return 0;
}