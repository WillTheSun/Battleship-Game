/********************************************************/
/* William Sun, wjs45									*/
/* DATE: March 10, 2015									*/
/* FILES: ships.c, ships.h, testShips.c 				*/
/* DESCRIPTION: 										*/
/* Implementation of a battleship game, using a hash	*/
/* table to simulate the square grid's 					*/
/* coordinates.											*/
/********************************************************/

#define COORD_MAX (UINT32_MAX)
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <inttypes.h>
#include "ships.h"
#define evalC(expr) (fputs(#expr, stdout), (printf(" = %c\n", expr)))
#define evalD(expr) (fputs(#expr, stdout), (printf(" = %d\n", expr)))
#define evalS(expr) (fputs(#expr, stdout), (printf(" = %s\n", expr)))

typedef struct position position;
typedef struct ship ship;

typedef struct node{
	position p;
	ship s;
	struct node *next;
}node;

typedef struct field{
	int ships;
	int size;
	node **table;
}field;

node *createNode(ship s, position p){
	node *n = malloc(sizeof(node));
	n->s = s;
	n->p = p;
	n->next = NULL;
	return n;
}

struct field *fieldCreate(){
	field *f = malloc(sizeof(field));
	f->ships = 0;
	f->size = sqrt(COORD_MAX);
	f->table = malloc(sizeof(node*) * f->size);
	for(int i = 0; i<f->size; i++)
		f->table[i] = NULL;
	return f;
}

//Returns struct ship with data set
ship createShip(int x, int y, int direction, int length, char name){
	ship s;
	s.topLeft.x = x;
	s.topLeft.y = y;
	s.direction = direction;
	s.name = name;
	s.length = length;
	return s;
}

unsigned int hash(position p, field *f){
	return ((p.x + p.y) * 97) % f->size;
}

void fieldDestroy(struct field *f){
	for(int i=0; i < f->size; i++){
		while(f->table[i]){
			node *temp = f->table[i];
			f->table[i] = f->table[i]->next;
			/*ship sh = temp->s;
			printf("Node (%d,%d) contains Ship '%c'| (%d,%d) | length %d | dir %d\n",
				temp->p.x, temp->p.y, sh.name,
				sh.topLeft.x, sh.topLeft.y,
				sh.length, sh.direction);*/
			free(temp);
		}
	}
	free(f->table);
	free(f);
}

//Free node at position p1 and reconnect the linked list
int delete(position p1, field *f){
	int x = hash(p1,f);
	node *prev=NULL;
	for (node *temp = f->table[x];temp!=NULL;prev=temp,temp=temp->next){
		if (temp->p.x == p1.x && temp->p.y == p1.y){
			/*printf("Delete: Node (%d,%d) of Ship '%c'| (%d,%d) | length %d | dir %d\n",
				p1.x, p1.y, temp->s.name,
				temp->s.topLeft.x, temp->s.topLeft.y,
				temp->s.length, temp->s.direction);*/

			if (prev == NULL){
				f->table[x] = f->table[x]->next;
				free(temp);
			}
			else{
				prev->next = temp->next;
				free(temp);
			}
			return 1;
		}
	}
	return 0;
}

char fieldAttack(struct field *f, struct position p1){
	ship sh;
	sh.name = NO_SHIP_NAME;
	int x = hash(p1,f);
	node *temp = f->table[x];

	for (node *n = temp; n!=NULL; n=n->next){
		if (n->p.x == p1.x && n->p.y == p1.y){
			sh = n->s;
			/*printf("Ship Found, Node (%d,%d) contains Ship '%c'| (%d,%d) | length %d | dir %d\n",
			n->p.x, n->p.y, sh.name,
			sh.topLeft.x, sh.topLeft.y,
			sh.length, sh.direction);*/
		}
	}
	if (sh.name == NO_SHIP_NAME)
		return NO_SHIP_NAME;

	position p2 = sh.topLeft;
	for (int i=0;i<sh.length;i++){
		delete(p2,f);
		if(sh.direction == HORIZONTAL)
			p2.x++;
		else
			p2.y++;
	}

	f->ships--;
	return sh.name;
}

//Helper function to see if there already exists a node
node *findNode(field *f, position p){
	int x = hash(p,f);
	for (node *n = f->table[x]; n!=NULL; n=n->next){
		if (n->p.x == p.x && n->p.y == p.y)
			return n;
	}
	return NULL;
}

//Put node into correct position in hashtable
int insertNode(field *f, node *n){
	/*printf("Node (%d,%d) of Ship '%c'| (%d,%d) | length %d | dir %d\n",
		n->p.x, n->p.y, n->s.name,
		n->s.topLeft.x, n->s.topLeft.y,
		n->s.length, n->s.direction);*/
	int x = hash(n->p,f);
	n->next = f->table[x];
	f->table[x] = n;
	return 1;
}

void fieldPlaceShip(struct field *f, struct ship s){
	double sx = s.topLeft.x;
	double sy = s.topLeft.y;
	double c = COORD_MAX;
	sx += (s.length-1) * (s.direction == HORIZONTAL);
	sy += (s.length-1) * (s.direction == VERTICAL);
	if (sy > c || sx > c)
		return;

	if (s.length <1 || s.length >MAX_SHIP_LENGTH 
		|| s.name == NO_SHIP_NAME)
		return; 

	for(int i=0; i<s.length; i++){
		node *n = malloc(sizeof(node));
		n->p = s.topLeft;
		n->p.x += i*(s.direction == HORIZONTAL);
		n->p.y += i*(s.direction == VERTICAL);
		n->s = s;
		node *temp = findNode(f, n->p);
		if (temp){
			//printf("Existing node at location!\n");
			fieldAttack(f,n->p);
		}
		insertNode(f,n);
	}
	f->ships++;
}

size_t fieldCountShips(const struct field *f){
	return f->ships;
}
