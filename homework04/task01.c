#include <stdlib.h>
#include <stdio.h>
#define N 20

struct list
{
    int index;
    char *firstName;
    char *lastName;
    char *number;
    struct list *next;
};


struct list *listInit()
{
    struct list *tmp = (struct list*)malloc(sizeof(struct list));
    char* fN = (char*)malloc(N*sizeof(char));
    fN = "1";
    tmp->firstName = fN;
    char *lN = (char*)malloc(N*sizeof(char));
    lN = "1";
    tmp->lastName = lN;
    char *nN = (char*)malloc(N*sizeof(char));
    nN = "1";
    tmp->number = nN;
    tmp->index = 1;
    tmp->next = NULL;
    return(tmp);
}

void listPrint(struct list *tmp)
{
    while (tmp != NULL)
    {
        printf("%d. %s ", tmp -> index, tmp -> firstName);
        printf("%s %s\n", tmp -> lastName, tmp -> number);
        tmp = tmp -> next;
    }
}

void listAdd(struct list *head)
{
    struct list *tmp = (struct list*)malloc(sizeof(struct list));
    printf("Type firstname: ");
    char *fN = (char*)malloc(N*sizeof(char));
    scanf("%s", fN);
    tmp->firstName = fN;
    printf("Type lastname: "); 
    char *lN = (char*)malloc(N*sizeof(char));
    scanf("%s", lN);
    tmp->lastName = lN;
    printf("Type phonenumber: ");                                               
    char *nN = (char*)malloc(N*sizeof(char));
    scanf("%s", nN);
    tmp->number = nN;
    tmp -> next = NULL;
    struct list *p = head;
    while (p -> next != NULL)
    {
        p = p -> next;
    }
    p -> next = tmp;
    tmp->index = p->index+1;
}

struct list *allDelete(struct list *head)
{
    struct list *p = head;
    while (head != NULL)
    {
        free(p->firstName);
        printf("check!");
        free(p->lastName);
        printf("check!");
        free(p->number);
        printf("check!");
        head = head -> next;
        free(p);
        printf("check!");
    }
    return NULL;
}

struct list *deleteContact(struct list *head)
{
    struct list *tmp = head, *p = NULL;
    if (head == NULL)
    {
        printf("List is empty!\n");
        return NULL;
    }
    int delIndex;
    printf("Type index here: ");
    scanf("%d", &delIndex);
    while (tmp && tmp->index != delIndex)
    {
        p = tmp;
        tmp = tmp->next;
    }
    if (tmp == head)
    {
        free(tmp->firstName);
        free(tmp->lastName);
        free(tmp->number);
        free(tmp);
        return tmp->next; 
    }
    if (tmp == NULL)
    {
        return head;
    }
    p->next = tmp->next;
    free(tmp->firstName);
    free(tmp->lastName);
    free(tmp->number);
    free(tmp);
    return head;
}

void main ()
{
    int choose, kill = 1;
    struct list *head = listInit();
    while (1)                                                                   
    {                                                                           
        printf("Menu:\n1. Show contacts;\n2. Add contacts;\n");                 
        printf("3. Delete contact;\nAny. Kill list.\nType options: ");                          
        scanf("%d", &choose);                                               
        switch(choose) {                                                    
        case 1: listPrint(head); break;                                 
        case 2: listAdd(head); break;                                  
        case 3: head = deleteContact(head); break;              
        default: printf("\nOops!\n"); kill = 0; break;                          
        }                                                                       
        if (kill == 0)                                                          
            break;                                                              
    }
    head = allDelete(head);
}
