#include <stdio.h>
#include <stdlib.h>
#define PACKET_SIZE 1000

typedef struct packet {
    unsigned int total_frag;
    unsigned int frag_no;
    unsigned int size;
    char* filename;
    char filedata[PACKET_SIZE];
} packet;

/*
    input:
        arr: a pointer to a character array representation of a packet
            e.g., "3:2:10:foobar.txt:lo World!\n"
        
        len: length of the input char array

    return:
        a pointer to the packet that results of decoding the character array,
            or null, if the input is invalid
*/
packet* decode_char_array(char* arr, int len) {
    packet* p = (packet *) malloc (sizeof(packet));
    int delimiter_indexes[4] = {0, 0, 0, 0};
    int current_delimiter_count = 0;

    for (int i = 0; i < len; i++) {
        if (current_delimiter_count > 4) {
            return NULL;
        }
        if (arr[i] == ':') {
            delimiter_indexes[current_delimiter_count] = i;
            current_delimiter_count += 1;
        }
    }

    for (int j = 0; j < 4; j++) {
        printf("%d\n", delimiter_indexes[j]);
    }

    p -> total_frag = atoi(arr);
    p -> frag_no = atoi(arr + sizeof(char) * delimiter_indexes[0] + sizeof(char));
    p -> size = atoi(arr + sizeof(char) * delimiter_indexes[1] + sizeof(char));
    // TODO: Use delimiter_indexes[2] and delimiter_indexes[3] to figure out file name length
    // TODO: then use mempy to obtain the filename
    
    return p;
}