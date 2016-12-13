#include <stdio.h>
#include <gcrypt.h>
#include <iostream>
#include <stdlib.h>
#include <time.h> 
using namespace std;

string hashIt(string input){
    char hash[65] = "";
    size_t index;
    size_t txtLength = input.length();
    char * hashBuffer = (char*) malloc(33);
    char * textBuffer = (char*) malloc(txtLength+1);
    memset(hashBuffer, 0, 33);

    strncpy(textBuffer, input.c_str(), txtLength);

    gcry_md_hash_buffer(
        GCRY_MD_SHA256, // gcry_cipher_hd_t
        hashBuffer,    // void *
        textBuffer,    // const void *
        txtLength);   // size_t

    //printf("hashBuffer = ");
    for (index = 0; index<32; index++)
        sprintf(hash+index*2, "%02X", (unsigned char)hashBuffer[index]);
    //printf("\n");
    return (string) hash;
}

string newSalt(void){
    srand(time(NULL));
    string theString;
    for (int i = 0; i < 8; i++){
      int k = rand() % 94 + 32;
      char j = static_cast<char>(k);
      theString += j;
    }
    return theString;
}

string hashFor(string password, string salt, int rounds){
    string hash = "0";
    for(int i=0; i < rounds; i++){
        hash = hashIt(hash+password+salt);
        //cout << "round: " << i << " hash: " << hash << endl;
    }
    return hash;
}
