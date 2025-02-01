#include <stdio.h>
#include <stdlib.h>

int main() 
{
   int aaaaaaaaaaaaaaaa, bbbbbbbbbbbbbbbb, cccccccccccccccc, cycleaaaaaaaaaaa, cyclebbbbbbbbbbb, cycleccccccccccc;
   printf("Enter aaaaaaaaaaaaaaaa:");
   scanf("%d", &aaaaaaaaaaaaaaaa);
   aaaaaaaaaaaaaaaa = aaaaaaaaaaaaaaaa + 1;
   for (cycleaaaaaaaaaaa = 0; 32767 >= cycleaaaaaaaaaaa; cycleaaaaaaaaaaa++)
   {
   bbbbbbbbbbbbbbbb = aaaaaaaaaaaaaaaa - 1;
   for (cyclebbbbbbbbbbb = 0; 32767 >= cyclebbbbbbbbbbb; cyclebbbbbbbbbbb++)
   {
   if ((bbbbbbbbbbbbbbbb > 1))
{
}
   else
{
   goto endcyclebbbbbbbb;
}
   cccccccccccccccc = aaaaaaaaaaaaaaaa;
   for (cycleccccccccccc = 0; 32767 >= cycleccccccccccc; cycleccccccccccc++)
   {
   if ((cccccccccccccccc > bbbbbbbbbbbbbbbb))
{
}
   else
{
   goto endcyclecccccccc;
}
   cccccccccccccccc = cccccccccccccccc - bbbbbbbbbbbbbbbb;
   }
endcyclecccccccc:
   if ((cccccccccccccccc == 0))
{
   goto endcyclebbbbbbbb;
}
   bbbbbbbbbbbbbbbb = bbbbbbbbbbbbbbbb - 1;
   }
endcyclebbbbbbbb:
   if ((bbbbbbbbbbbbbbbb == 1))
{
   goto endcycleaaaaaaaa;
}
   aaaaaaaaaaaaaaaa = aaaaaaaaaaaaaaaa + 1;
   }
endcycleaaaaaaaa:
   printf("%d\n", aaaaaaaaaaaaaaaa);
   system("pause");
    return 0;
}
