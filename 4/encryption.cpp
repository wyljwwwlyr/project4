#include <stdio.h>
#include <gcrypt.h>
#include <iostream>
#include <bitset>
#include <string.h>
#include <cstdint>

#include "hashing.cpp"

using namespace std;

int charToInt(char letter)
{
    int myNumerical;
    // First we want to check if its 0-9, A-F, or a-f) --> See ASCII Table
    if(letter > 47 && letter < 58)
    {
        // 0-9
        myNumerical = letter-48;
        // The Letter "0" is in the ASCII table at position 48 -> meaning if we subtract 48 we get 0 and so on...
    }
    else if(letter > 64 && letter < 71)
    {
       // A-F
       myNumerical = letter-55; 
       // The Letter "A" (dec 10) is at Pos 65 --> 65-55 = 10 and so on..
    }
    else if(letter > 96 && letter < 103)
    {
       // a-f
       myNumerical = letter-87;
       // The Letter "a" (dec 10) is at Pos 97--> 97-87 = 10 and so on...
    }
    else
    {
       // Not supported letter...
       myNumerical = -1;
    }
    return myNumerical;
}

int appendNumbers(int higherNibble, int lowerNibble)
{
     int myNumber = higherNibble << 4;
     myNumber |= lowerNibble;
     return myNumber;
    // Example: higherNibble = 0x0A, lowerNibble = 0x03;  -> myNumber 0 0xA3
    // Of course you have to ensure that the parameters are not bigger than 0x0F 
}

unsigned char* create_stream (string hash, string nonce, int messageLength) {
    gcry_error_t     gcryError;
    gcry_cipher_hd_t gcryCipherHd;
    size_t           index;
    
    const char * salsaKey = hash.c_str(); // 32 bytes
    const char * iniVector = nonce.c_str(); // 8 bytes

    gcryError = gcry_cipher_open(
        &gcryCipherHd, // gcry_cipher_hd_t *
        GCRY_CIPHER_SALSA20,   // int
        GCRY_CIPHER_MODE_STREAM,   // int
        0);            // unsigned int
    if (gcryError)
    {
        // printf("gcry_cipher_open failed:  %s/%s\n",
        //       gcry_strsource(gcryError),
        //       gcry_strerror(gcryError));
        throw runtime_error("Internal Cipher Error: Cipher open failed/" + string(gcry_strsource(gcryError)) + "/" + string(gcry_strerror(gcryError)));
    }
    //printf("gcry_cipher_open worked\n");
    
    gcryError = gcry_cipher_setkey(gcryCipherHd, salsaKey, 32);
    if (gcryError)
    {
        // printf("gcry_cipher_setkey failed:  %s/%s\n",
        //       gcry_strsource(gcryError),
        //       gcry_strerror(gcryError));
        throw runtime_error("Internal Cipher Error: setkey failed/" + string(gcry_strsource(gcryError)) + "/" + string(gcry_strerror(gcryError)));
        //return 0;
    }
    //printf("gcry_cipher_setkey worked\n");
    
    gcryError = gcry_cipher_setiv(gcryCipherHd, iniVector, 8);
    if (gcryError)
    {
        // printf("gcry_cipher_setiv failed:  %s/%s\n",
        //       gcry_strsource(gcryError),
        //       gcry_strerror(gcryError));
        throw runtime_error("Internal Cipher Error: setiv failed/" + string(gcry_strsource(gcryError)) + "/" + string(gcry_strerror(gcryError)));
        //return 0;
    }
    //printf("gcry_cipher_setiv worked\n");
    
    size_t txtLength = messageLength;
    char * encBuffer = (char*)malloc(txtLength);
    char * textBuffer = (char*)malloc(txtLength);
    memset(textBuffer, 0, txtLength);

    gcryError = gcry_cipher_encrypt(
        gcryCipherHd, // gcry_cipher_hd_t
        encBuffer,    // void *
        txtLength,    // size_t
        textBuffer,    // const void *
        
        txtLength);   // size_t
    if (gcryError)
    {
        // printf("gcry_cipher_decrypt failed:  %s/%s\n",
        //       gcry_strsource(gcryError),
        //       gcry_strerror(gcryError));
        throw runtime_error("Internal Cipher Error: decrypt failed/" + string(gcry_strsource(gcryError)) + "/" + string(gcry_strerror(gcryError)));
        //return 0;
    }
    //printf("gcry_cipher_decrypt worked\n");
    
    //printf("encBuffer = ");
    char stream[txtLength * 2];
    int currIndex = 0;
    for (index = 0; index<txtLength-1; index++){
        sprintf(stream+index*2, "%02X", (unsigned char)encBuffer[index]);
    }
        //sprintf("%02X", (unsigned char)encBuffer[index]);
        
    //cout << (string) stream << endl;
    return (unsigned char*)encBuffer;
    //return string(stream);
}

