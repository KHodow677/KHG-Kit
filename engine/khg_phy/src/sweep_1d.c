#include "khg_phy/phy_private.h"
#include "khg_phy/spatial_index.h"
#include <stdlib.h>

static inline phy_spatial_index_class *Klass(void);

//MARK: Basic Structures

typedef struct Bounds {
	float min, max;
} Bounds;

typedef struct TableCell {
	void *obj;
	Bounds bounds;
} TableCell;

struct phy_sweep_1d
{
	phy_spatial_index spatialIndex;
	
	int num;
	int max;
	TableCell *table;
};

static inline bool
BoundsOverlap(Bounds a, Bounds b)
{
	return (a.min <= b.max && b.min <= a.max);
}

static inline Bounds
BBToBounds(phy_sweep_1d *sweep, phy_bb bb)
{
	Bounds bounds = {bb.l, bb.r};
	return bounds;
}

static inline TableCell
MakeTableCell(phy_sweep_1d *sweep, void *obj)
{
	TableCell cell = {obj, BBToBounds(sweep, sweep->spatialIndex.bbfunc(obj))};
	return cell;
}

//MARK: Memory Management Functions

phy_sweep_1d *
phy_sweep_1d_alloc(void)
{
	return (phy_sweep_1d *)calloc(1, sizeof(phy_sweep_1d));
}

static void
ResizeTable(phy_sweep_1d *sweep, int size)
{
	sweep->max = size;
	sweep->table = (TableCell *)realloc(sweep->table, size*sizeof(TableCell));
}

phy_spatial_index *
phy_sweep_1d_init(phy_sweep_1d *sweep, phy_spatial_index_BB_func bbfunc, phy_spatial_index *staticIndex)
{
	phy_spatial_index_init((phy_spatial_index *)sweep, Klass(), bbfunc, staticIndex);
	
	sweep->num = 0;
	ResizeTable(sweep, 32);
	
	return (phy_spatial_index *)sweep;
}

phy_spatial_index *
phy_sweep_1D_new(phy_spatial_index_BB_func bbfunc, phy_spatial_index *staticIndex)
{
	return phy_sweep_1d_init(phy_sweep_1d_alloc(), bbfunc, staticIndex);
}

static void
cpSweep1DDestroy(phy_sweep_1d *sweep)
{
	free(sweep->table);
	sweep->table = NULL;
}

//MARK: Misc

static int
cpSweep1DCount(phy_sweep_1d *sweep)
{
	return sweep->num;
}

static void
cpSweep1DEach(phy_sweep_1d *sweep, phy_spatial_index_iterator_func func, void *data)
{
	TableCell *table = sweep->table;
	for(int i=0, count=sweep->num; i<count; i++) func(table[i].obj, data);
}

static int
cpSweep1DContains(phy_sweep_1d *sweep, void *obj, phy_hash_value hashid)
{
	TableCell *table = sweep->table;
	for(int i=0, count=sweep->num; i<count; i++){
		if(table[i].obj == obj) return true;
	}
	
	return false;
}

//MARK: Basic Operations

static void
cpSweep1DInsert(phy_sweep_1d *sweep, void *obj, phy_hash_value hashid)
{
	if(sweep->num == sweep->max) ResizeTable(sweep, sweep->max*2);
	
	sweep->table[sweep->num] = MakeTableCell(sweep, obj);
	sweep->num++;
}

static void
cpSweep1DRemove(phy_sweep_1d *sweep, void *obj, phy_hash_value hashid)
{
	TableCell *table = sweep->table;
	for(int i=0, count=sweep->num; i<count; i++){
		if(table[i].obj == obj){
			int num = --sweep->num;
			
			table[i] = table[num];
			table[num].obj = NULL;
			
			return;
		}
	}
}

//MARK: Reindexing Functions

static void
cpSweep1DReindexObject(phy_sweep_1d *sweep, void *obj, phy_hash_value hashid)
{
	// Nothing to do here
}

static void
cpSweep1DReindex(phy_sweep_1d *sweep)
{
	// Nothing to do here
	// Could perform a sort, but queries are not accelerated anyway.
}

//MARK: Query Functions

static void
cpSweep1DQuery(phy_sweep_1d *sweep, void *obj, phy_bb bb, phy_spatial_index_query_func func, void *data)
{
	// Implementing binary search here would allow you to find an upper limit
	// but not a lower limit. Probably not worth the hassle.
	
	Bounds bounds = BBToBounds(sweep, bb);
	
	TableCell *table = sweep->table;
	for(int i=0, count=sweep->num; i<count; i++){
		TableCell cell = table[i];
		if(BoundsOverlap(bounds, cell.bounds) && obj != cell.obj) func(obj, cell.obj, 0, data);
	}
}

static void
cpSweep1DSegmentQuery(phy_sweep_1d *sweep, void *obj, phy_vect a, phy_vect b, float t_exit, phy_spatial_index_segment_query_func func, void *data)
{
	phy_bb bb = phy_bb_expand(phy_bb_new(a.x, a.y, a.x, a.y), b);
	Bounds bounds = BBToBounds(sweep, bb);
	
	TableCell *table = sweep->table;
	for(int i=0, count=sweep->num; i<count; i++){
		TableCell cell = table[i];
		if(BoundsOverlap(bounds, cell.bounds)) func(obj, cell.obj, data);
	}
}

//MARK: Reindex/Query

static int
TableSort(TableCell *a, TableCell *b)
{
	return (a->bounds.min < b->bounds.min ? -1 : (a->bounds.min > b->bounds.min ? 1 : 0));
}

static void
cpSweep1DReindexQuery(phy_sweep_1d *sweep, phy_spatial_index_query_func func, void *data)
{
	TableCell *table = sweep->table;
	int count = sweep->num;
	
	// Update bounds and sort
	for(int i=0; i<count; i++) table[i] = MakeTableCell(sweep, table[i].obj);
	qsort(table, count, sizeof(TableCell), (int (*)(const void *, const void *))TableSort); // TODO: use insertion sort instead
	
	for(int i=0; i<count; i++){
		TableCell cell = table[i];
		float max = cell.bounds.max;
		
		for(int j=i+1; table[j].bounds.min < max && j<count; j++){
			func(cell.obj, table[j].obj, 0, data);
		}
	}
	
	// Reindex query is also responsible for colliding against the static index.
	// Fortunately there is a helper function for that.
	phy_spatial_index_collide_static((phy_spatial_index *)sweep, sweep->spatialIndex.static_index, func, data);
}

static phy_spatial_index_class klass = {
	(phy_spatial_index_destroy_impl)cpSweep1DDestroy,
	
	(phy_spatial_index_count_impl)cpSweep1DCount,
	(phy_spatial_index_each_impl)cpSweep1DEach,
	(phy_spatial_index_contains_impl)cpSweep1DContains,
	
	(phy_spatial_index_insert_impl)cpSweep1DInsert,
	(phy_spatial_index_remove_impl)cpSweep1DRemove,
	
	(phy_spatial_index_reindex_impl)cpSweep1DReindex,
	(phy_spatial_index_reindex_object_impl)cpSweep1DReindexObject,
	(phy_spatial_index_reindex_query_impl)cpSweep1DReindexQuery,
	
	(phy_spatial_index_query_impl)cpSweep1DQuery,
	(phy_spatial_index_segment_query_impl)cpSweep1DSegmentQuery,
};

static inline phy_spatial_index_class *Klass(){return &klass;}

