// Pro_Script1.cpp : Defines the entry point for the console application.
// Lucas's PC 4/25/2018
/*
Update log:
5/9/2018   Fix MulGetData tm_mon+1
5/2/2018   Resampling applied to fix AR issue. Menu and informing notices implemented. FinalVersion1.0
5/1/2018   Multi-thread download applied. Bug: Might stuck or abort sometimes.
4/28/2018  Excel driver issue fixed. Current issue: libcurl extremely slow. AR might be inf for some samples.
4/27/2018  All parts combined. Excel driver cannot be initialized.
*/
#include <iostream>
#include "Boost.h"
#include "Getdata.hpp"
#include "MulGetData.h"

using namespace std;
int main() {
	int choice;
	cout << endl;
	cout << "--------------------------------------------------------------------------" << endl
		<< "|  ______   __    ____   _    __      ___    ____   _                     |" << endl
		<< "|    ||    //\\\\   || \\\\  || ||  \\\\  ||   ||  || \\\\  ||                    |" << endl
		<< "|    ||   //==\\\\  ||  \\\\ || ||   || ||   ||  ||  \\\\ ||                    |" << endl
		<< "|    ||  //    \\\\ ||   \\\\|| ||__//  ||___||  ||   \\\\||                    |" << endl
		<< "|                                                                         |" << endl
		<< "|Financial Computing Final Project                                        |" << endl
		<< "|Author:Florence, Kyle, Li, Lucas, Yicheng                                |" << endl
		<< "|Please enter numbers 1 - 5 to choose different options                   |" << endl
		<< "--------------------------------------------------------------------------" << endl
		<< "1: Retrieve historical price data for all stocks." << endl
		<< "2: Pull information for one stock from one group" << endl
		<< "3: Show AAR or CAAR for one group" << endl
		<< "4: Show the Excel graph with CAAR for all 3 groups" << endl
		<< "5: Exit your program" << endl;


	map<string, Stock> All_Stocks;
	Share SPY;
	Boost Boo;
    double threshold;

	while (1) {
		cout << "Before initializing the program, please enter the threshold for bootstrapping between 5 to 10:";
		cin >> threshold;
		if (threshold < 5 || threshold >10) 
			cout << "Don't you dare messing up with me";
		else 
			break;
	}
    Boo.SetThreshold(threshold);
	cout << "Program initializing completed." << endl;

	while (1) {
		// get choice from user
		while (1) {
			cout << "Tell me what you want, do not do anything before retrieving data" << endl;
			cin >> choice;
			if (cin.fail() || choice <1 || choice >5) // in case input is not integer
			{
				cout << "HEY!!!!YOU ONLY HAVE 5 OPTIONS!!! Don't mess it up with me" << endl;
				cin.clear();
				cin.ignore(INT_MAX, '\n');
				continue;
			}
			else {
				break;
			}
		}
		// use choice
		switch (choice) {
			// download
		case 1: {
			//This is loop download
			
			/*string sCrumb;
			string sCookies;
			AllStock(All_Stocks);
			GetspyAlsoGetCrumb(SPY, sCrumb, sCookies);
			pricehandle(All_Stocks, sCrumb, sCookies);*/
			
			
			//This is multithread
			MulAllStock(All_Stocks); // read sp500 eps from csv
			MulTimeForMagic(All_Stocks, SPY); // download all stock and SPY using libcurl
			break; 
		}
			// get one stock info
		case 2: {
			if (All_Stocks.size() == 0) {
				cout << "I told you do not do anything before retrieving data. Don't mess it up with me" << endl;
				break;
			}
			string temp;
			cout << "Tell me what you want" << endl;
			cin >> temp;
			auto whatyouwant = All_Stocks.find(temp);
			if (whatyouwant != All_Stocks.end())
				whatyouwant->second.Display(threshold);
			else
				cout <<"Seriously? I cannot find it. Bye" << endl;
			break;
		}
			// get AAR/CAAR for group you want
		case 3: {
			if (All_Stocks.size() == 0) {
				cout << "I told you do not do anything before retrieving data. Don't mess it up with me" << endl;
				break;
			}
			int temp;
			cout << "Which group do you want?" << endl;
			cin >> temp; // group 1, 2, 3
			string ar;
			cout << "AAR or CAAR?" << endl;
			cin >> ar;
			Boo.WriteGroups(All_Stocks);
			Boo.Bootstrap(SPY);
			if (ar == "AAR") {
				for (int i = 1; i <= 90; i++)
					cout << Boo.GetAAR_CAAR(long(temp), long(1))[i] << endl;
			}
			else if (ar == "CAAR") {
				for (int i = 1; i <=90; i++)
					cout << Boo.GetAAR_CAAR(long(temp), long(2))[i] << endl;
			}
			else {
				cout << "Huh???? Say that again??" << endl;
			}
			break;
		}

		case 4: {
			if (All_Stocks.size() == 0) {
				cout << "I told you do not do anything before retrieving data. Don't mess it up with me" << endl;
				break;
			}
			if (Boo.GetGroup(1).Gettickers().size() == 0) {
				Boo.WriteGroups(All_Stocks);
				Boo.Bootstrap(SPY);
			}
			list<string> group; // string list
			group.push_back("Beat");
			group.push_back("Meet");
			group.push_back("Miss");

			// caarlist: a list of vectors (3)
			// x is (0-90)-60 = (-60,30)
			Vector<double, long> x(Boo.GetCAARlist().front().MaxIndex());
			for (long i = x.MinIndex(); i <= x.MaxIndex(); i++) {
				x[i] = i - 60;
			}

			// excel driver
			ExcelDriver& excel = ExcelDriver::Instance();
			excel.MakeVisible(true);
			// x-axis, groupname, caar, title, xlabel, ylabel
			printInExcel(x, group, Boo.GetCAARlist(), string("CAAR"), string("Days to Report day"), string("CAAR"));
			break;
		}

		case 5: {
			cout << "Self-destruct initialized. Exploding counting down in process." << endl
				<< "5" << endl
				<< "4" << endl
				<< "3" << endl
				<< "2" << endl
				<< "1" << endl
				<< "I am kidding. Bye" << endl;
			system("pause");
			return 0;
		}
		}
	}
}
