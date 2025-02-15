/*
 * File: yahtzee_v7.cpp
 * Author: Thomas Kleist
 * Created on Feb 7, 2025, 10:00 AM
 * Purpose: To implement the upper section of YAHTZEE
 * Goal for V7: Output/Input average score, standard deviation
 */

//System Libraries
#include <iostream>  //I/O Library
#include <cstdlib>   //Random Function Library
#include <ctime>     //Time Library
#include <iomanip>   //Formatting Library
#include <fstream>   //File IO Library
#include <cmath>     //Math library (need sqrt for standard deviation)
#include <string>    //string library
using namespace std;

//User Libraries

//Global Constants - No Global Variables
//Only Universal Constants, Math, Physics, Conversions, Higher Dimensions

// Function Prototypes
void roll(int [], int);             // Role dice with no hold
void roll(int [], char[], int);     // Role dice with hold
void dispDce(int [], int);          // Display dice
void getHold(char[], int);          // Ask the player if they would like to hold each dice
int turnScore(int [], int, int);    // Get the score for the turn
bool gameDone(bool [], int);        // Get if the game is completed based on the row completion
void displayScore(int [], bool [], int);     // Display the current scores
int selScore(bool [], int);         // Select which row to score
void displayTurn();                 // Display the turn number

int main(int argc, char** argv) {
    // Set Random Number seed
    srand(static_cast<unsigned int>(time(0)));

    const int bnsCut = 63; // The minimum score the player must earn to get the bonus
    const int bnsScr = 35; // The bonus score the player earns if they meet the cutoff
    const string fileNme = "avgData.dat";
    const int numDice = 5;
    const int numUppR = 6;

    //Declare Variables
    int dice[numDice]; // The value of the dice
    char hold[numDice]; // Wether or not the user wants to hold each dice. 'Y' or 'y' for yes, anything else for no
    int numSel; // The number the user selects to score this turn
    int turnScr; // The score for this turn
    int turnCnt; // The counter for how many turns the player had done
    int uppScr[numUppR]; // The score for each of numbers on a die
    bool uppDone[numUppR]; // Bool to store if a certain number has been scored
    int uppSum, bonus, uppTtl; // The sum of all the scores, the bonus points, and the total score for the game

    int count; // The count that will be read from the average data file
    int ttl1, ttl2, ttl3, ttl4, ttl5, ttl6, ttlSum, ttlBns, ttlTtl; // The total that is read from the average data file
    float avg1, avg2, avg3, avg4, avg5, avg6, avgSum, avgBns, avgTtl; // The average of each score that is calculated 
    float M2; // A helper variable stored in the average data file to calculate the standard deviation of the average total
    float stdDev; // The calculated standard deviation of the total score

    //Initialize Variables
    for (int i = 0; i < numUppR; i++) {
        uppScr[i] = 0;
        uppDone[i] = false;
    }
    turnCnt = 1; // Set it to the first turn

    // Run the game
    // Run the loop until all numbers have been scored
    while (!gameDone(uppDone, numUppR)) {
        // Display turn count and current scores
        displayTurn();
        displayScore(uppScr, uppDone, numUppR);

        // Loop 3 times because each turn in yahtzee allows three roles
        for (int i = 0; i < 3; i++) {
            cout << "Roll " << i+1 << endl;
            
            if (i == 0) roll(dice, numDice);
            else roll(dice, hold, numDice);

            // Display the value of each of the dice
            dispDce(dice, numDice);

            // Don't ask about holding on the third role (there are no more rolls so holding wouldn't do anything)
            if (i != 2) { 
                getHold(hold, numDice);
            }
            cout << endl;
        }

        // After rolling three times, select which number the user would like to score in
        // If the number selected is out of range, ask them to input again
        // If the number they selected has already been scored, ask them to input again
        numSel = selScore(uppDone, numUppR);
        turnScr = turnScore(dice, numDice, numSel);

        // Output the number of points earned for the user to see
        // Save the number of points in the variable for the row
        // Set the bool of that row to true to indicate that the row has been scored
        switch (numSel)
        {
        case 1:
            cout << "The number of points earned in the ACE row is " << turnScr << endl << endl << endl;
            uppScr[0] = turnScr;
            uppDone[0] = true;
            break;
        case 2:
            cout << "The number of points earned in the TWOS row is " << turnScr << endl << endl << endl;
            uppScr[1] = turnScr;
            uppDone[1] = true;
            break;
        case 3:
            cout << "The number of points earned in the THREES row is " << turnScr << endl << endl << endl;
            uppScr[2] = turnScr;
            uppDone[2] = true;
            break;
        case 4:
            cout << "The number of points earned in the FOURS row is " << turnScr << endl << endl << endl;
            uppScr[3] = turnScr;
            uppDone[3] = true;
            break;
        case 5:
            cout << "The number of points earned in the FIVES row is " << turnScr << endl << endl << endl;
            uppScr[4] = turnScr;
            uppDone[4] = true;
            break;
        case 6:
            cout << "The number of points earned in the SIXES row is " << turnScr << endl << endl << endl;
            uppScr[5] = turnScr;
            uppDone[5] = true;
            break;
        default:
            break;
        }
    }
    // After all the scores have been filled in, compute the score and output a nicely formatted score card. 
    uppSum = 0;
    for (int i = 0; i < numUppR; i++) {
        uppSum += uppScr[i];
    }
    if (uppSum >= bnsCut) bonus = 35; else bonus = 0;
    uppTtl = uppSum + bonus;

    // Display the score for the game
    cout << "Your score for this game:" << endl;
    cout << "ACES:   " << setw(3) << uppScr[0] << endl;
    cout << "TWOS:   " << setw(3) << uppScr[1] << endl;
    cout << "THREES: " << setw(3) << uppScr[2] << endl;
    cout << "FOURS:  " << setw(3) << uppScr[3] << endl;
    cout << "FIVES:  " << setw(3) << uppScr[4] << endl;
    cout << "SIXES:  " << setw(3) << uppScr[5] << endl;
    cout << "-----------" << endl;
    cout << "SCORE:  " << setw(3) << uppSum << endl;
    cout << "BONUS:  " << setw(3) << bonus << endl;
    cout << "TOTAL:  " << setw(3) << uppTtl << endl << endl << endl;

    // Open the average data file, then read the count, totals, and M2
    ifstream avgIn(fileNme);
    avgIn >> count >> ttl1 >> ttl2 >> ttl3 >> ttl4 >> ttl5 >> ttl6 >> ttlSum >> ttlBns >> ttlTtl >> M2;
    avgIn.close();

    // Update the count and totals
    count++;
    ttl1 += uppScr[0];
    ttl2 += uppScr[1];
    ttl3 += uppScr[2];
    ttl4 += uppScr[3];
    ttl5 += uppScr[4];
    ttl6 += uppScr[5];
    ttlSum += uppSum;
    ttlBns += bonus;
    ttlTtl += uppTtl;

    // Calculate the average of all the scores 
    avg1 = (float)ttl1/float(count);
    avg2 = (float)ttl2/float(count);
    avg3 = (float)ttl3/float(count);
    avg4 = (float)ttl4/float(count);
    avg5 = (float)ttl5/float(count);
    avg6 = (float)ttl6/float(count);
    avgSum = (float)ttlSum/float(count);
    avgBns = (float)ttlBns/float(count);
    avgTtl = (float)ttlTtl/float(count);
    // Calculate the standard deviation of the total score
    M2 += pow(uppTtl - avgTtl, 2);
    stdDev = (count > 1) ? sqrt(M2/(count-1)) : 0;

    // Store the updated count and totals into the file
    ofstream avgOut(fileNme);
    avgOut << count << " " << ttl1 << " " << ttl2 << " " << ttl3 << " " << ttl4 << " " << ttl5 << " " << ttl6 << " " << ttlSum << " " << ttlBns << " " << ttlTtl << " " << M2;
    avgOut.close();

    // Output the average score in the same formatting as the users score
    cout << "The average score of games played:" << endl;
    cout << "ACES:   " << setw(3) << avg1 << endl;
    cout << "TWOS:   " << setw(3) << avg2 << endl;
    cout << "THREES: " << setw(3) << avg3 << endl;
    cout << "FOURS:  " << setw(3) << avg4 << endl;
    cout << "FIVES:  " << setw(3) << avg5 << endl;
    cout << "SIXES:  " << setw(3) << avg6 << endl;
    cout << "-----------" << endl;
    cout << "SCORE:  " << setw(3) << avgSum << endl;
    cout << "BONUS:  " << setw(3) << avgBns << endl;
    cout << "TOTAL:  " << setw(3) << avgTtl << endl;
    cout << "The standard deviation of the total scores is: " << stdDev << endl;

    //Exit the Program
    return 0;
}



