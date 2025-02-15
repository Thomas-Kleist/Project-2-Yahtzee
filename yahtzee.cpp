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
#include <vector>
using namespace std;

//User Libraries

//Global Constants - No Global Variables
//Only Universal Constants, Math, Physics, Conversions, Higher Dimensions

// Function Prototypes
void roll(int [], int);                         // Role dice with no hold
void roll(int [], char[], int);                 // Role dice with hold
void dispDce(int [], int);                      // Display dice
void getHold(char[], int);                      // Ask the player if they would like to hold each dice
bool gameDone(bool [], int, bool [], int);      // Get if the game is completed based on the row completion
void displayScore(int [], bool [], int, int [], bool [], int);        // Display the current scores
void selScore(bool [], bool [], int&);              // Select which row to score
void displayTurn();                             // Display the turn number
void selSort(int [],int);                       // Selection sort
void bubSort(int [],int);                       // Selection sort
void updateScore(int [], bool [], int[], bool [], int, int); // Update the scores based on a turn score

int turnScore(int [], int, int);                // Get the score for the turn
vector<int> getDiceVals(int [], int);           // Get the number of each value of dice
int linearSearch(vector<int>, int);

int totalScore(int [], int[], int, int, int &, int &, int&, int&);

void dispFin(int [], int [], int, int, int, int, int);

