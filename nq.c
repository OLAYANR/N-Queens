#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* 
   Notes on MAX_BOARDSIZE:

   A 32 bit unsigned long is sufficient to hold the results for an 18 x 18
   board (666090624 solutions) but not for a 19 x 19 board (4968057848 solutions).
   
   In Win32, I use a 64 bit variable to hold the results, and merely set the
   MAX_BOARDSIZE to 21 because that's the largest board for which I've 
   calculated a result.

   Note: a 20x20 board will take over 20 hours to run on a Pentium III 800MHz,
   while a 21x21 board will take over a week to run on the same PC. 

   On Unix, you could probably change the type of g_numsolutions from unsigned long
   to unsigned long long, or change the code to use two 32 bit ints to store the
   results for board sizes 19 x 19 and up.
*/

#ifdef WIN32

#define MAX_BOARDSIZE 21
typedef unsigned __int64 SOLUTIONTYPE;

#else

#define MAX_BOARDSIZE 18
typedef unsigned int SOLUTIONTYPE;

#endif

#define MIN_BOARDSIZE 2

SOLUTIONTYPE g_numsolutions = 0; 


/* Print a chess table with queens positioned for a solution */
/* This is not a critical path function & I didn't try to optimize it. */
void printtable(int boardsize, int* aQueenBitRes, SOLUTIONTYPE numSolution)
{
    int i, j, k, row;

    /*  We only calculated half the solutions, because we can derive
        the other half by reflecting the solution across the "Y axis". */
    for (k = 0; k < 2; ++k)
    {
#ifdef WIN32
            printf("*** Solution #: %I64d ***\n", 2 * numSolution + k - 1);
#else
            printf("*** Solution #: %d ***\n", 2 * numSolution + k - 1);
#endif
        for ( i = 0; i < boardsize; i++)
        {
            unsigned int bitf;
            /*
               Get the column that was set (i.e. find the
               first, least significant, bit set).
               If aQueenBitRes[i] = 011010b, then
               bitf = 000010b
            */
            bitf = aQueenBitRes[i];

            row = bitf ^ (bitf & (bitf - 1)); /* get least significant bit */
            for ( j = 0; j < boardsize; j++)
            {
                /* keep shifting row over to the right until we find the one '1' in
                   the binary representation.  There will only be one '1'. */
                if (0 == k && ((row >> j) & 1))
                {
                    printf("Q ");
                }
                else if (1 == k && (row & (1 << (boardsize - j - 1)))) /* this is the board reflected across the "Y axis" */
                {
                    printf("Q ");
                }
                else
                {
                    printf(". ");
                }
            }
            printf("\n");
        }
        printf("\n");
    }
}


/* The function which calculates the N queen solutions.
   We calculate one-half the solutions, then flip the results over
   the "Y axis" of the board.  Every solution can be reflected that
   way to generate another unique solution (assuming the board size
   isn't 1 x 1).  That's because a solution cannot be symmetrical
   across the Y-axis (because you can't have two queens in the same
   horizontal row).  A solution also cannot consist of queens 
   down the middle column of a board with an odd number of columns,
   since you can't have two queens in the same vertical row.

   This is a backtracking algorithm.  We place a queen in the top
   row, then note the column and diagonals it occupies.  We then 
   place a queen in the next row down, taking care not to place it
   in the same column or diagonal.  We then update the occupied
   columns & diagonals & move on to the next row.  If no position
   is open in the next row, we back track to the previous row & move
   the queen over to the next available spot in its row & the process
   starts over again.
*/
void Nqueen(int board_size)
{
    int aQueenBitRes[MAX_BOARDSIZE]; /* results */
    int aQueenBitCol[MAX_BOARDSIZE]; /* marks colummns which already have queens */
    int aQueenBitPosDiag[MAX_BOARDSIZE]; /* marks "positive diagonals" which already have queens */
    int aQueenBitNegDiag[MAX_BOARDSIZE]; /* marks "negative diagonals" which already have queens */
    int aStack[MAX_BOARDSIZE + 2]; /* we use a stack instead of recursion */
    register int* pnStack;


    register int numrows = 0; /* numrows redundant - could use stack */
    register unsigned int lsb; /* least significant bit */
    register unsigned int bitfield; /* bits which are set mark possible positions for a queen */
    int i;
    int odd = board_size & 1; /* 0 if board_size even, 1 if odd */
    int board_minus = board_size - 1; /* board size - 1 */
    int mask = (1 << board_size) - 1; /* if board size is N, mask consists of N 1's */


    /* Initialize stack */
    aStack[0] = -1; /* set sentinel -- signifies end of stack */

    /* NOTE: (board_size & 1) is true iff board_size is odd */
    /* We need to loop through 2x if board_size is odd */
    for (i = 0; i < (1 + odd); ++i)
    {
        /* We don't have to optimize this part; it ain't the 
           critical loop */
        bitfield = 0;
        if (0 == i)
        {
            /* Handle half of the board, except the middle
               column. So if the board is 5 x 5, the first
               row will be: 00011, since we're not worrying
               about placing a queen in the center column (yet).
            */
            int half = board_size>>1; /* divide by two */
            /* fill in rightmost 1's in bitfield for half of board_size
               If board_size is 7, half of that is 3 (we're discarding the remainder)
               and bitfield will be set to 111 in binary. */
            bitfield = (1 << half) - 1;
            pnStack = aStack + 1; /* stack pointer */
    
            aQueenBitRes[0] = 0;
            aQueenBitCol[0] = aQueenBitPosDiag[0] = aQueenBitNegDiag[0] = 0;
        }
        else
        {
            /* Handle the middle column (of a odd-sized board).
               Set middle column bit to 1, then set
               half of next row.
               So we're processing first row (one element) & half of next.
               So if the board is 5 x 5, the first row will be: 00100, and
               the next row will be 00011.
            */
            bitfield = 1 << (board_size >> 1);
            numrows = 1; /* prob. already 0 */

            /* The first row just has one queen (in the middle column).*/
            aQueenBitRes[0] = bitfield;
            aQueenBitCol[0] = aQueenBitPosDiag[0] = aQueenBitNegDiag[0] = 0;
            aQueenBitCol[1] = bitfield;

            /* Now do the next row.  Only set bits in half of it, because we'll
               flip the results over the "Y-axis".  */
            aQueenBitNegDiag[1] = (bitfield >> 1);
            aQueenBitPosDiag[1] = (bitfield << 1);
            pnStack = aStack + 1; /* stack pointer */
            *pnStack++ = 0; /* we're done w/ this row -- only 1 element & we've done it */
            bitfield = (bitfield - 1) >> 1; /* bitfield -1 is all 1's to the left of the single 1 */
        }

        /* this is the critical loop */
        for (;;)
        {
            /* could use 
               lsb = bitfield ^ (bitfield & (bitfield -1)); 
               to get first (least sig) "1" bit, but that's slower. */
            lsb = -((signed)bitfield) & bitfield; /* this assumes a 2's complement architecture */
            if (0 == bitfield) 
            {
                bitfield = *--pnStack; /* get prev. bitfield from stack */
                if (pnStack == aStack) { /* if sentinel hit.... */
                    break ;
                }
                --numrows;
                continue;
            }
            bitfield &= ~lsb; /* toggle off this bit so we don't try it again */

            aQueenBitRes[numrows] = lsb; /* save the result */
            if (numrows < board_minus) /* we still have more rows to process? */
            {
                int n = numrows++;
                aQueenBitCol[numrows] = aQueenBitCol[n] | lsb;
                aQueenBitNegDiag[numrows] = (aQueenBitNegDiag[n] | lsb) >> 1;
                aQueenBitPosDiag[numrows] = (aQueenBitPosDiag[n] | lsb) << 1;
                *pnStack++ = bitfield;
                /* We can't consider positions for the queen which are in the same
                   column, same positive diagonal, or same negative diagonal as another
                   queen already on the board. */
                bitfield = mask & ~(aQueenBitCol[numrows] | aQueenBitNegDiag[numrows] | aQueenBitPosDiag[numrows]);
                continue;
            }
            else
            {
                /* We have no more rows to process; we found a solution. */
                /* Comment out the call to printtable in order to print the solutions as board position*/
                printtable(board_size, aQueenBitRes, g_numsolutions + 1); 
                ++g_numsolutions;
                bitfield = *--pnStack;
                --numrows;
                continue;
            }
        }
    }

    /* multiply solutions by two, to count mirror images */
    g_numsolutions *= 2;
}

