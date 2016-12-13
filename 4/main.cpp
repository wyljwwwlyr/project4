#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <regex>
#include <stdexcept>
#include <iostream>

#define PASS_SIZE 

//#include "dbhandler.cpp"
#include "secure_functions.cpp"
#include "authentication.cpp"
//#include "hashing.cpp"
#include "dbHandler.cpp"
#include "encryption.cpp"


using namespace std;

void drawMenu(string Username) {
	cout << endl << endl;
	system("clear");
	cout << endl;
	cout << "*****************************************" << endl;
	cout << "*                                       *" << endl;
	cout << "*          Welcome to GEE-MAIL!         *" << endl;
	cout << "*                                       *" << endl;
	cout << "*                          by group 14  *" << endl;
	cout << "*****************************************" << endl;

	cout << "You have " << getMessageCount(Username) << " message(s)." << endl;
	cout << endl;
	cout << "What would you like to do? Please select" << endl;
	cout << "(R)ead Messages, (S)end Message, (L)og out" << endl;

}

void sendAMessage(string Username){
    message m;
    m.From = Username;
    cout << "To: ";
    cin >> m.To;
    cin.get();
    string pw;
    bool flag = false;
    m.Salt = newSalt();
    while (!flag){
        pw = getpass("Password: ",true);
        if (pw == getpass("Verify Password: ",true)){
            pw = hashFor(pw, m.Salt, 100000);
            flag = true;
        }
    }
    cout << "Message: ";
    getline(cin, m.Message);
    m.Message = encrypt(pw, m.Salt, m.Message);
    sendMessage(m);
    cout << "Message Sent!" << endl;
    unsigned char action = 0;
    cout << "Press enter to return to main menu" << endl;
    while(action != 10){
        action = getch();
    }
    drawMenu(Username);
}


void readAMessage(string Username){
    vector<string> froms = messagesFrom(Username);
    
    
    cout << "You have messages from: ";
    for (int i = 0; i<froms.size(); i++){
        if (froms[i] != ""){
            cout << froms[i];
            if(i!=froms.size()-1){
                cout << ", ";
            }
        }
    }
    cout << endl << endl;
    cout << "Who do you want to read a message from? " << endl;
    cout << "User: ";
    string user = "";
   
    cin >> user;
    while(!(find(froms.begin(), froms.end(),user)!=froms.end())){
        cout << "Selected user has not sent you a message, please give a valid user: ";
        cin >> user;
        //cin.get();
    }
    char ch; 
    while ((ch = getchar()) != '\n' && ch != EOF);
    message Mess = getRecentMessage(Username, user);
   
    string password = getpass("Password: ",true);
    string hash = hashFor(password, Mess.Salt,100000);
    Mess.Message = decrypt(hash, Mess.Salt, Mess.Message);
    cout << "Message: " << Mess.Message << endl;
    unsigned char action = 0;
    cout << "Press enter to return to main menu" << endl;
    while(action != 10){
        action = getch();
    }
    drawMenu(Username);
    
 
}

bool isValidAction(char a){
    switch(a){
        case 'r': return true;
        case 'R': return true;
        case 's': return true;
        case 'S': return true;
        case 'l': return true;
        case 'L': return true;
        default: return false;
    }
}


int main(int argc, char **argv){
    if (argc == 1){
        try{
            bool loggedin = false;
            string Option = "";
            string Username = "";
            string password = "";
            string PasswordHash = "";
            string salt = "";
          
            while(!loggedin){
                cout << "Username: ";
                cin >> Username;
                cin.get();
              
                if(userExists(Username)){
                    password=getpass("Password: ",true);
                    salt = getSalt(Username);
                    PasswordHash = hashFor(password, salt, 100000);
                    if(getPassword(Username) == PasswordHash){
                        loggedin = true;
                    } else {
                        cout << "Invalid Password" << endl;
                    }
                } else {
                    cout << "User doesn't exist, Creating account..." << endl;
                    password=getpass("New Password: ",true);
                    if(password == getpass("Verify Password: ",true)){
                        cout << "Creating user..." << endl;
                        salt=newSalt();
                        createUser(Username,hashFor(password, salt, 100000),salt);
                        cout << "User Created Successfully, Please login" << endl;
                    } else {
                        cout << "Passwords didn't match, please try again" << endl;
                    }
                }
            }
            
            bool done = false;
            unsigned char action = 0;
            drawMenu(Username);
            while(!done){
               
                unsigned char action = 0;
                while(!isValidAction(action)){
                  
                    action = getch();
                  
                }
                drawMenu(Username);
                if(action=='R' || action=='r'){
                    if(getMessageCount(Username)>0){
                        readAMessage(Username);
                    } else {
                        cout << "Why are you trying to read messages you don't have?" << endl;
                    }
                }
                else if(action=='S'||action=='s'){
                    sendAMessage(Username);
                }
                else if(action=='L'||action=='l'){
                    done=true;
                }
                else {
                    throw runtime_error("Somehow, You won... you beat logic itself");
                }
            }
            cout << "Good bye!" << endl;
            system("clear");            
           
        } catch (invalid_argument const &e){
            cout << "Argument Error: " << e.what() << endl;
            return 1;
        } catch (bad_alloc const &e){
            cout << "Memory Error: " << e.what() << endl;
            return 2;
        } catch (runtime_error const &e){
            cout << "Runtime Error: " << e.what() << endl;
            return 3;
        } catch (exception const &e){
            cout << "Other Error: " << e.what() << endl;
            return 4;
        } catch (...){
            cout << "Unknown Error has Occured" << endl;
            return 5;
        }
    }
    return 0;
}