void roll(int dice[], int numDice) {
    for (int i = 0; i < numDice; i++) {
        dice[i] = rand() % 6 + 1;
    }
}

void roll(int dice[], char hold[], int numDice) {
    for (int i = 0; i < numDice; i++) {
        if (!(hold[i] == 'Y' || hold[i] == 'y')) dice[i] = rand() % 6 + 1;
    }
}

void dispDce(int dice[], int numDice) {
    for (int i = 0; i < numDice; i++) {
        cout << "Dice " << i+1 << ": " << dice[i] << endl;
    }
}

void getHold(char hold[], int numDice) {
    for (int i = 0; i < numDice; i++) {
        // Ask if the user would like to hold dice 1
        cout << "Would you like to hold dice " << i+1 << "? (Y or N): ";
        cin >> hold[i];
    }
}

int turnScore(int dice[], int numDice, int numSel) {
    int turnScr = 0;
    for (int i = 0; i < numDice; i++) {
        if (dice[i] == numSel) turnScr += numSel;
    }
    return turnScr;
}

bool gameDone(bool uppDone[], int numUppR) {
    bool done = true;
    for (int i = 0; i < numUppR; i++) {
        if (uppDone[i] == false) done = false;
    }
    return done;
}

void displayScore(int uppScr[], bool uppDone[], int numUppR) {
    cout << "Current Scores:" << endl;

    for (int i = 0; i < numUppR; i++) {
        switch (i)
        {
        case 0:
            cout << "ACES:   ";
            break;
        case 1:
            cout << "TWOS:   ";
            break;
        case 2:
            cout << "THREES: ";
            break;
        case 3:
            cout << "FOURS:  ";
            break;
        case 4:
            cout << "FIVES:  ";
            break;
        case 5:
            cout << "SIXES:  ";
            break;
        default:
            break;
        }

        if (uppDone[i]) cout << setw(2) << uppScr[i];
        cout << endl;
    }
}

int selScore(bool uppDone[], int numUppR) {
    int numSel;
    do {
        cout << "Which row would you like to score this turn? (1-6): ";
        cin >> numSel; 
        if (numSel < 1 || numSel > 6) cout << "Selected number out of range" << endl;
        else if (uppDone[numSel-1]) cout << "Selected number has already been scored" << endl;
    } while (numSel < 1 || numSel > 6 || uppDone[numSel-1]);
    return numSel;
}

void displayTurn() {
    static int turn;
    turn++;
    cout << "Turn " << turn << endl;
}