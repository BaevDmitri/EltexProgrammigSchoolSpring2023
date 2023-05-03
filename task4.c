#include <stdio.h>
#define N 5

void main()
{
    int mas[N][N];
    int counter = 1, i = 0, j = 0, offset = 0;
    
    while (counter < (N*N))
    {    
        while (j < N - offset - 1)
        {
            mas[i][j] = counter;
            counter++;
            j++;
        }
    
        while (i < N - offset - 1)
        {
            mas[i][j] = counter;
            counter++;
            i++;
        }
        
        while (j > offset)
        {
            mas[i][j] = counter;
            counter++;
            j--;
        }
    
        offset++;
        
        while (i > offset)
        {
            mas[i][j] = counter;
            counter++;
            i--;
        }
    }
    
    mas[N/2][N/2] = counter;
    
    for (int k = 0; k < N; k++)
    {
        for (int m = 0; m < N; m++)
        {
            printf("%.2d ", mas[k][m]);
        }
        printf("\n");
    }
}