int main(int argc, char** argv) {
    // Set Random Number seed
    srand(static_cast<unsigned int>(time(0)));

    const int bnsCut = 63; // The minimum score the player must earn to get the bonus
    const int bnsScr = 35; // The bonus score the player earns if they meet the cutoff
    const string fileNme = "avgData.dat";
    const int numDice = 5;
    const int numUppR = 6;
    const int numLwrR = 7;

    //Declare Variables
    int dice[numDice]; // The value of the dice
    char hold[numDice]; // Wether or not the user wants to hold each dice. 'Y' or 'y' for yes, anything else for no
    int numSel; // The number the user selects to score this turn
    int turnScr; // The score for this turn

    int uppScr[numUppR]; // The score for each of numbers on a die
    bool uppDone[numUppR]; // Bool to store if a certain number has been scored

    int lwrScr[numLwrR]; // The score for each of numbers on a die
    bool lwrDone[numLwrR]; // Bool to store if a certain number has been scored
    
    int uppSum, uppBns, uppTtl; // The sum of all the scores, the bonus points, and the total score for the game
    int lwrTtl;
    int total;

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
    for (int i = 0; i < numLwrR; i++) {
        lwrScr[i] = 0;
        lwrDone[i] = false;
    }

    // Run the game
    // Run the loop until all numbers have been scored
    while (!gameDone(uppDone, numUppR, lwrDone, numLwrR)) {
        // Display turn count and current scores
        displayTurn();
        displayScore(uppScr, uppDone, numUppR, lwrScr, lwrDone, numLwrR);

        // Loop 3 times because each turn in yahtzee allows three roles
        for (int i = 0; i < 3; i++) {
            cout << "Roll " << i+1 << endl;
            
            if (i == 0) roll(dice, numDice);
            else roll(dice, hold, numDice);

            if (i == 0 || i == 2) selSort(dice, numDice);
            if (i == 1) bubSort(dice, numDice);

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
        selScore(uppDone, lwrDone, numSel);
        turnScr = turnScore(dice, numDice, numSel);
        updateScore(uppScr, uppDone, lwrScr, lwrDone, turnScr, numSel);
    }

    total = totalScore(uppScr, lwrScr, numUppR, numLwrR, uppSum, uppBns, uppTtl, lwrTtl);

    dispFin(uppScr, lwrScr, uppSum, uppBns, uppTtl, lwrTtl, total);

    // // Open the average data file, then read the count, totals, and M2
    // ifstream avgIn(fileNme);
    // avgIn >> count >> ttl1 >> ttl2 >> ttl3 >> ttl4 >> ttl5 >> ttl6 >> ttlSum >> ttlBns >> ttlTtl >> M2;
    // avgIn.close();

    // // Update the count and totals
    // count++;
    // ttl1 += uppScr[0];
    // ttl2 += uppScr[1];
    // ttl3 += uppScr[2];
    // ttl4 += uppScr[3];
    // ttl5 += uppScr[4];
    // ttl6 += uppScr[5];
    // ttlSum += uppSum;
    // ttlBns += bonus;
    // ttlTtl += uppTtl;

    // // Calculate the average of all the scores 
    // avg1 = (float)ttl1/float(count);
    // avg2 = (float)ttl2/float(count);
    // avg3 = (float)ttl3/float(count);
    // avg4 = (float)ttl4/float(count);
    // avg5 = (float)ttl5/float(count);
    // avg6 = (float)ttl6/float(count);
    // avgSum = (float)ttlSum/float(count);
    // avgBns = (float)ttlBns/float(count);
    // avgTtl = (float)ttlTtl/float(count);
    // // Calculate the standard deviation of the total score
    // M2 += pow(uppTtl - avgTtl, 2);
    // stdDev = (count > 1) ? sqrt(M2/(count-1)) : 0;

    // // Store the updated count and totals into the file
    // ofstream avgOut(fileNme);
    // avgOut << count << " " << ttl1 << " " << ttl2 << " " << ttl3 << " " << ttl4 << " " << ttl5 << " " << ttl6 << " " << ttlSum << " " << ttlBns << " " << ttlTtl << " " << M2;
    // avgOut.close();

    // // Output the average score in the same formatting as the users score
    // cout << "The average score of games played:" << endl;
    // cout << "ACES:   " << setw(3) << avg1 << endl;
    // cout << "TWOS:   " << setw(3) << avg2 << endl;
    // cout << "THREES: " << setw(3) << avg3 << endl;
    // cout << "FOURS:  " << setw(3) << avg4 << endl;
    // cout << "FIVES:  " << setw(3) << avg5 << endl;
    // cout << "SIXES:  " << setw(3) << avg6 << endl;
    // cout << "-----------" << endl;
    // cout << "SCORE:  " << setw(3) << avgSum << endl;
    // cout << "BONUS:  " << setw(3) << avgBns << endl;
    // cout << "TOTAL:  " << setw(3) << avgTtl << endl;
    // cout << "The standard deviation of the total scores is: " << stdDev << endl;

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
    vector<int> diceVal = getDiceVals(dice, numDice);
    int turnScr = 0;
    if (numSel >= 1 && numSel <= 6) {
        turnScr = diceVal[numSel-1] * numSel;
    } else if (numSel == 7) {
        if (linearSearch(diceVal, 3) != -1 || linearSearch(diceVal, 4) != -1 || linearSearch(diceVal, 5) != -1) {
            for (int i = 0; i < numDice; i++) {
                turnScr += dice[i];
            }
        }
    } else if (numSel == 8) {
        if (linearSearch(diceVal, 4) != -1 || linearSearch(diceVal, 5) != -1) {
            for (int i = 0; i < numDice; i++) {
                turnScr += dice[i];
            }
        }
    } else if (numSel == 9) {
        if (linearSearch(diceVal, 2) != -1 && linearSearch(diceVal, 3) != -1) {
            turnScr = 25;
        }
    } else if (numSel == 10) {
        if ((diceVal[0] >= 1 && diceVal[1] >= 1 && diceVal[2] >= 1 && diceVal[3] >= 1) || // 1, 2, 3, 4
            (diceVal[1] >= 1 && diceVal[2] >= 1 && diceVal[3] >= 1 && diceVal[4] >= 1) || // 2, 3, 4, 5
            (diceVal[2] >= 1 && diceVal[3] >= 1 && diceVal[4] >= 1 && diceVal[5] >= 1))   // 3, 4, 5, 6
        {
            turnScr = 30;
        }
    } else if (numSel == 11) {
        if ((diceVal[0] >= 1 && diceVal[1] >= 1 && diceVal[2] >= 1 && diceVal[3] >= 1 && diceVal[4] >= 1) || // 1, 2, 3, 4, 5
            (diceVal[1] >= 1 && diceVal[2] >= 1 && diceVal[3] >= 1 && diceVal[4] >= 1 && diceVal[5] >= 1))   // 2, 3, 4, 5, 6
        {
            turnScr = 40;
        }
    } else if (numSel == 12) {
        if (linearSearch(diceVal, 5) != -1) {
            turnScr = 50;
        }
    } else if (numSel == 13) {
        for (int i = 0; i < numDice; i++) {
            turnScr += dice[i];
        }
    }
    return turnScr;
}

vector<int> getDiceVals(int dice[], int numDice) {
    vector<int> vals = {0, 0, 0, 0, 0, 0};

    for (int i = 0; i < numDice; i++) {
        vals[dice[i]-1]++;
    }

    return vals;
}

int linearSearch(vector<int> vec, int target) {
    for (size_t i = 0; i < vec.size(); ++i) {
        if (vec[i] == target) {
            return (int)i; // Return the index if found
        }
    }
    return -1; // Return -1 if not found
}

bool gameDone(bool uppDone[], int numUppR, bool lwrDone[], int numLwrR) {
    bool done = true;
    for (int i = 0; i < numUppR; i++) {
        if (uppDone[i] == false) done = false;
    }
    for (int i = 0; i < numLwrR; i++) {
        if (lwrDone[i] == false) done = false;
    }
    return done;
}

void displayScore(int uppScr[], bool uppDone[], int numUppR, int lwrScr[], bool lwrDone[], int numLwrR) {
    cout << "Current Scores:" << endl;

    for (int i = 0; i < numUppR; i++) {
        switch (i)
        {
        case 0:
            cout << "Aces:         ";
            break;
        case 1:
            cout << "Twos:         ";
            break;
        case 2:
            cout << "Threes:       ";
            break;
        case 3:
            cout << "Fours:        ";
            break;
        case 4:
            cout << "Fives:        ";
            break;
        case 5:
            cout << "Sixes:        ";
            break;
        default:
            break;
        }

        if (uppDone[i]) cout << setw(2) << uppScr[i];
        cout << endl;
    }

    for (int i = 0; i < numLwrR; i++) {
        switch (i)
        {
        case 0:
            cout << "3 of a kind:  ";
            break;
        case 1:
            cout << "4 of a kind:  ";
            break;
        case 2:
            cout << "Full House:   ";
            break;
        case 3:
            cout << "Sm. Straight: ";
            break;
        case 4:
            cout << "Lg. Straight: ";
            break;
        case 5:
            cout << "YAHTZEE:      ";
            break;
        case 6:
            cout << "Chance:       ";
            break;
        default:
            break;
        }

        if (lwrDone[i]) cout << setw(2) << lwrScr[i];
        cout << endl;
    }
}

void selScore(bool uppDone[], bool lwrDone[], int &numSel) {
    do {
        if (uppDone[0] == false) cout << "1. Aces" << endl;
        if (uppDone[1] == false) cout << "2. Twos" << endl;
        if (uppDone[2] == false) cout << "3. Threes" << endl;
        if (uppDone[3] == false) cout << "4. Fours" << endl;
        if (uppDone[4] == false) cout << "5. Fives" << endl;
        if (uppDone[5] == false) cout << "6. Sixes" << endl;

        if (lwrDone[0] == false) cout << "7. 3 of a kind" << endl;
        if (lwrDone[1] == false) cout << "8. 4 of a kind" << endl;
        if (lwrDone[2] == false) cout << "9. Full House" << endl;
        if (lwrDone[3] == false) cout << "10. Small Straight" << endl;
        if (lwrDone[4] == false) cout << "11. Large Straight" << endl;
        if (lwrDone[5] == false) cout << "12. YAHTZEE" << endl;
        if (lwrDone[6] == false) cout << "13. Chance" << endl;

        cout << "Which row would you like to score this turn? (1-13): ";
        cin >> numSel; 
        if (numSel < 1 || numSel > 13) cout << "Selected number out of range" << endl;
        else if (numSel >= 1 && numSel <= 6 && uppDone[numSel-1]) cout << "Selected number has already been scored" << endl;
        else if (numSel >= 7 && numSel <= 13 && lwrDone[numSel-7]) cout << "Selected number has already been scored" << endl;
    } while (numSel < 1 || numSel > 13 || (numSel >= 1 && numSel <= 6 && uppDone[numSel-1]) || (numSel >= 7 && numSel <= 13 && lwrDone[numSel-7]));
}

void displayTurn() {
    static int turn;
    turn++;
    cout << "Turn " << turn << endl;
}

void selSort(int a[],int n) {
    for(int pos=0;pos<n-1;pos++){
        int selIndx=pos;
        for(int i=pos+1;i<n;i++){
            if(*(a+selIndx)>*(a+i))selIndx=i;
        }
        int temp = a[selIndx];
        a[selIndx] = a[pos];
        a[pos] = temp;
    }
}

void bubSort(int a[],int n){
    bool swap;
    int redList=n-1;
    do{
        swap=false;
        for(int i=0;i<redList;i++){
            if(a[i]>a[i+1]){
                int temp = a[i];
                a[i] = a[i+1];
                a[i+1] = temp;
                swap=true;
            }
        } 
       redList--;
    }while(swap);
}

void updateScore(int uppScr[], bool uppDone[], int lwrScr[], bool lwrDone[], int turnScr, int numSel) {
    // Output the number of points earned for the user to see
    // Save the number of points in the variable for the row
    // Set the bool of that row to true to indicate that the row has been scored
    switch (numSel)
    {
    case 1:
        cout << "The number of points earned in the Aces row is " << turnScr << endl << endl << endl;
        uppScr[0] = turnScr;
        uppDone[0] = true;
        break;
    case 2:
        cout << "The number of points earned in the Twos row is " << turnScr << endl << endl << endl;
        uppScr[1] = turnScr;
        uppDone[1] = true;
        break;
    case 3:
        cout << "The number of points earned in the Threes row is " << turnScr << endl << endl << endl;
        uppScr[2] = turnScr;
        uppDone[2] = true;
        break;
    case 4:
        cout << "The number of points earned in the Fours row is " << turnScr << endl << endl << endl;
        uppScr[3] = turnScr;
        uppDone[3] = true;
        break;
    case 5:
        cout << "The number of points earned in the Fives row is " << turnScr << endl << endl << endl;
        uppScr[4] = turnScr;
        uppDone[4] = true;
        break;
    case 6:
        cout << "The number of points earned in the Sizes row is " << turnScr << endl << endl << endl;
        uppScr[5] = turnScr;
        uppDone[5] = true;
        break;
    case 7:
        cout << "The number of points earned in the 3 of a kind row is " << turnScr << endl << endl << endl;
        lwrScr[0] = turnScr;
        lwrDone[0] = true;
        break;
    case 8:
        cout << "The number of points earned in the 4 of a kind row is " << turnScr << endl << endl << endl;
        lwrScr[1] = turnScr;
        lwrDone[1] = true;
        break;
    case 9:
        cout << "The number of points earned in the Full House row is " << turnScr << endl << endl << endl;
        lwrScr[2] = turnScr;
        lwrDone[2] = true;
        break;
    case 10:
        cout << "The number of points earned in the Small Straight row is " << turnScr << endl << endl << endl;
        lwrScr[3] = turnScr;
        lwrDone[3] = true;
        break;
    case 11:
        cout << "The number of points earned in the Large Straight row is " << turnScr << endl << endl << endl;
        lwrScr[4] = turnScr;
        lwrDone[4] = true;
        break;
    case 12:
        cout << "The number of points earned in the YAHTZEE row is " << turnScr << endl << endl << endl;
        lwrScr[5] = turnScr;
        lwrDone[5] = true;
        break;
    case 13:
        cout << "The number of points earned in the Chance row is " << turnScr << endl << endl << endl;
        lwrScr[6] = turnScr;
        lwrDone[6] = true;
        break;
    default:
        break;
    }
}

int totalScore(int uppScr[], int lwrScr[], int numUppR, int numLwrR, int &uppSum, int &uppBns, int &uppTtl, int &lwrTtl) {
    const int bnsCut = 35;

    // After all the scores have been filled in, compute the score and output a nicely formatted score card. 
    uppSum = 0;
    for (int i = 0; i < numUppR; i++) {
        uppSum += uppScr[i];
    }
    if (uppSum >= bnsCut) uppBns = 35; else uppBns = 0;
    uppTtl = uppSum + uppBns;

    lwrTtl = 0;
    for (int i = 0; i < numLwrR; i++) {
        lwrTtl += lwrScr[i];
    }

    return uppTtl + lwrTtl;
}

void dispFin(int uppScr[], int lwrScr[], int uppSum, int uppBns, int uppTtl, int lwrTtl, int total) {
    // Display the score for the game
    cout << "Your score for this game:" << endl;
    cout << "UPPER SECTION:" << endl;
    cout << "Aces:                   " << setw(3) << uppScr[0] << endl;
    cout << "Twos:                   " << setw(3) << uppScr[1] << endl;
    cout << "Threes:                 " << setw(3) << uppScr[2] << endl;
    cout << "Fours:                  " << setw(3) << uppScr[3] << endl;
    cout << "Fives:                  " << setw(3) << uppScr[4] << endl;
    cout << "Sixes:                  " << setw(3) << uppScr[5] << endl;
    cout << "------------------------" << endl;
    cout << "TOTAL SCORE:            " << setw(3) << uppSum << endl;
    cout << "BONUS:                  " << setw(3) << uppBns << endl;
    cout << "TOTAL Of Upper Section: " << setw(3) << uppTtl << endl;
    cout << endl;
    cout << "LOWER SECTION:" << endl;
    cout << "3 of a kind:            " << setw(3) << lwrScr[0] << endl;
    cout << "4 of a kind:            " << setw(3) << lwrScr[1] << endl;
    cout << "Full House:             " << setw(3) << lwrScr[2] << endl;
    cout << "Small Straight:         " << setw(3) << lwrScr[3] << endl;
    cout << "Large Straight:         " << setw(3) << lwrScr[4] << endl;
    cout << "YAHTZEE:                " << setw(3) << lwrScr[5] << endl;
    cout << "Chance:                 " << setw(3) << lwrScr[6] << endl;
    cout << "------------------------" << endl;
    cout << "TOTAL Of Lower Section: " << setw(3) << lwrTtl << endl;
    cout << "TOTAL of Upper Section: " << setw(3) << uppTtl << endl;
    cout << "GRAND TOTAL:            " << setw(3) << total << endl;
}