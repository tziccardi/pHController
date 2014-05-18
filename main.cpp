//
//  main.cpp
//  phController
//
//  Created by Thomas Ziccardi on 5/3/14.
//  Copyright (c) 2014 Thomas Ziccardi. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <ctime> 
#include <stdlib.h>
#include <unistd.h>

#include <wiringPi.h>

using namespace std;

const char wday_name[] [4]= {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

const int pin = 0;

//struct setup

struct databaseInfo {
    
    string dayOfWeek;
    double amount;
    int status;
    
};



void resetCursor(fstream &file);


void acidPump(double amount, int pin);



int main(int argc, const char * argv[])
{
    
    
    
    

    fstream dbFile;
    fstream logFile("pHCtr.log", ios::out | ios::app);
    
    databaseInfo acidCtr;
    
    string day, col1, col2;
    
    int wP = 0;
    
    
    // wiringPi setup
    wiringPiSetup();
    
    pinMode(pin, OUTPUT);
    
    // for time
    time_t rawtime;
    struct tm * timeinfo;


    if( dbFile.fail()) {
        
        // the file doesnt exist -> create it and populate the database
        
        dbFile.open("pHCtr.db", ios::out);
        
        // table heading:  DAY | TIME | ON/OFF
        
        for (int i = 0; i < 7; i++) {
            
            dbFile << wday_name[i] <<"$" << "0.0" << "$" << "0" << "$\n";
    
        }
        
        cout << "The initial values are stored\n";
        dbFile.close();
    }
    else {
       
        dbFile.close() ;
        cout << "The Database Exists\n";
    }
    
    // get current day
    
    
    time ( &rawtime );
    
    timeinfo = localtime ( &rawtime );
    
    //printf ( "The current date/time is: %s", asctime (timeinfo) );
    
    
    // store the day to struct
	acidCtr.dayOfWeek = wday_name[timeinfo->tm_wday];
    
    cout << "The Current Day is: " << acidCtr.dayOfWeek << endl;
    
    // open database and populate struct with data based on current day
    
    dbFile.open("pHCtr.db", ios::in);
    
    resetCursor(dbFile);
    
    // load values into struct
    
    while(!dbFile.eof()) {
        
        
		getline(dbFile, day, '$');
        
		if (day == acidCtr.dayOfWeek) {
            
			getline(dbFile, col1, '$');
			getline(dbFile, col2, '$');
            
			break;
		}
        
		dbFile.ignore(256, '\n');
        
        
	}
    
    dbFile.close();
    
    // convert strings to double and int
    acidCtr.amount = atof(col1.c_str());
    acidCtr.status = atof(col2.c_str());
    
    cout << "The Values for today are: Amount: " << acidCtr.amount <<" Status: " << acidCtr.status << endl;
    
    // write log file
    //open log
    logFile << asctime (timeinfo); 
    logFile << "amount:" << acidCtr.amount << " Status:" << acidCtr.status << endl; 
    logFile.close(); 
    cout << "Log Written\n"; 
    
    if (acidCtr.status == 0) {
        
        cout << "Pump off for Today, exiting.....\n";
        
        return 1;
        
    }
    
    
    acidPump(acidCtr.amount, pin);
    
    cout << "Acid Pumped\n";

    return 0;

}


void resetCursor(fstream &dataFile) {
    
    dataFile.clear();
    dataFile.seekg(0, ios::beg);
}



void acidPump(double amount, int pin) {
    
    
     digitalWrite(pin, 0);
     sleep(amount);
     digitalWrite(pin, 1);
     
    
} 



