#include "bruter.h"

char* strduplicate(const char *str)
{
    char *dup = (char*)malloc(strlen(str) + 1);
    strcpy(dup, str);
    return dup;
}

char* strnduplicate(const char *str, Int n)
{
    char *dup = (char*)malloc(n + 1);
    for (Int i = 0; i < n; i++)
    {
        dup[i] = str[i];
    }
    dup[n] = '\0';
    return dup;
}

char* toString(CharList *list)
{

    if (list->size == 0)
    {
        return "";
    }
    
    char *str = (char*)malloc(list->size + 2);
    for (Int i = 0; i < list->size; i++)
    {
        if(list->data != NULL)
        {
            if (list->data[i] == '\0')
            {
                str[i] = ' ';
            }
            else
            {
                str[i] = list->data[i];
            }
        }
    }
    str[list->size] = '\0';
    return str;
}

StringList* specialSplit(char *str)
{
    StringList *splited;
    splited = (StringList*)malloc(sizeof(StringList));
    StackInit(*splited);
    
    Int i = 0;
    while (str[i] != '\0')
    {
        if (str[i] == '(')
        {
            Int j = i;
            Int count = 1;
            while (count != 0)
            {
                j++;
                if (str[j] == '(')
                {
                    count++;
                }
                else if (str[j] == ')')
                {
                    count--;
                }
            }
            char *tmp = strnduplicate(str + i, j - i + 1);
            StackPush(*splited, tmp);
            i = j + 1;
        }
        else if (str[i] == '@' && str[i + 1] == '(')
        {
            Int j = i + 1;
            Int count = 1;
            while (count != 0)
            {
                j++;
                if (str[j] == '(')
                {
                    count++;
                }
                else if (str[j] == ')')
                {
                    count--;
                }
            }
            char *tmp = strnduplicate(str + i, j - i + 1);
            StackPush(*splited, tmp);
            i = j + 1;
        }
        else if (str[i] == '!' && str[i + 1] == '(')
        {
            Int j = i + 1;
            Int count = 1;
            while (count != 0)
            {
                j++;
                if (str[j] == '(')
                {
                    count++;
                }
                else if (str[j] == ')')
                {
                    count--;
                }
            }
            char *tmp = strnduplicate(str + i, j - i + 1);
            StackPush(*splited, tmp);
            i = j + 1;
        }
        else if (isSpace(str[i]))
        {
            i++;
        }
        else
        {
            Int j = i;
            while (!isSpace(str[j]) && str[j] != '\0' && str[j] != '(' && str[j] != ')')
            {
                j++;
            }
            StackPush(*splited, strnduplicate(str + i, j - i));
            i = j;
        }
    }

    return splited;
}

StringList* splitString(char *str, char *delim)
{
    StringList *splited;
    splited = (StringList*)malloc(sizeof(StringList));
    StackInit(*splited);
    
    Int i = 0;
    while (str[i] != '\0')
    {
        if (str[i] == delim[0])
        {
            i++;
        }
        else
        {
            Int j = i;
            while (str[j] != delim[0] && str[j] != '\0')
            {
                j++;
            }
            StackPush(*splited, strnduplicate(str + i, j - i));
            i = j;
        }
    }

    return splited;
}