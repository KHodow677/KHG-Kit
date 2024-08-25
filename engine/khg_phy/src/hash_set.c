#include "khg_phy/phy_private.h"
#include "khg_phy/prime.h"
#include "khg_utl/error_func.h"

typedef struct cpHashSetBin {
	void *elt;
	phy_hash_value hash;
	struct cpHashSetBin *next;
} cpHashSetBin;

struct phy_hash_set {
	unsigned int entries, size;
	
	phy_hash_set_eql_func eql;
	void *default_value;
	
	cpHashSetBin **table;
	cpHashSetBin *pooledBins;
	
	phy_array *allocatedBuffers;
};

void
phy_hash_set_free(phy_hash_set *set)
{
	if(set){
		free(set->table);
		
		phy_array_free_each(set->allocatedBuffers, free);
		phy_array_free(set->allocatedBuffers);
		
		free(set);
	}
}

phy_hash_set *
cp_hash_set_new(int size, phy_hash_set_eql_func eqlFunc)
{
	phy_hash_set *set = (phy_hash_set *)calloc(1, sizeof(phy_hash_set));
	
	set->size = next_prime(size);
	set->entries = 0;
	
	set->eql = eqlFunc;
	set->default_value = NULL;
	
	set->table = (cpHashSetBin **)calloc(set->size, sizeof(cpHashSetBin *));
	set->pooledBins = NULL;
	
	set->allocatedBuffers = phy_array_new(0);
	
	return set;
}

void
phy_hash_set_set__default_value(phy_hash_set *set, void *default_value)
{
	set->default_value = default_value;
}

static int
setIsFull(phy_hash_set *set)
{
	return (set->entries >= set->size);
}

static void
cpHashSetResize(phy_hash_set *set)
{
	// Get the next approximate doubled prime.
	unsigned int newSize = next_prime(set->size + 1);
	// Allocate a new table.
	cpHashSetBin **newTable = (cpHashSetBin **)calloc(newSize, sizeof(cpHashSetBin *));
	
	// Iterate over the chains.
	for(unsigned int i=0; i<set->size; i++){
		// Rehash the bins into the new table.
		cpHashSetBin *bin = set->table[i];
		while(bin){
			cpHashSetBin *next = bin->next;
			
			phy_hash_value idx = bin->hash%newSize;
			bin->next = newTable[idx];
			newTable[idx] = bin;
			
			bin = next;
		}
	}
	
	free(set->table);
	
	set->table = newTable;
	set->size = newSize;
}

static inline void
recycleBin(phy_hash_set *set, cpHashSetBin *bin)
{
	bin->next = set->pooledBins;
	set->pooledBins = bin;
	bin->elt = NULL;
}

static cpHashSetBin *
getUnusedBin(phy_hash_set *set)
{
	cpHashSetBin *bin = set->pooledBins;
	
	if(bin){
		set->pooledBins = bin->next;
		return bin;
	} else {
		// Pool is exhausted, make more
		int count = PHY_BUFFER_BYTES/sizeof(cpHashSetBin);
		if (!count) {
      utl_error_func("Buffer size is too small", utl_user_defined_data);
    }
		
		cpHashSetBin *buffer = (cpHashSetBin *)calloc(1, PHY_BUFFER_BYTES);
		phy_array_push(set->allocatedBuffers, buffer);
		
		// push all but the first one, return it instead
		for(int i=1; i<count; i++) recycleBin(set, buffer + i);
		return buffer;
	}
}

int
phy_hash_set_count(phy_hash_set *set)
{
	return set->entries;
}

const void *
phy_hash_set_insert(phy_hash_set *set, phy_hash_value hash, const void *ptr, phy_hash_set_trans_func trans, void *data)
{
	phy_hash_value idx = hash%set->size;
	
	// Find the bin with the matching element.
	cpHashSetBin *bin = set->table[idx];
	while(bin && !set->eql(ptr, bin->elt))
		bin = bin->next;
	
	// Create it if necessary.
	if(!bin){
		bin = getUnusedBin(set);
		bin->hash = hash;
		bin->elt = (trans ? trans(ptr, data) : data);
		
		bin->next = set->table[idx];
		set->table[idx] = bin;
		
		set->entries++;
		if(setIsFull(set)) cpHashSetResize(set);
	}
	
	return bin->elt;
}

const void *
phy_hash_set_remove(phy_hash_set *set, phy_hash_value hash, const void *ptr)
{
	phy_hash_value idx = hash%set->size;
	
	cpHashSetBin **prev_ptr = &set->table[idx];
	cpHashSetBin *bin = set->table[idx];
	
	// Find the bin
	while(bin && !set->eql(ptr, bin->elt)){
		prev_ptr = &bin->next;
		bin = bin->next;
	}
	
	// Remove it if it exists.
	if(bin){
		// Update the previous linked list pointer
		(*prev_ptr) = bin->next;
		set->entries--;
		
		const void *elt = bin->elt;
		recycleBin(set, bin);
		
		return elt;
	}
	
	return NULL;
}

const void *
phy_hash_set_find(phy_hash_set *set, phy_hash_value hash, const void *ptr)
{	
	phy_hash_value idx = hash%set->size;
	cpHashSetBin *bin = set->table[idx];
	while(bin && !set->eql(ptr, bin->elt))
		bin = bin->next;
		
	return (bin ? bin->elt : set->default_value);
}

void
phy_hash_set_each(phy_hash_set *set, phy_hash_set_iterator_func func, void *data)
{
	for(unsigned int i=0; i<set->size; i++){
		cpHashSetBin *bin = set->table[i];
		while(bin){
			cpHashSetBin *next = bin->next;
			func(bin->elt, data);
			bin = next;
		}
	}
}

void
phy_hash_set_filter(phy_hash_set *set, phy_hash_set_filter_func func, void *data)
{
	for(unsigned int i=0; i<set->size; i++){
		// The rest works similarly to cpHashSetRemove() above.
		cpHashSetBin **prev_ptr = &set->table[i];
		cpHashSetBin *bin = set->table[i];
		while(bin){
			cpHashSetBin *next = bin->next;
			
			if(func(bin->elt, data)){
				prev_ptr = &bin->next;
			} else {
				(*prev_ptr) = next;

				set->entries--;
				recycleBin(set, bin);
			}
			
			bin = next;
		}
	}
}