/* Print the results at the end of the run */
void printResults(time_t* pt1, time_t* pt2)
{
    double secs;
    int hours , mins, intsecs;

    printf("End: \t%s", ctime(pt2));
    secs = difftime(*pt2, *pt1);
    intsecs = (int)secs;
    printf("Calculations took %d second%s.\n", intsecs, (intsecs == 1 ? "" : "s"));

    /* Print hours, minutes, seconds */
    hours = intsecs/3600;
    intsecs -= hours * 3600;
    mins = intsecs/60;
    intsecs -= mins * 60;
    if (hours > 0 || mins > 0) 
    {
        printf("Equals ");
        if (hours > 0) 
        {
            printf("%d hour%s, ", hours, (hours == 1) ? "" : "s");
        }
        if (mins > 0)
        {           
            printf("%d minute%s and ", mins, (mins == 1) ? "" : "s");
        }
        printf("%d second%s.\n", intsecs, (intsecs == 1 ? "" : "s"));

    }
}

/* main routine for N Queens program.*/
int main(int argc, char** argv)
{
    time_t t1, t2;
    int boardsize;

    if (argc != 2) {
        // printf("N Queens program by Jeff Somers.\n");
        //printf("\tallagash98@yahoo.com or jsomers@alumni.williams.edu\n");
        printf("This program calculates the total number of solutions to the N Queens problem.\n");
        printf("Usage: ./nqueen N= user pass in size of board --> \nfor example ./nqueen N=10\n\n"); 
	/* user must pass in size of board*/
 	printf("Note that printing the board arrangements slows down the program quite a bit \n,");
	printf("unless you pipe the output to a text file, as follows ./nqueen N=10 > output.txt\n");
        return 0;
    }

    boardsize = atoi(argv[1]);

    /* check size of board is within correct range */
    if (MIN_BOARDSIZE > boardsize || MAX_BOARDSIZE < boardsize)
    {
        printf("Width of board must be between %d and %d, inclusive.\n", 
                                            MIN_BOARDSIZE, MAX_BOARDSIZE );
        return 0;
    }

    time(&t1);
    printf("N Queens program by Jeff Somers.\n");
    printf("\tallagash98@yahoo.com or jsomers@alumni.williams.edu\n");
    printf("Start: \t %s", ctime(&t1));

    Nqueen(boardsize); /* find solutions */
    time(&t2);

    printResults(&t1, &t2);

    if (g_numsolutions != 0)
    {
#ifdef WIN32
        printf("For board size %d, %I64d solution%s found.\n", boardsize, g_numsolutions, (g_numsolutions == 1 ? "" : "s"));
#else
        printf("For board size %d, %d solution%s found.\n", boardsize, g_numsolutions, (g_numsolutions == 1 ? "" : "s"));
#endif
    }
    else
    {
        printf("No solutions found.\n");
    }

    return 0;
}
