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

void avg(int [][2], int [], int [], int, int, int, int, int, float &);
void dispFin(int [][2], float, int = 7);

int main(int argc, char** argv) {
    // Set Random Number seed
    srand(static_cast<unsigned int>(time(0)));

    const int bnsCut = 63; // The minimum score the player must earn to get the bonus
    const int bnsScr = 35; // The bonus score the player earns if they meet the cutoff
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

    int scrSht[19][2];
    float stdDev;

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

    avg(scrSht, uppScr, lwrScr, uppSum, uppBns, uppTtl, lwrTtl, total, stdDev);

    dispFin(scrSht, stdDev);

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
    } else if (numSel == 14) {
        exit(0);
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

        cout << "14. Exit" << endl;

        cout << "Which row would you like to score this turn? (1-14): ";
        cin >> numSel; 
        if (numSel < 1 || numSel > 14) cout << "Selected number out of range" << endl;
        else if (numSel >= 1 && numSel <= 6 && uppDone[numSel-1]) cout << "Selected number has already been scored" << endl;
        else if (numSel >= 7 && numSel <= 13 && lwrDone[numSel-7]) cout << "Selected number has already been scored" << endl;
    } while (numSel < 1 || numSel > 14 || (numSel >= 1 && numSel <= 6 && uppDone[numSel-1]) || (numSel >= 7 && numSel <= 13 && lwrDone[numSel-7]));
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

void avg(int scrSht[][2], int uppScr[], int lwrScr[], int uppSum, int uppBns, int uppTtl, int lwrTtl, int total, float &stdDev) {
    const string fileNme = "avgData.dat";
    int count;
    int uppTtls[6];
    int uppSumT;
    int uppBnsT;
    int uppTtlT;
    int lwrTtls[7];
    int lwrTtlT;
    int totalT;
    float M2;

    float avgs[18];

    // Open the average data file, then read the count, totals, and M2
    ifstream avgIn(fileNme);
    avgIn >> count;
    avgIn >> uppTtls[0] >> uppTtls[1] >> uppTtls[2] >> uppTtls[3] >> uppTtls[4] >> uppTtls[5] >> uppSumT >> uppBnsT >> uppTtlT;
    avgIn >> lwrTtls[0] >> lwrTtls[1] >> lwrTtls[2] >> lwrTtls[3] >> lwrTtls[4] >> lwrTtls[5] >> lwrTtls[6] >> lwrTtlT;
    avgIn >> totalT;
    avgIn >> M2;
    avgIn.close();

    // Update the count and totals
    count++;

    uppTtls[0] += uppScr[0];
    uppTtls[1] += uppScr[1];
    uppTtls[2] += uppScr[2];
    uppTtls[3] += uppScr[3];
    uppTtls[4] += uppScr[4];
    uppTtls[5] += uppScr[5];
    uppSumT += uppSum;
    uppBnsT += uppBns;
    uppTtlT += uppTtl;

    lwrTtls[0] += lwrScr[0];
    lwrTtls[1] += lwrScr[1];
    lwrTtls[2] += lwrScr[2];
    lwrTtls[3] += lwrScr[3];
    lwrTtls[4] += lwrScr[4];
    lwrTtls[5] += lwrScr[5];
    lwrTtls[6] += lwrScr[6];
    lwrTtlT += lwrTtl;

    totalT += total;


    // Calculate the average of all the scores 
    avgs[0] = (float)uppTtls[0]/float(count);
    avgs[1] = (float)uppTtls[1]/float(count);
    avgs[2] = (float)uppTtls[2]/float(count);
    avgs[3] = (float)uppTtls[3]/float(count);
    avgs[4] = (float)uppTtls[4]/float(count);
    avgs[5] = (float)uppTtls[5]/float(count);

    avgs[6] = (float)uppSumT/float(count);
    avgs[7] = (float)uppBnsT/float(count);
    avgs[8] = (float)uppTtlT/float(count);

    avgs[9] = (float)lwrTtls[0]/float(count);
    avgs[10] = (float)lwrTtls[1]/float(count);
    avgs[11] = (float)lwrTtls[2]/float(count);
    avgs[12] = (float)lwrTtls[3]/float(count);
    avgs[13] = (float)lwrTtls[4]/float(count);
    avgs[14] = (float)lwrTtls[5]/float(count);
    avgs[15] = (float)lwrTtls[6]/float(count);
   
    avgs[16] = (float)lwrTtlT/float(count);
    avgs[17] = (float)totalT/float(count);

    // Calculate the standard deviation of the total score
    M2 += pow(uppTtl - avgs[17], 2);
    stdDev = (count > 1) ? sqrt(M2/(count-1)) : 0;

    // Store the updated count and totals into the file
    ofstream avgOut(fileNme);
    avgOut << count << " ";
    avgOut << uppTtls[0] << " " << uppTtls[1] << " " << uppTtls[2] << " " << uppTtls[3] << " " << uppTtls[4] << " " << uppTtls[5] << " " << uppSumT << " " << uppBnsT << " " << uppTtlT << " ";
    avgOut << lwrTtls[0] << " " << lwrTtls[1] << " " << lwrTtls[2] << " " << lwrTtls[3] << " " << lwrTtls[4] << " " << lwrTtls[5] << " " << lwrTtls[6] << " " << uppSumT << " ";
    avgOut << totalT << " ";
    avgOut << M2;
    avgOut.close();

    scrSht[0][0] = uppScr[0];
    scrSht[0][1] = avgs[0];

    scrSht[1][0] = uppScr[1];
    scrSht[1][1] = avgs[1];

    scrSht[2][0] = uppScr[2];
    scrSht[2][1] = avgs[2];

    scrSht[3][0] = uppScr[3];
    scrSht[3][1] = avgs[3];

    scrSht[4][0] = uppScr[4];
    scrSht[4][1] = avgs[4];

    scrSht[5][0] = uppScr[5];
    scrSht[5][1] = avgs[5];

    scrSht[6][0] = uppSum;
    scrSht[6][1] = avgs[6];

    scrSht[7][0] = uppBns;
    scrSht[7][1] = avgs[7];

    scrSht[8][0] = uppTtl;
    scrSht[8][1] = avgs[8];

    scrSht[9][0] = lwrScr[0];
    scrSht[9][1] = avgs[9];

    scrSht[10][0] = lwrScr[1];
    scrSht[10][1] = avgs[10];

    scrSht[11][0] = lwrScr[2];
    scrSht[11][1] = avgs[11];

    scrSht[12][0] = lwrScr[3];
    scrSht[12][1] = avgs[12];

    scrSht[13][0] = lwrScr[4];
    scrSht[13][1] = avgs[13];

    scrSht[14][0] = lwrScr[5];
    scrSht[14][1] = avgs[14];

    scrSht[15][0] = lwrScr[6];
    scrSht[15][1] = avgs[15];

    scrSht[16][0] = lwrTtl;
    scrSht[16][1] = avgs[16];

    scrSht[17][0] = uppTtl;
    scrSht[17][1] = avgs[8];

    scrSht[18][0] = total;
    scrSht[18][1] = avgs[17];
}

