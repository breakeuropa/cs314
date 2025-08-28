#include <stdio.h>
int main(int argc, char** argv)
{
        FILE* src = fopen(argv[1], "r");

        char buffer;
        for (int i = 1; i < argc; i++)
        {
                src = fopen(argc[1], 
                {
                        fwrite(&buffer, 1, 1, stdout);
                }
        }
        return 0;
}
