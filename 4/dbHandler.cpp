#include <stdio.h>
#include <sqlite3.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <exception>
#include <stdexcept>  

#define UsersTable "users"
#define MessagesTable "messages"

using namespace std;

class message{
    public:
    std::string To;
    std::string From;
    std::string Message;
    std::string Salt;
};
struct A{
    string b;
    int c;
    string salt;
};

struct B{
    vector<string> s;
};

string sanitize(string s){
    string answer;
    for (int i = 0; i < s.length(); i++){
        answer += s.at(i);
        if (s.at(i) == '\''){
            answer += s.at(i);
        }
    }
    return answer;
}  

//callback for inserts
static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   return 0;
}

//callback for a recent message
static int callbackRM(void *data, int argc, char **argv, char **azColName){
    int i;
    A *tableData = (A *)data ;
    tableData->b = argv[argc - 1];
    return 0;
}

//callback for a recent message
static int callbackRMSalt(void *data, int argc, char **argv, char **azColName){
    int i;
    A *tableData = (A *)data ;
    tableData->salt = argv[argc - 1];
    return 0;
}

//callback to return string val
static int callbackString(void *data, int argc, char **argv, char **azColName){
    int i;
    A *tableData = (A *)data ;
    for(i=0; i<argc; i++){
        tableData->b = argv[i];
    }
    return 0;
}

//callback to return a vector
static int callbackVector(void *data, int argc, char **argv, char **azColName){
    int i;
    B *vData = (B *)data ;
    for(i=0; i<argc; i++){
        vData->s.push_back(argv[i]);
    }
    return 0;
}

//callback to return an int
static int callbackInt(void *data, int argc, char **argv, char **azColName){
    int i;
    A *numberHolder = (A *)data ;
    numberHolder->c = argc;
    return 0;
}

//callback for a select count(*)...
static int callbackCount(void *data, int argc, char **argv, char **azColName){
    int i;
    A *numberHolder = (A *)data ;
    numberHolder->c = stoi(argv[0]);
    return 0;
}

//call back when checking for the existence of the db/tables
static int callbackTE(void *data, int argc, char **argv, char **azColName){
    int i;
    A *numberHolder = (A *)data ;
    int j = stoi(argv[0]);
    numberHolder->c = j;
    return 0;
}

//check if the table/db already exist and if not, create them
void tablesExist(void){
    sqlite3* db;
    char *zErrMsg = 0;
    int rc;
    A numberHolder;
    rc = sqlite3_open("geemail.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        throw runtime_error("Can't Open Database");
    };
    //check for users table
    string sql = "select count(*) from sqlite_master where type='table' and name='" + string(UsersTable) +"';";
    rc = sqlite3_exec(db, sql.c_str(), callbackTE, &numberHolder, &zErrMsg);
    if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      throw runtime_error("Error in checking for user table");
    }
    //if the table did not exist create it
    if (numberHolder.c == 0){
        sql = "create table users( ID integer primary key autoincrement, UserName varchar, ";
        sql += "Password varchar, Salt varchar);";
        rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      throw runtime_error("Error in creating user table");
    }
    }
    //check for the existence of the messages table
    sql = "select count(*) from sqlite_master where type='table' and name='" + string(MessagesTable) +"';";
    rc = sqlite3_exec(db, sql.c_str(), callbackTE, &numberHolder, &zErrMsg);
    if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      throw runtime_error("Error in checking for messages table");
    }
    //if it did not exist, create it
    if (numberHolder.c == 0){
        sql = "create table messages( MID integer primary key autoincrement, FRM varchar, ";
        sql += "TOM varchar, Message text, Salt varchar);";
        rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      throw runtime_error("Error in creating messages table");
    }
    }
    sqlite3_close(db);
}

//check if a user exists in the db
bool userExists(string UserName){
    tablesExist();
    sqlite3* db;
    char *zErrMsg = 0;
    int rc;
    A numberHolder;
    rc = sqlite3_open("geemail.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        throw runtime_error("Can't Open Database");
    };
    UserName = sanitize(UserName);
    string sql = "select count(*) from " + string(UsersTable) +" where Username='" + UserName + "';";
    rc = sqlite3_exec(db, sql.c_str(), callbackCount, &numberHolder, &zErrMsg);
    if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      throw runtime_error("Error in checking if a user exists");
    }
    sqlite3_close(db);
    //means user did not exist
    if (numberHolder.c == 0){
        return false;
    }
    //means user did exist
    else return true;
}

//creates a user
void createUser(string Username, string Password, string Salt){
    tablesExist();
    sqlite3* db;
    char *zErrMsg = 0;
    int rc;
    string data = "Callback function called";
    rc = sqlite3_open("geemail.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        throw runtime_error("Can't Open Database");
    };
    Username = sanitize(Username);
    string sql = "insert into " + string(UsersTable) + " ('USERNAME', 'PASSWORD', 'SALT') values ('" + Username 
    + "', '" + Password + "', '" 
    + Salt + "');";
    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      throw runtime_error("Error creating new user");
    }
    sqlite3_close(db);
}

