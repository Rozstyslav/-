#include <stdio.h>
#include <stdlib.h>

int main() 
{
   int aaaaaaaaaaaaaaaa;
   int bbbbbbbbbbbbbbbb;
   int xxxxxxxxxxxxxxxx;
   int yyyyyyyyyyyyyyyy;
   printf("Enter aaaaaaaaaaaaaaaa:");
   scanf("%d", &aaaaaaaaaaaaaaaa);
   printf("Enter bbbbbbbbbbbbbbbb:");
   scanf("%d", &bbbbbbbbbbbbbbbb);
   printf("%d\n", (aaaaaaaaaaaaaaaa + bbbbbbbbbbbbbbbb));
   printf("%d\n", (aaaaaaaaaaaaaaaa - bbbbbbbbbbbbbbbb));
   printf("%d\n", (aaaaaaaaaaaaaaaa * bbbbbbbbbbbbbbbb));
   printf("%d\n", (aaaaaaaaaaaaaaaa / bbbbbbbbbbbbbbbb));
   printf("%d\n", (aaaaaaaaaaaaaaaa % bbbbbbbbbbbbbbbb));
   xxxxxxxxxxxxxxxx = (((aaaaaaaaaaaaaaaa - bbbbbbbbbbbbbbbb) * 10) + ((aaaaaaaaaaaaaaaa + bbbbbbbbbbbbbbbb) / 10));
   yyyyyyyyyyyyyyyy = (xxxxxxxxxxxxxxxx + (xxxxxxxxxxxxxxxx % 10));
   printf("%d\n", xxxxxxxxxxxxxxxx);
   printf("%d\n", yyyyyyyyyyyyyyyy);
   system("pause");
    return 0;
}
