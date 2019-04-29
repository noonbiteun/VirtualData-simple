#include <iostream>
#include "VirtualData.h"

int main(int argc, char const *argv[])
{
    int L = 400;
    int W = 600;
    int H = 150;
    VirtualData<unsigned char> vd(L, W, H, 2, 1, 1);
    for (int i = 0; i < L; i++)
    {
        for (int j = 0; j < W; j++)
        {
            for (int k = 0; k < H; k++)
            {
                vd.GetValue(i, j, k) = k;
            }
            // printf("\n");
        }
    }

printf("%d\n", vd.GetValue(210, 6, 10));
    
    return 0;
}