string encrypt (string hash, string nonce, string message) {
    unsigned char* keystream = create_stream(hash, nonce, message.length());
    const char* messageBytes = message.c_str();
    unsigned char* outputBuffer = (unsigned char*)malloc(sizeof(unsigned char)*message.length());
    for(int i=0; i < message.length(); i++){
        //cout << "messageBytes[i] " << messageBytes[i] << endl;
        //cout << "keystream[i] " << keystream[i] << endl;
        outputBuffer[i]=messageBytes[i]^keystream[i];
        //cout << "outputBuffer[i] " << outputBuffer[i] << endl;
    }
    char stream[message.length() * 2];
    for (int index = 0; index<message.length(); index++){
        sprintf(stream+index*2, "%02X", (unsigned char)outputBuffer[index]);
    }
    return (string) stream;
    
}

string decrypt (string hash, string nonce, string encryption) {
    unsigned char* keystream = create_stream(hash, nonce, encryption.length()/2);
    const char* encryptedBytes = encryption.c_str();
    //unsigned char* preXOR = (unsigned char*)malloc(sizeof(string)*encryption.length()/2);
    // const char* source_val = encryption.c_str();// Represents the numbers 0x0A, 0x03 and 0xB7
    // unsigned int dest_val[encryption.length()/2];                            // We want to save 3 numbers
    // for(int i = 0; i<encryption.length(); i++)
    // {
    //  sscanf(&source_val[i*2],"%x%x",&dest_val[i]); // Everytime we read two chars --> %x%x
    // }
    // for (int k = 0; k < encryption.length() /2; k++){
    //     char enc = encryptedBytes[k] + encryptedBytes[k+1];
    //     preXOR[k] = (enc >= 'A')? (enc - 'A' + 10): (enc - '0');
    // }
    const char* source_val = encryption.c_str(); // Represents the numbers 0x0A, 0x03 and 0xB7
    int dest_val[encryption.length()/2];                             // We want to save 3 numbers
    int temp_low, temp_high;
    for(int i = 0; i<encryption.length()/2; i++)
    {
        temp_high = charToInt(source_val[i*2]);
        temp_low = charToInt(source_val[i*2+1]);
        dest_val[i] = appendNumbers(temp_high , temp_low);
    }
    
    char* outputBuffer = (char*)malloc(sizeof(char)*(encryption.length()/2));
    for(int i=0; i < encryption.length()/2; i++){
        //cout << "encryptedBytes[i] " << encryptedBytes[i] << endl;
        //cout << "keystream[i] " << keystream[i] << endl;
        outputBuffer[i]=dest_val[i]^keystream[i];
        //cout << "outputBuffer[i] " << outputBuffer[i] << endl;
    }
    // char stream[encryption.length()];
    // for (int index = 0; index<encryption.length()/2; index++){
    //     sprintf(stream+index*2, "%02X", (unsigned char)outputBuffer[index]);
    // }
    return (string) outputBuffer;
}




/*
int main(){
    string a = "hello this is a super secret message"; //message
    string b = "hellohel"; // salt/nonce
    string c = "12345678901234567890123456789012"; //encryption key
    string encrypted = encrypt(c,b,a);
    cout << "encrypted text: " << encrypted << endl;
    cout << "decrypted text: " << decrypt(c,b,encrypted) << endl;
    //cout << encrypt()
    return 0;
}
