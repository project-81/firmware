#pragma once

#include <iostream>
using namespace std; // lets us not put std:: before cout

/*
 * Takes time in seconds and converts to hours, minutes, and seconds
 * and then prints the result.
 */
void get_time(int time)
{
    // declare variables
    int hour = 0;
    int min = 0;
    int sec = 0;

    hour = time / 3600;
    time = time % 3600;
    min = time / 60;
    time = time % 60;
    sec = time;

    cout << "\nTime is: " << hour << " h, " << min << " m, " << sec << " s\n";
}
