#include <stdio.h>
#define N 5

int main()
{
    int mas[N][N];
    int counter = 1;
    
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            mas[i][j] = counter;
            counter++;
        }
    }
    
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            printf("%2.0d ", mas[i][j]);
        }
        printf("\n");
    }
    
    return 0;
}