//gets a vector of all the users in the db
vector<string> getUsers(void){
    tablesExist();
    sqlite3* db;
    char *zErrMsg = 0;
    int rc;
    B vData;
    rc = sqlite3_open("geemail.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        throw runtime_error("Can't Open Database");
    };
    string sql = "select username from " + string(UsersTable) + ";";
    rc = sqlite3_exec(db, sql.c_str(), callbackVector, &vData, &zErrMsg);
    if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      throw runtime_error("Error getting usernames");
    }
    sqlite3_close(db);
    return vData.s;
}

//gets the hashed pw of a username
string getPassword(string Username){
    tablesExist();
    sqlite3* db;
    char *zErrMsg = 0;
    int rc;
    A tableData;
    rc = sqlite3_open("geemail.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        throw runtime_error("Can't Open Database");
    };
    Username = sanitize(Username);
    string sql = "select password from " + string(UsersTable) +" where username='" + Username + "';";
    rc = sqlite3_exec(db, sql.c_str(), callbackString, &tableData, &zErrMsg);
    if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      throw runtime_error("error in getting the password");
    }
    sqlite3_close(db);
    return tableData.b;
}

//gets the salt of a username
string getSalt(string Username){
    tablesExist();
    sqlite3* db;
    char *zErrMsg = 0;
    int rc;
    A tableData;
    rc = sqlite3_open("geemail.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        throw runtime_error("Can't Open Database");
    };
    Username = sanitize(Username);
    string sql = "select salt from " + string(UsersTable) + " where username='" + Username + "';";
    rc = sqlite3_exec(db, sql.c_str(), callbackString, &tableData, &zErrMsg);
    if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      throw runtime_error("Error in getting the salt");
    }
    sqlite3_close(db);
    return tableData.b;
}

//gets the number of messages in a user's inbox
int getMessageCount(string UserName){
    tablesExist();
    sqlite3* db;
    char *zErrMsg = 0;
    int rc;
    A numberHolder;
    rc = sqlite3_open("geemail.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        throw runtime_error("Can't Open Database");
    };
    UserName = sanitize(UserName);
    string sql = "select count(*) from " + string(MessagesTable) + " where tom='" + UserName + "';";
    rc = sqlite3_exec(db, sql.c_str(), callbackCount, &numberHolder, &zErrMsg);
    if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      throw runtime_error("Error in checking the messages count");
    }
    sqlite3_close(db);
    return numberHolder.c;
}

//sends a message (inserts it into messages table)
void sendMessage(message message){
    tablesExist();
    sqlite3* db;
    char *zErrMsg = 0;
    int rc;
    string data = "Callback function called";
    rc = sqlite3_open("geemail.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        throw runtime_error("Can't Open Database");
    };
    message.To = sanitize(message.To);
    message.From = sanitize(message.From);
    string sql = "insert into " + string(MessagesTable) + "(TOM, FRM, Message, Salt) values ('" + message.To;
    sql += "', '" + message.From + "', '" + message.Message + "', '" + message.Salt + "');";
    rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      throw runtime_error("Error in sending a message");
    }
    sqlite3_close(db);
}

//returns a vector of users who have sent the given username a message
vector<string> messagesFrom(string myusername){
    tablesExist();
    sqlite3* db;
    char *zErrMsg = 0;
    int rc;
    B vData;
    rc = sqlite3_open("geemail.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        throw runtime_error("Can't Open Database");
    };
    myusername = sanitize(myusername);
    string sql = "select Frm from " + string(MessagesTable) + " where ";
    sql += "Tom='" + myusername + "';";
    rc = sqlite3_exec(db, sql.c_str(), callbackVector, &vData, &zErrMsg);
    if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      throw runtime_error("Error in retreiving the user's inbox");
    }
    sqlite3_close(db);
    sort( vData.s.begin(), vData.s.end() );
    vData.s.erase( unique( vData.s.begin(), vData.s.end() ), vData.s.end() );
    return vData.s;
}

//gets the most recent message from a given user to your username
message getRecentMessage(string myusername, string from){
    tablesExist();
    sqlite3* db;
    char *zErrMsg = 0;
    int rc;
    A tableData;
    rc = sqlite3_open("geemail.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        throw runtime_error("Can't Open Database");
    };
    myusername = sanitize(myusername);
    from = sanitize(from);
    string sql = "select message from " + string(MessagesTable) + " where Tom='" + myusername;
    sql += "' AND Frm='" + from + "';";
    rc = sqlite3_exec(db, sql.c_str(), callbackRM, &tableData, &zErrMsg);
    if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      throw runtime_error("Error getting the most recent message from the given user");
    }
    string resultMes = tableData.b;
    sql = "select salt from " + string(MessagesTable) + " where Tom='" + myusername;
    sql += "' AND Frm='" + from + "';";
    rc = sqlite3_exec(db, sql.c_str(), callbackRMSalt, &tableData, &zErrMsg);
    if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      throw runtime_error("Error getting the salt from the given user");
    }
    string resultSalt = tableData.salt;
    message finalMessage;
    finalMessage.To = myusername;
    finalMessage.From = from;
    finalMessage.Message = resultMes;
    finalMessage.Salt = resultSalt;
    sqlite3_close(db);
    return finalMessage;
}
/*
int main(){
    cout << "message count " << getMessageCount("trevor") << endl;
    return 0;
}
*/