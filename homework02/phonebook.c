#include <stdio.h>
#define N 50
#define C 15

struct contact
{
    int index;
    char firstName[C];
    char lastName[C];
    char phoneNumber[C];
    int checkBox;
};

void showContacts(struct contact * phoneBook)                                      
{                                                                               
    int totalContacts = 0; 
    for (int i = 0; i < N; i++)
    {
        if (phoneBook[i].checkBox == 1)
        {
            printf("%d. %s ", phoneBook[i].index+1, phoneBook[i].firstName);
            printf("%s %s\n", phoneBook[i].lastName, phoneBook[i].phoneNumber);
            totalContacts++;
        }
    }
    printf("Total: %d\n", totalContacts);    
}

int searchEmpty(struct contact * phoneBook)
{
    int i = 0;
    while (phoneBook[i].checkBox == 1)
    {
        i++;
    }
    if (i < N)
        return i;
    else
        return -1;
}

void addContacts(struct contact * phoneBook)
{
    int  numberNew = searchEmpty(phoneBook);
    if (numberNew == -1)
    {
        printf("Phonebook is overflow!");
    }
    phoneBook[numberNew].index = numberNew + 1;
    phoneBook[numberNew].checkBox = 1;
    printf("Type firstname: ");
    scanf("%s", phoneBook[numberNew].firstName);
    printf("Type lastname: ");
    scanf("%s", phoneBook[numberNew].lastName);
    printf("Type phonenumber: ");
    scanf("%s", phoneBook[numberNew].phoneNumber);
}

void deleteContact(struct contact * phoneBook)
{
    int i;
    printf("Contact index: ");
    scanf("%d", &i);
    phoneBook[i-1].index = 0;                                 
    phoneBook[i-1].checkBox = 0;
}

void main() {
    struct contact phoneBook[N] = { 0, "Test", "Testov", "666", 1 };
    int menuChoose, kill = 1;
    while (1)
    {
        printf("Menu:\n1. Show contacts;\n2. Add contacts;\n");
        printf("3. Delete contact: \nType options: ");
        scanf("%d", &menuChoose);
        switch(menuChoose) {
        case 1: showContacts(phoneBook); break;
        case 2: addContacts(phoneBook); break;
        case 3: deleteContact(phoneBook); break;
        default: printf("\nOops!\n"); kill = 0; break;
        }
        if (kill == 0)
            break;
    }
} 
