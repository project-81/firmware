/*
 * This is a comment. Instructions:
 * 1000 -> 1k
 * 1000000 -> 1M
 *  Want to be able to configure starting prefix.
 *  123456 seconds = H:M:S 34:17:36
 *  To make a new line - end in /n
 */

#include "common/amber.h"

int main(void)
{
    // obtain data from user
    cout << "Enter a time in seconds: ";
    int time_val;
    cin >> time_val;
    get_time(time_val);
    return 0;
}
