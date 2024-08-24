#include "khg_phy/phy_private.h"
#include "khg_utl/error_func.h"

void
cpSpatialIndexFree(cpSpatialIndex *index)
{
	if(index){
		cpSpatialIndexDestroy(index);
		free(index);
	}
}

cpSpatialIndex *
cpSpatialIndexInit(cpSpatialIndex *index, cpSpatialIndexClass *klass, cpSpatialIndexBBFunc bbfunc, cpSpatialIndex *staticIndex)
{
	index->klass = klass;
	index->bbfunc = bbfunc;
	index->staticIndex = staticIndex;
	
	if(staticIndex){
		if (staticIndex->dynamicIndex) {
      utl_error_func("This static index is already associated with a dynamic index", utl_user_defined_data);
    }
		staticIndex->dynamicIndex = index;
	}
	
	return index;
}

typedef struct dynamicToStaticContext {
	cpSpatialIndexBBFunc bbfunc;
	cpSpatialIndex *staticIndex;
	cpSpatialIndexQueryFunc queryFunc;
	void *data;
} dynamicToStaticContext;

static void
dynamicToStaticIter(void *obj, dynamicToStaticContext *context)
{
	cpSpatialIndexQuery(context->staticIndex, obj, context->bbfunc(obj), context->queryFunc, context->data);
}

void
cpSpatialIndexCollideStatic(cpSpatialIndex *dynamicIndex, cpSpatialIndex *staticIndex, cpSpatialIndexQueryFunc func, void *data)
{
	if(staticIndex && cpSpatialIndexCount(staticIndex) > 0){
		dynamicToStaticContext context = {dynamicIndex->bbfunc, staticIndex, func, data};
		cpSpatialIndexEach(dynamicIndex, (cpSpatialIndexIteratorFunc)dynamicToStaticIter, &context);
	}
}