void dispFin(int scrSht[][2], float stdDev, int width) {
    // Display the score for the game
    cout << "Your score for this game and the average score of games played:" << endl;
    cout << "UPPER SECTION:          " << setw(width) << "Player" << " | " << setw(width) << "Average" << endl;
    cout << "Aces:                   " << setw(width) << scrSht[0][0] << " | " << setw(width) << scrSht[0][1] << endl;
    cout << "Twos:                   " << setw(width) << scrSht[1][0] << " | " << setw(width) << scrSht[1][1] << endl;
    cout << "Threes:                 " << setw(width) << scrSht[2][0] << " | " << setw(width) << scrSht[2][1] << endl;
    cout << "Fours:                  " << setw(width) << scrSht[3][0] << " | " << setw(width) << scrSht[3][1] << endl;
    cout << "Fives:                  " << setw(width) << scrSht[4][0] << " | " << setw(width) << scrSht[4][1] << endl;
    cout << "Sixes:                  " << setw(width) << scrSht[5][0] << " | " << setw(width) << scrSht[5][1] << endl;
    cout << "------------------------" << endl;
    cout << "TOTAL SCORE:            " << setw(width) << scrSht[6][0] << " | " << setw(width) << scrSht[6][1] << endl;
    cout << "BONUS:                  " << setw(width) << scrSht[7][0] << " | " << setw(width) << scrSht[7][1] << endl;
    cout << "TOTAL Of Upper Section: " << setw(width) << scrSht[8][0] << " | " << setw(width) << scrSht[8][1] << endl;
    cout << endl;
    cout << "LOWER SECTION:" << endl;
    cout << "3 of a kind:            " << setw(width) << scrSht[9][0] << " | " << setw(width) << scrSht[9][1] << endl;
    cout << "4 of a kind:            " << setw(width) << scrSht[10][0] << " | " << setw(width) << scrSht[10][1] << endl;
    cout << "Full House:             " << setw(width) << scrSht[11][0] << " | " << setw(width) << scrSht[11][1] << endl;
    cout << "Small Straight:         " << setw(width) << scrSht[12][0] << " | " << setw(width) << scrSht[12][1] << endl;
    cout << "Large Straight:         " << setw(width) << scrSht[13][0] << " | " << setw(width) << scrSht[13][1] << endl;
    cout << "YAHTZEE:                " << setw(width) << scrSht[14][0] << " | " << setw(width) << scrSht[14][1] << endl;
    cout << "Chance:                 " << setw(width) << scrSht[15][0] << " | " << setw(width) << scrSht[15][1] << endl;
    cout << "------------------------" << endl;
    cout << "TOTAL Of Lower Section: " << setw(width) << scrSht[16][0] << " | " << setw(width) << scrSht[16][1] << endl;
    cout << "TOTAL of Upper Section: " << setw(width) << scrSht[17][0] << " | " << setw(width) << scrSht[17][1] << endl;
    cout << "GRAND TOTAL:            " << setw(width) << scrSht[18][0] << " | " << setw(width) << scrSht[18][1] << endl;
    cout << "The standard deviation of the Grand Total is: " << fixed << setprecision(4) << stdDev << endl;
}