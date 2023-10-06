#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "spider.h"
/* I would like to acknowledge this many not be the most safe or efficient approach */

char *generate_url(char *url){
    char *result = malloc(strlen(url) + strlen("https://.com"));
    memset(result, '\0', strlen(url) + strlen("https://.com"));
    strcat(result, "https://");
    strcat(result, url);
    strcat(result, ".com");
    return result;
}

/* Somewhat patchworky 
 * Planning to eliminate redundancies
 */
void check_valid_url(const char *str, CURL *curl, char *name){
    CURLcode res;

    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // handel 301...
    curl_easy_setopt(curl, CURLOPT_URL, str);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 4L); // skip hanging URL after 4 seconds

    char *filepath = malloc(strlen(name) + strlen("archive/.html"));
    memset(filepath, '\0', strlen(name) + strlen("archive/.html"));
    strcat(filepath, "archive/");
    strcat(filepath, name);
    strcat(filepath, ".html");

    FILE *fptr = fopen(filepath, "w");
    if(!fptr)
        return;

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fptr);

    res = curl_easy_perform(curl);
    fclose(fptr);

    if (res != CURLE_OK){
        /* Not great for a SSD to do consecutive writes
         * Although SSDs are not partically meant for this..
         * Also reduncant
         */
        remove(filepath);
        return;
    }
    printf("%s: URL is indexable.\n", str);

    free(filepath);
}

void backtrack(char *alphabet, char *prev_str, int index, list *result, int n, CURL *curl){
    if(strlen(prev_str) == n){
        // memory management shenanigans
        list *ret = malloc(sizeof(list));
        ret->data = malloc(strlen(prev_str) + 1);  // +1 for null-terminator
        strcpy(ret->data, prev_str);
        ret->next = NULL;

        append_list(result, ret);
        char *urlptr = generate_url(ret->data);
        check_valid_url(urlptr, curl, ret->data);
        free(urlptr);

        return;
    }

    // memory management shenanigans and string manipulation
    char *buff = malloc(n + 1);  // +1 for null-terminator
    strcpy(buff, prev_str);

    for(int i = index; i < strlen(alphabet); i++){
        buff[strlen(prev_str)] = alphabet[i];
        buff[strlen(prev_str) + 1] = '\0'; // null-terminator
        backtrack(alphabet, buff, i, result, n, curl);
    }

    free(buff);
}

int main(){
    CURL *curl;

    curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Failed to initialize libcurl.\n");
        return 1;
    }

    // Index all pages with a domain of 4-5 in length
    for(int i = 4; i <= 5; i++) {
        list *head = malloc(sizeof(list));
        head->next = NULL;

        backtrack("abcdefghijklmnopqrstuvwxyz", "", 0, head, i, curl);

        for(list *freelptr = head->next; freelptr; freelptr = freelptr->next){
            free(freelptr->data);
            free(freelptr);
        }
        free(head);
    }

    curl_easy_cleanup(curl);

    return 0;
}
