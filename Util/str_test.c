#include<stdio.h>
#include "str_util.h"

int main()
{
    char s1[10]="1234\r\n";
    char s2[10]="5678\n";

    printf("s1 len=%d, [%s]\n", strlen(s1), s1);
    printf("s2 len=%d, [%s]\n", strlen(s2), s2);

    str_remove_crlf(s1);
    str_remove_crlf(s2);

    printf("s1 len=%d, [%s]\n", strlen(s1), s1);
    printf("s2 len=%d, [%s]\n", strlen(s2), s2);

}