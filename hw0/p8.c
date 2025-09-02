#include <stdio.h>
#include <stdlib.h>

struct listnode
{
	int num;
	struct listnode* next, *prev;
}listnode;

void reverse(struct listnode** head)
{
	struct listnode* start = *head;
       	struct listnode* temp = NULL;

	while (start != NULL)
	{
		temp = start->prev;
        	start->prev = start->next;
        	start->next = temp;
		start = start->prev;
	}
	*head = temp->prev;
}

void printList(struct listnode* head)
{
	struct listnode* temp = head;
	
	while (temp != NULL)
	{
		printf("%d  ", temp->num);
		temp = temp->next;
	}
}

struct listnode* makeNode(int num)
{
	struct listnode* newNode = (struct listnode*)malloc(sizeof(struct listnode));
	newNode->num = num;
	newNode->next = NULL;
	newNode->prev = NULL;
	return newNode;
}

void insert(struct listnode** head, int num)
{
	struct listnode* newNode = makeNode(num);
	
	if (*head == NULL)
	{
		*head = newNode;
		return;
	}
	struct listnode* temp = *head;

	while (temp->next != NULL)
	{
		temp = temp->next;
	}

	temp->next = newNode;
	newNode->prev = temp;
}

int main()
{
	printf("\n\nProblem 8\n\n");

	struct listnode* head = nullptr;
	struct listnode* temp = head;
	
	insert(&head, 1);
	insert(&head, 2);
	insert(&head, 3);
	insert(&head, 4);
	insert(&head, 5);

	printf("List: ");
	printList(head);
	printf("\nReversing list: "); 
	reverse(&head);
	printList(head);
	printf("\n");
	

	return 0;
}
