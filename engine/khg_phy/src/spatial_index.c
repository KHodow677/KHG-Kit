#include "khg_phy/phy_private.h"
#include "khg_utl/error_func.h"
#include <stdlib.h>

void
phy_spatial_index_free(phy_spatial_index *index)
{
	if(index){
		phy_spatial_index_destroy(index);
		free(index);
	}
}

phy_spatial_index *
phy_spatial_index_init(phy_spatial_index *index, phy_spatial_index_class *klass, phy_spatial_index_BB_func bbfunc, phy_spatial_index *staticIndex)
{
	index->class = klass;
	index->bbfunc = bbfunc;
	index->static_index = staticIndex;
	
	if(staticIndex){
		if (staticIndex->dynamic_index) {
      utl_error_func("This static index is already associated with a dynamic index", utl_user_defined_data);
    }
		staticIndex->dynamic_index = index;
	}
	
	return index;
}

typedef struct dynamicToStaticContext {
	phy_spatial_index_BB_func bbfunc;
	phy_spatial_index *staticIndex;
	phy_spatial_index_query_func queryFunc;
	void *data;
} dynamicToStaticContext;

static void
dynamicToStaticIter(void *obj, dynamicToStaticContext *context)
{
	phy_spatial_index_query(context->staticIndex, obj, context->bbfunc(obj), context->queryFunc, context->data);
}

void
phy_spatial_index_collide_static(phy_spatial_index *dynamicIndex, phy_spatial_index *staticIndex, phy_spatial_index_query_func func, void *data)
{
	if(staticIndex && phy_spatial_index_count(staticIndex) > 0){
		dynamicToStaticContext context = {dynamicIndex->bbfunc, staticIndex, func, data};
		phy_spatial_index_each(dynamicIndex, (phy_spatial_index_iterator_func)dynamicToStaticIter, &context);
	}
}

