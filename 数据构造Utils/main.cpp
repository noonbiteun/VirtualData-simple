#include <iostream>
#include <stdio.h>
#include <cstdlib>

using namespace std;

int LENGTH = 200;
int WIDTH = 600;
int HEIGHT = 150;

void sava2file(const char *path)
{
    FILE *tmpdat = fopen(path, "w+b");
    if (!tmpdat) {
        cout<<"open file failed"<<endl;
        return;
    }

    const int blocksize = LENGTH*WIDTH;
    unsigned char *buffer = new unsigned char[blocksize];
    for(int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < blocksize; j++)
        {
            buffer[j] = j+1;
        }
        fwrite(buffer, sizeof(unsigned char), blocksize, tmpdat);
    }
    delete[] buffer;
    fclose(tmpdat);
    cout<<"file save success"<<endl;
}


void read4file(const char *path)
{
    FILE *tmpdat = fopen(path, "r+b");
    if (!tmpdat) {
        cout<<"open file failed"<<endl;
        return;
    }

    const int blocksize = LENGTH*WIDTH;
    unsigned char *buffer = new unsigned char[blocksize];
    for (int i = 0; i < HEIGHT; i++)
    {
        for (int j = 0; j < blocksize; j++)
        {
            fread(buffer, sizeof(unsigned char), blocksize, tmpdat);
            for (int k = 0; k < blocksize; k++)
            {
                // if(buffer[k] != 0)
                    printf("%d \n", buffer[k]);
            }
        }
    }
    
    
    delete[] buffer;
    fclose(tmpdat);
    cout<<"file read success"<<endl;
}

int main(int argc, char const *argv[])
{
    LENGTH = atoi(argv[2]);
    WIDTH = atoi(argv[3]);
    HEIGHT = atoi(argv[4]);

    sava2file(argv[1]);
    // read4file();
    return 0;
}
