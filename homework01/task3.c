#include <stdio.h>
#define N 10

void main()
{
    int mas[N][N];
    
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if ((i + j) >= N-1)
            {
                mas[i][j] = 1;
            }
            else
            {
                mas[i][j] = 0;
            }
        }
    }
    
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            printf("%d ", mas[i][j]);
        }
        printf("\n");
    }        
}
