#include <stdio.h>
#include <stdlib.h>

struct listnode
{
	int num;
	struct listnode* next;
}listnode;

int findNthElement(struct listnode* head, int N)
{
	struct listnode* temp = head;
	int count = 1;
	while (temp != NULL)
	{
		if (count == N)
			return temp->num;
		temp = temp->next;
		count ++;
	}
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
	temp->next = newNode;
}

int main()
{
	printf("\n\nProblem 4\n\n");

	struct listnode* head = nullptr;
	struct listnode* temp = head;
	
	insert(&head, 1);
	insert(&head, 2);
	insert(&head, 3);
	insert(&head, 4);
	insert(&head, 5);

	printf("List: ");
	printList(head);
	printf("\nFind second element: ");
	printf("%d\n", findNthElement(head, 2));
	printf("\n");
	

	return 0;
}
