#include <stdio.h>
#include <stdlib.h>

struct listnode
{
	int num;
	struct listnode* next;
}listnode;

int has_loop(struct listnode* head)
{
	struct listnode* temp = head;
	int noLoop = 0;
	int loop = 1;

	while (temp->next != NULL)
	{
		temp = temp->next;
		if (temp->next == head)
			return loop;
	}

	return noLoop;
	
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
	if (num == 6)
	{
		temp->next = *head;
		return;
	}
	else
		temp->next = newNode;
}

int main()
{
	printf("\n\nProblem 7\n\n");

	struct listnode* head = nullptr;
	struct listnode* temp = head;
	
	insert(&head, 1);
	insert(&head, 2);
	insert(&head, 3);
	insert(&head, 4);
	insert(&head, 5);
	insert(&head, 6); //for loop list

	//printf("List: ");
	//printList(head);
	printf("\nCheck for loop: ");
	printf("%d\n", has_loop(head));
	//printf("List: ");
	//printList(head);
	printf("\n");
	

	return 0;
}
