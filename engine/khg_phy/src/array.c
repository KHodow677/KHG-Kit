/* Copyright (c) 2013 Scott Lembcke and Howling Moon Software
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <string.h>

#include "khg_phy/phy_private.h"


phy_array *
cpArrayNew(int size)
{
	phy_array *arr = (phy_array *)calloc(1, sizeof(phy_array));
	
	arr->num = 0;
	arr->max = (size ? size : 4);
	arr->arr = (void **)calloc(arr->max, sizeof(void*));
	
	return arr;
}

void
cpArrayFree(phy_array *arr)
{
	if(arr){
		free(arr->arr);
		arr->arr = NULL;
		
		free(arr);
	}
}

void
cpArrayPush(phy_array *arr, void *object)
{
	if(arr->num == arr->max){
		arr->max = 3*(arr->max + 1)/2;
		arr->arr = (void **)realloc(arr->arr, arr->max*sizeof(void*));
	}
	
	arr->arr[arr->num] = object;
	arr->num++;
}

void *
cpArrayPop(phy_array *arr)
{
	arr->num--;
	
	void *value = arr->arr[arr->num];
	arr->arr[arr->num] = NULL;
	
	return value;
}

void
cpArrayDeleteObj(phy_array *arr, void *obj)
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
cpArrayFreeEach(phy_array *arr, void (freeFunc)(void*))
{
	for(int i=0; i<arr->num; i++) freeFunc(arr->arr[i]);
}

bool
cpArrayContains(phy_array *arr, void *ptr)
{
	for(int i=0; i<arr->num; i++)
		if(arr->arr[i] == ptr) return true;
	
	return false;
}
