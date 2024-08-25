#include "khg_phy/phy_private.h"
#include <stdlib.h>
#include <string.h>


phy_array *
phy_array_new(int size)
{
	phy_array *arr = (phy_array *)calloc(1, sizeof(phy_array));
	
	arr->num = 0;
	arr->max = (size ? size : 4);
	arr->arr = (void **)calloc(arr->max, sizeof(void*));
	
	return arr;
}

void
phy_array_free(phy_array *arr)
{
	if(arr){
		free(arr->arr);
		arr->arr = NULL;
		
		free(arr);
	}
}

void
phy_array_push(phy_array *arr, void *object)
{
	if(arr->num == arr->max){
		arr->max = 3*(arr->max + 1)/2;
		arr->arr = (void **)realloc(arr->arr, arr->max*sizeof(void*));
	}
	
	arr->arr[arr->num] = object;
	arr->num++;
}

void *
phy_array_pop(phy_array *arr)
{
	arr->num--;
	
	void *value = arr->arr[arr->num];
	arr->arr[arr->num] = NULL;
	
	return value;
}

void
phy_array_delete_obj(phy_array *arr, void *obj)
{
	for(int i=0; i<arr->num; i++){
		if(arr->arr[i] == obj){
			arr->num--;
			
			arr->arr[i] = arr->arr[arr->num];
			arr->arr[arr->num] = NULL;
			
			return;
		}
	}
}

void
phy_array_free_each(phy_array *arr, void (freeFunc)(void*))
{
	for(int i=0; i<arr->num; i++) freeFunc(arr->arr[i]);
}

bool
phy_array_contains(phy_array *arr, void *ptr)
{
	for(int i=0; i<arr->num; i++)
		if(arr->arr[i] == ptr) return true;
	
	return false;
}
