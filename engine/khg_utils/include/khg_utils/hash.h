#pragma once

#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

#define DECLTYPE_ASSIGN(dst,src) {\
  (dst) = DECLTYPE(dst)(src);\
}

#ifndef HASH_FUNCTION
#define HASH_FUNCTION(keyptr,keylen,hashv) HASH_JEN(keyptr, keylen, hashv)
#endif

#ifndef HASH_KEYCMP
#define HASH_KEYCMP(a,b,n) memcmp(a,b,n)
#endif

#ifndef HASH_NONFATAL_OOM
#define HASH_NONFATAL_OOM 0
#endif

#define HASH_RECORD_OOM(oomed) exit(-1)
#define IF_HASH_NONFATAL_OOM(x)

#define HASH_INITIAL_NUM_BUCKETS 32U
#define HASH_INITIAL_NUM_BUCKETS_LOG2 5U
#define HASH_BKT_CAPACITY_THRESH 10U

#define ELMT_FROM_HH(tbl,hhp) ((void*)(((char*)(hhp)) - ((tbl)->hho)))
#define HH_FROM_ELMT(tbl,elp) ((hash_handle*)(void*)(((char*)(elp)) + ((tbl)->hho)))

#define HASH_ROLLBACK_BKT(hh, head, itemptrhh) {\
  struct hash_handle *_hd_hh_item = (itemptrhh);\
  unsigned _hd_bkt;\
  HASH_TO_BKT(_hd_hh_item->hashv, (head)->hh.tbl->num_buckets, _hd_bkt);\
  (head)->hh.tbl->buckets[_hd_bkt].count++;\
  _hd_hh_item->hh_next = NULL;\
  _hd_hh_item->hh_prev = NULL;\
}

#define HASH_VALUE(keyptr,keylen,hashv) {\
  HASH_FUNCTION(keyptr, keylen, hashv);\
}

#define HASH_FIND_BYHASHVALUE(hh,head,keyptr,keylen,hashval,out) {\
  (out) = NULL;\
  if (head) {\
    unsigned _hf_bkt;\
    HASH_TO_BKT(hashval, (head)->hh.tbl->num_buckets, _hf_bkt);\
    if (HASH_BLOOM_TEST((head)->hh.tbl, hashval)) {\
      HASH_FIND_IN_BKT((head)->hh.tbl, hh, (head)->hh.tbl->buckets[ _hf_bkt ], keyptr, keylen, hashval, out);\
    }\
  }\
}

#define HASH_FIND(hh,head,keyptr,keylen,out) {\
  (out) = NULL;\
  if (head) {\
    unsigned _hf_hashv;\
    HASH_VALUE(keyptr, keylen, _hf_hashv);\
    HASH_FIND_BYHASHVALUE(hh, head, keyptr, keylen, _hf_hashv, out);\
  }\
}

#define HASH_BLOOM_MAKE(tbl,oomed)
#define HASH_BLOOM_FREE(tbl)
#define HASH_BLOOM_ADD(tbl,hashv)
#define HASH_BLOOM_TEST(tbl,hashv) 1
#define HASH_BLOOM_BYTELEN 0U

#define HASH_MAKE_TABLE(hh,head,oomed) {\
  (head)->hh.tbl = (hash_table*)malloc(sizeof(hash_table));\
  if (!(head)->hh.tbl) {\
    HASH_RECORD_OOM(oomed);\
  }\
  else {\
    memset((head)->hh.tbl,'\0', sizeof(hash_table));\
    (head)->hh.tbl->tail = &((head)->hh);\
    (head)->hh.tbl->num_buckets = HASH_INITIAL_NUM_BUCKETS;\
    (head)->hh.tbl->log2_num_buckets = HASH_INITIAL_NUM_BUCKETS_LOG2;\
    (head)->hh.tbl->hho = (char*)(&(head)->hh) - (char*)(head);\
    (head)->hh.tbl->buckets = (hash_bucket*)malloc(HASH_INITIAL_NUM_BUCKETS * sizeof(struct hash_bucket));\
    (head)->hh.tbl->signature = HASH_SIGNATURE;\
    if (!(head)->hh.tbl->buckets) {\
      HASH_RECORD_OOM(oomed);\
      free((head)->hh.tbl, sizeof(hash_table));\
    }\
    else {\
      memset((head)->hh.tbl->buckets, '\0', HASH_INITIAL_NUM_BUCKETS * sizeof(struct hash_bucket));\
      HASH_BLOOM_MAKE((head)->hh.tbl, oomed);\
      IF_HASH_NONFATAL_OOM(\
        if (oomed) {\
          free((head)->hh.tbl->buckets, HASH_INITIAL_NUM_BUCKETS*sizeof(struct hash_bucket));\
          free((head)->hh.tbl, sizeof(hash_table));\
        }\
      )\
    }\
  }\
}

#define HASH_REPLACE_BYHASHVALUE_INORDER(hh,head,fieldname,keylen_in,hashval,add,replaced,cmpfcn) {\
  (replaced) = NULL;\
  HASH_FIND_BYHASHVALUE(hh, head, &((add)->fieldname), keylen_in, hashval, replaced);\
  if (replaced) {\
    HASH_DELETE(hh, head, replaced);\
  }\
  HASH_ADD_KEYPTR_BYHASHVALUE_INORDER(hh, head, &((add)->fieldname), keylen_in, hashval, add, cmpfcn);\
}

#define HASH_REPLACE_BYHASHVALUE(hh,head,fieldname,keylen_in,hashval,add,replaced) {\
  (replaced) = NULL;\
  HASH_FIND_BYHASHVALUE(hh, head, &((add)->fieldname), keylen_in, hashval, replaced);\
  if (replaced) {\
    HASH_DELETE(hh, head, replaced);\
  }\
  HASH_ADD_KEYPTR_BYHASHVALUE(hh, head, &((add)->fieldname), keylen_in, hashval, add);\
}

#define HASH_REPLACE(hh,head,fieldname,keylen_in,add,replaced) {\
  unsigned _hr_hashv;\
  HASH_VALUE(&((add)->fieldname), keylen_in, _hr_hashv);\
  HASH_REPLACE_BYHASHVALUE(hh, head, fieldname, keylen_in, _hr_hashv, add, replaced);\
}

#define HASH_REPLACE_INORDER(hh,head,fieldname,keylen_in,add,replaced,cmpfcn) {\
  unsigned _hr_hashv;\
  HASH_VALUE(&((add)->fieldname), keylen_in, _hr_hashv);\
  HASH_REPLACE_BYHASHVALUE_INORDER(hh, head, fieldname, keylen_in, _hr_hashv, add, replaced, cmpfcn); \
}

#define HASH_APPEND_LIST(hh, head, add) {\
  (add)->hh.next = NULL;\
  (add)->hh.prev = ELMT_FROM_HH((head)->hh.tbl, (head)->hh.tbl->tail);\
  (head)->hh.tbl->tail->next = (add);\
  (head)->hh.tbl->tail = &((add)->hh);\
}

#define HASH_AKBI_INNER_LOOP(hh,head,add,cmpfcn) {\
  do {\
    if (cmpfcn(DECLTYPE(head)(_hs_iter), add) > 0) {\
      break;\
    }\
  } while ((_hs_iter = HH_FROM_ELMT((head)->hh.tbl, _hs_iter)->next));\
}

#define HASH_ADD_TO_TABLE(hh,head,keyptr,keylen_in,hashval,add,oomed) {\
  unsigned _ha_bkt;\
  (head)->hh.tbl->num_items++;\
  HASH_TO_BKT(hashval, (head)->hh.tbl->num_buckets, _ha_bkt);\
  HASH_ADD_TO_BKT((head)->hh.tbl->buckets[_ha_bkt], hh, &(add)->hh, oomed);\
  HASH_BLOOM_ADD((head)->hh.tbl, hashval);\
  HASH_EMIT_KEY(hh, head, keyptr, keylen_in);\
}

#define HASH_ADD_KEYPTR_BYHASHVALUE_INORDER(hh,head,keyptr,keylen_in,hashval,add,cmpfcn) {\
  IF_HASH_NONFATAL_OOM( int _ha_oomed = 0; )\
  (add)->hh.hashv = (hashval);\
  (add)->hh.key = (char*) (keyptr);\
  (add)->hh.keylen = (unsigned) (keylen_in);\
  if (!(head)) {\
    (add)->hh.next = NULL;\
    (add)->hh.prev = NULL;\
    HASH_MAKE_TABLE(hh, add, _ha_oomed);\
    IF_HASH_NONFATAL_OOM( if (!_ha_oomed) { )\
      (head) = (add);\
    IF_HASH_NONFATAL_OOM( } )\
  }\
  else {\
    void *_hs_iter = (head);\
    (add)->hh.tbl = (head)->hh.tbl;\
    HASH_AKBI_INNER_LOOP(hh, head, add, cmpfcn);\
    if (_hs_iter) {\
      (add)->hh.next = _hs_iter;\
      if (((add)->hh.prev = HH_FROM_ELMT((head)->hh.tbl, _hs_iter)->prev)) {\
        HH_FROM_ELMT((head)->hh.tbl, (add)->hh.prev)->next = (add);\
      }\
      else {\
        (head) = (add);\
      }\
      HH_FROM_ELMT((head)->hh.tbl, _hs_iter)->prev = (add);\
    }\
    else {\
      HASH_APPEND_LIST(hh, head, add);\
    }\
  }\
  HASH_ADD_TO_TABLE(hh, head, keyptr, keylen_in, hashval, add, _ha_oomed);\
  HASH_FSCK(hh, head, "HASH_ADD_KEYPTR_BYHASHVALUE_INORDER");\
}

#define HASH_ADD_KEYPTR_INORDER(hh,head,keyptr,keylen_in,add,cmpfcn) {\
  unsigned _hs_hashv;\
  HASH_VALUE(keyptr, keylen_in, _hs_hashv);\
  HASH_ADD_KEYPTR_BYHASHVALUE_INORDER(hh, head, keyptr, keylen_in, _hs_hashv, add, cmpfcn);\
}

#define HASH_ADD_BYHASHVALUE_INORDER(hh,head,fieldname,keylen_in,hashval,add,cmpfcn) HASH_ADD_KEYPTR_BYHASHVALUE_INORDER(hh, head, &((add)->fieldname), keylen_in, hashval, add, cmpfcn)

#define HASH_ADD_INORDER(hh,head,fieldname,keylen_in,add,cmpfcn) HASH_ADD_KEYPTR_INORDER(hh, head, &((add)->fieldname), keylen_in, add, cmpfcn)

#define HASH_ADD_KEYPTR_BYHASHVALUE(hh,head,keyptr,keylen_in,hashval,add) {\
  IF_HASH_NONFATAL_OOM( int _ha_oomed = 0; )\
  (add)->hh.hashv = (hashval);\
  (add)->hh.key = (const void*) (keyptr);\
  (add)->hh.keylen = (unsigned) (keylen_in);\
  if (!(head)) {\
    (add)->hh.next = NULL;\
    (add)->hh.prev = NULL;\
    HASH_MAKE_TABLE(hh, add, _ha_oomed);\
    IF_HASH_NONFATAL_OOM( if (!_ha_oomed) { )\
      (head) = (add);\
    IF_HASH_NONFATAL_OOM( } )\
  } else {\
    (add)->hh.tbl = (head)->hh.tbl;\
    HASH_APPEND_LIST(hh, head, add);\
  }\
  HASH_ADD_TO_TABLE(hh, head, keyptr, keylen_in, hashval, add, _ha_oomed);\
  HASH_FSCK(hh, head, "HASH_ADD_KEYPTR_BYHASHVALUE");\
}

#define HASH_ADD_KEYPTR(hh,head,keyptr,keylen_in,add) {\
  unsigned _ha_hashv;\
  HASH_VALUE(keyptr, keylen_in, _ha_hashv);\
  HASH_ADD_KEYPTR_BYHASHVALUE(hh, head, keyptr, keylen_in, _ha_hashv, add);\
}

#define HASH_ADD_BYHASHVALUE(hh,head,fieldname,keylen_in,hashval,add) HASH_ADD_KEYPTR_BYHASHVALUE(hh, head, &((add)->fieldname), keylen_in, hashval, add)

#define HASH_ADD(hh,head,fieldname,keylen_in,add) HASH_ADD_KEYPTR(hh, head, &((add)->fieldname), keylen_in, add)

#define HASH_TO_BKT(hashv,num_bkts,bkt) {\
  bkt = ((hashv) & ((num_bkts) - 1U));\
}

#define HASH_DELETE(hh,head,delptr) HASH_DELETE_HH(hh, head, &(delptr)->hh)

#define HASH_DELETE_HH(hh,head,delptrhh) {\
  const struct hash_handle *_hd_hh_del = (delptrhh);\
  if ((_hd_hh_del->prev == NULL) && (_hd_hh_del->next == NULL)) {\
    HASH_BLOOM_FREE((head)->hh.tbl);\
    free((head)->hh.tbl->buckets,\
         (head)->hh.tbl->num_buckets * sizeof(struct hash_bucket));\
    free((head)->hh.tbl, sizeof(hash_table));\
    (head) = NULL;\
  } else {\
    unsigned _hd_bkt;\
    if (_hd_hh_del == (head)->hh.tbl->tail) {\
      (head)->hh.tbl->tail = HH_FROM_ELMT((head)->hh.tbl, _hd_hh_del->prev);\
    }\
    if (_hd_hh_del->prev != NULL) {\
      HH_FROM_ELMT((head)->hh.tbl, _hd_hh_del->prev)->next = _hd_hh_del->next;\
    } else {\
      DECLTYPE_ASSIGN(head, _hd_hh_del->next);\
    }\
    if (_hd_hh_del->next != NULL) {\
      HH_FROM_ELMT((head)->hh.tbl, _hd_hh_del->next)->prev = _hd_hh_del->prev;\
    }\
    HASH_TO_BKT(_hd_hh_del->hashv, (head)->hh.tbl->num_buckets, _hd_bkt);\
    HASH_DEL_IN_BKT((head)->hh.tbl->buckets[_hd_bkt], _hd_hh_del);\
    (head)->hh.tbl->num_items--;\
  }\
  HASH_FSCK(hh, head, "HASH_DELETE_HH");\
}

#define HASH_FIND_STR(head,findstr,out) {\
    unsigned _hash_hfstr_keylen = (unsigned)strlen(findstr);\
    HASH_FIND(hh, head, findstr, _hash_hfstr_keylen, out);\
}

#define HASH_ADD_STR(head,strfield,add) {\
    unsigned _hash_hastr_keylen = (unsigned)strlen((add)->strfield);\
    HASH_ADD(hh, head, strfield[0], _hash_hastr_keylen, add);\
}

#define HASH_REPLACE_STR(head,strfield,add,replaced) {\
    unsigned _hash_hrstr_keylen = (unsigned)strlen((add)->strfield);\
    HASH_REPLACE(hh, head, strfield[0], _hash_hrstr_keylen, add, replaced);\
}

#define HASH_FIND_INT(head,findint,out) HASH_FIND(hh,head,findint,sizeof(int),out)
#define HASH_ADD_INT(head,intfield,add) HASH_ADD(hh,head,intfield,sizeof(int),add)
#define HASH_REPLACE_INT(head,intfield,add,replaced) HASH_REPLACE(hh,head,intfield,sizeof(int),add,replaced)
#define HASH_FIND_PTR(head,findptr,out) HASH_FIND(hh,head,findptr,sizeof(void *),out)
#define HASH_ADD_PTR(head,ptrfield,add) HASH_ADD(hh,head,ptrfield,sizeof(void *),add)
#define HASH_REPLACE_PTR(head,ptrfield,add,replaced) HASH_REPLACE(hh,head,ptrfield,sizeof(void *),add,replaced)
#define HASH_DEL(head,delptr) HASH_DELETE(hh,head,delptr)

#define HASH_FSCK(hh,head,where)

#define HASH_EMIT_KEY(hh,head,keyptr,fieldlen)

#define HASH_BER(key,keylen,hashv) {\
  unsigned _hb_keylen = (unsigned)keylen;\
  const unsigned char *_hb_key = (const unsigned char*)(key);\
  (hashv) = 0;\
  while (_hb_keylen-- != 0U) {\
    (hashv) = (((hashv) << 5) + (hashv)) + *_hb_key++;\
  }\
}

#define HASH_SAX(key,keylen,hashv) {\
  unsigned _sx_i;\
  const unsigned char *_hs_key = (const unsigned char*)(key);\
  hashv = 0;\
  for (_sx_i=0; _sx_i < keylen; _sx_i++) {\
    hashv ^= (hashv << 5) + (hashv >> 2) + _hs_key[_sx_i];\
  }\
}

#define HASH_FNV(key,keylen,hashv) {\
  unsigned _fn_i;\
  const unsigned char *_hf_key = (const unsigned char*)(key);\
  (hashv) = 2166136261U;\
  for (_fn_i=0; _fn_i < keylen; _fn_i++) {\
    hashv = hashv ^ _hf_key[_fn_i];\
    hashv = hashv * 16777619U;\
  }\
}

#define HASH_OAT(key,keylen,hashv) {\
  unsigned _ho_i;\
  const unsigned char *_ho_key=(const unsigned char*)(key);\
  hashv = 0;\
  for(_ho_i=0; _ho_i < keylen; _ho_i++) {\
      hashv += _ho_key[_ho_i];\
      hashv += (hashv << 10);\
      hashv ^= (hashv >> 6);\
  }\
  hashv += (hashv << 3);\
  hashv ^= (hashv >> 11);\
  hashv += (hashv << 15);\
}

#define HASH_JEN_MIX(a,b,c) {\
  a -= b; a -= c; a ^= ( c >> 13 );\
  b -= c; b -= a; b ^= ( a << 8 );\
  c -= a; c -= b; c ^= ( b >> 13 );\
  a -= b; a -= c; a ^= ( c >> 12 );\
  b -= c; b -= a; b ^= ( a << 16 );\
  c -= a; c -= b; c ^= ( b >> 5 );\
  a -= b; a -= c; a ^= ( c >> 3 );\
  b -= c; b -= a; b ^= ( a << 10 );\
  c -= a; c -= b; c ^= ( b >> 15 );\
}

#define HASH_JEN(key,keylen,hashv) {\
  unsigned _hj_i,_hj_j,_hj_k;\
  unsigned const char *_hj_key=(unsigned const char*)(key);\
  hashv = 0xfeedbeefu;\
  _hj_i = _hj_j = 0x9e3779b9u;\
  _hj_k = (unsigned)(keylen);\
  while (_hj_k >= 12U) {\
    _hj_i +=    (_hj_key[0] + ( (unsigned)_hj_key[1] << 8 )\
        + ( (unsigned)_hj_key[2] << 16 )\
        + ( (unsigned)_hj_key[3] << 24 ) );\
    _hj_j +=    (_hj_key[4] + ( (unsigned)_hj_key[5] << 8 )\
        + ( (unsigned)_hj_key[6] << 16 )\
        + ( (unsigned)_hj_key[7] << 24 ) );\
    hashv += (_hj_key[8] + ( (unsigned)_hj_key[9] << 8 )\
        + ( (unsigned)_hj_key[10] << 16 )\
        + ( (unsigned)_hj_key[11] << 24 ) );\
     HASH_JEN_MIX(_hj_i, _hj_j, hashv);\
     _hj_key += 12;\
     _hj_k -= 12U;\
  }\
  hashv += (unsigned)(keylen);\
  switch ( _hj_k ) {\
    case 11: hashv += ( (unsigned)_hj_key[10] << 24 );\
    case 10: hashv += ( (unsigned)_hj_key[9] << 16 );\
    case 9:  hashv += ( (unsigned)_hj_key[8] << 8 );\
    case 8:  _hj_j += ( (unsigned)_hj_key[7] << 24 );\
    case 7:  _hj_j += ( (unsigned)_hj_key[6] << 16 );\
    case 6:  _hj_j += ( (unsigned)_hj_key[5] << 8 );\
    case 5:  _hj_j += _hj_key[4];\
    case 4:  _hj_i += ( (unsigned)_hj_key[3] << 24 );\
    case 3:  _hj_i += ( (unsigned)_hj_key[2] << 16 );\
    case 2:  _hj_i += ( (unsigned)_hj_key[1] << 8 );\
    case 1:  _hj_i += _hj_key[0];\
    default: break;\
  }\
  HASH_JEN_MIX(_hj_i, _hj_j, hashv);\
}

#undef get16bits
#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8) +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif

#define HASH_SFH(key,keylen,hashv) {\
  unsigned const char *_sfh_key=(unsigned const char*)(key);\
  uint32_t _sfh_tmp, _sfh_len = (uint32_t)keylen;\
  unsigned _sfh_rem = _sfh_len & 3U;\
  _sfh_len >>= 2;\
  hashv = 0xcafebabeu;\
  for (;_sfh_len > 0U; _sfh_len--) {\
    hashv    += get16bits (_sfh_key);\
    _sfh_tmp  = ((uint32_t)(get16bits (_sfh_key+2)) << 11) ^ hashv;\
    hashv     = (hashv << 16) ^ _sfh_tmp;\
    _sfh_key += 2U*sizeof (uint16_t);\
    hashv    += hashv >> 11;\
  }\
  switch (_sfh_rem) {\
    case 3:\
      hashv += get16bits (_sfh_key);\
      hashv ^= hashv << 16;\
      hashv ^= (uint32_t)(_sfh_key[sizeof (uint16_t)]) << 18;\
      hashv += hashv >> 11;\
      break;\
    case 2:\
      hashv += get16bits (_sfh_key);\
      hashv ^= hashv << 11;\
      hashv += hashv >> 17;\
      break;\
    case 1:\
      hashv += *_sfh_key;\
      hashv ^= hashv << 10;\
      hashv += hashv >> 1;\
      break;\
    default:\
      break;\
  }\
  hashv ^= hashv << 3;\
  hashv += hashv >> 5;\
  hashv ^= hashv << 4;\
  hashv += hashv >> 17;\
  hashv ^= hashv << 25;\
  hashv += hashv >> 6;\
}


#define HASH_FIND_IN_BKT(tbl,hh,head,keyptr,keylen_in,hashval,out) {\
  if ((head).hh_head != NULL) {\
    DECLTYPE_ASSIGN(out, ELMT_FROM_HH(tbl, (head).hh_head));\
  } else {\
    (out) = NULL;\
  }\
  while ((out) != NULL) {\
    if ((out)->hh.hashv == (hashval) && (out)->hh.keylen == (keylen_in)) {\
      if (HASH_KEYCMP((out)->hh.key, keyptr, keylen_in) == 0) {\
        break;\
      }\
    }\
    if ((out)->hh.hh_next != NULL) {\
      DECLTYPE_ASSIGN(out, ELMT_FROM_HH(tbl, (out)->hh.hh_next));\
    } else {\
      (out) = NULL;\
    }\
  }\
}

#define HASH_ADD_TO_BKT(head,hh,addhh,oomed) {\
  hash_bucket *_ha_head = &(head);\
  _ha_head->count++;\
  (addhh)->hh_next = _ha_head->hh_head;\
  (addhh)->hh_prev = NULL;\
  if (_ha_head->hh_head != NULL) {\
    _ha_head->hh_head->hh_prev = (addhh);\
  }\
  _ha_head->hh_head = (addhh);\
  if ((_ha_head->count >= ((_ha_head->expand_mult + 1U) * HASH_BKT_CAPACITY_THRESH)) && !(addhh)->tbl->noexpand) {\
    HASH_EXPAND_BUCKETS(addhh,(addhh)->tbl, oomed);\
    IF_HASH_NONFATAL_OOM(\
      if (oomed) {\
        HASH_DEL_IN_BKT(head,addhh);\
      }\
    )\
  }\
}

#define HASH_DEL_IN_BKT(head,delhh) {\
  hash_bucket *_hd_head = &(head);\
  _hd_head->count--;\
  if (_hd_head->hh_head == (delhh)) {\
    _hd_head->hh_head = (delhh)->hh_next;\
  }\
  if ((delhh)->hh_prev) {\
    (delhh)->hh_prev->hh_next = (delhh)->hh_next;\
  }\
  if ((delhh)->hh_next) {\
    (delhh)->hh_next->hh_prev = (delhh)->hh_prev;\
  }\
}

#define HASH_EXPAND_BUCKETS(hh,tbl,oomed) {\
  unsigned _he_bkt;\
  unsigned _he_bkt_i;\
  struct hash_handle *_he_thh, *_he_hh_nxt;\
  hash_bucket *_he_new_buckets, *_he_newbkt;\
  _he_new_buckets = (hash_bucket*)malloc(sizeof(struct hash_bucket) * (tbl)->num_buckets * 2U);\
  if (!_he_new_buckets) {\
    HASH_RECORD_OOM(oomed);\
  } else {\
    memset(_he_new_buckets, '\0', sizeof(struct hash_bucket) * (tbl)->num_buckets * 2U);\
    (tbl)->ideal_chain_maxlen = ((tbl)->num_items >> ((tbl)->log2_num_buckets+1U)) + ((((tbl)->num_items & (((tbl)->num_buckets*2U)-1U)) != 0U) ? 1U : 0U);\
    (tbl)->nonideal_items = 0;\
    for (_he_bkt_i = 0; _he_bkt_i < (tbl)->num_buckets; _he_bkt_i++) {\
      _he_thh = (tbl)->buckets[ _he_bkt_i ].hh_head;\
      while (_he_thh != NULL) {\
        _he_hh_nxt = _he_thh->hh_next;\
        HASH_TO_BKT(_he_thh->hashv, (tbl)->num_buckets * 2U, _he_bkt);\
        _he_newbkt = &(_he_new_buckets[_he_bkt]);\
        if (++(_he_newbkt->count) > (tbl)->ideal_chain_maxlen) {\
          (tbl)->nonideal_items++;\
          if (_he_newbkt->count > _he_newbkt->expand_mult * (tbl)->ideal_chain_maxlen) {\
            _he_newbkt->expand_mult++;\
          }\
        }\
        _he_thh->hh_prev = NULL;\
        _he_thh->hh_next = _he_newbkt->hh_head;\
        if (_he_newbkt->hh_head != NULL) {\
          _he_newbkt->hh_head->hh_prev = _he_thh;\
        }\
        _he_newbkt->hh_head = _he_thh;\
        _he_thh = _he_hh_nxt;\
      }\
    }\
    free((tbl)->buckets, (tbl)->num_buckets * sizeof(struct hash_bucket));\
    (tbl)->num_buckets *= 2U;\
    (tbl)->log2_num_buckets++;\
    (tbl)->buckets = _he_new_buckets;\
    (tbl)->ineff_expands = ((tbl)->nonideal_items > ((tbl)->num_items >> 1)) ? ((tbl)->ineff_expands+1U) : 0U;\
    if ((tbl)->ineff_expands > 1U) {\
      (tbl)->noexpand = 1;\
    }\
  }\
}

#define HASH_SORT(head,cmpfcn) HASH_SRT(hh,head,cmpfcn)
#define HASH_SRT(hh,head,cmpfcn) {\
  unsigned _hs_i;\
  unsigned _hs_looping,_hs_nmerges,_hs_insize,_hs_psize,_hs_qsize;\
  struct hash_handle *_hs_p, *_hs_q, *_hs_e, *_hs_list, *_hs_tail;\
  if (head != NULL) {\
    _hs_insize = 1;\
    _hs_looping = 1;\
    _hs_list = &((head)->hh);\
    while (_hs_looping != 0U) {\
      _hs_p = _hs_list;\
      _hs_list = NULL;\
      _hs_tail = NULL;\
      _hs_nmerges = 0;\
      while (_hs_p != NULL) {\
        _hs_nmerges++;\
        _hs_q = _hs_p;\
        _hs_psize = 0;\
        for (_hs_i = 0; _hs_i < _hs_insize; ++_hs_i) {\
          _hs_psize++;\
          _hs_q = ((_hs_q->next != NULL) ?\
            HH_FROM_ELMT((head)->hh.tbl, _hs_q->next) : NULL);\
          if (_hs_q == NULL) {\
            break;\
          }\
        }\
        _hs_qsize = _hs_insize;\
        while ((_hs_psize != 0U) || ((_hs_qsize != 0U) && (_hs_q != NULL))) {\
          if (_hs_psize == 0U) {\
            _hs_e = _hs_q;\
            _hs_q = ((_hs_q->next != NULL) ?\
              HH_FROM_ELMT((head)->hh.tbl, _hs_q->next) : NULL);\
            _hs_qsize--;\
          } else if ((_hs_qsize == 0U) || (_hs_q == NULL)) {\
            _hs_e = _hs_p;\
            if (_hs_p != NULL) {\
              _hs_p = ((_hs_p->next != NULL) ?\
                HH_FROM_ELMT((head)->hh.tbl, _hs_p->next) : NULL);\
            }\
            _hs_psize--;\
          } else if ((cmpfcn(\
                DECLTYPE(head)(ELMT_FROM_HH((head)->hh.tbl, _hs_p)),\
                DECLTYPE(head)(ELMT_FROM_HH((head)->hh.tbl, _hs_q))\
                )) <= 0) {\
            _hs_e = _hs_p;\
            if (_hs_p != NULL) {\
              _hs_p = ((_hs_p->next != NULL) ?\
                HH_FROM_ELMT((head)->hh.tbl, _hs_p->next) : NULL);\
            }\
            _hs_psize--;\
          } else {\
            _hs_e = _hs_q;\
            _hs_q = ((_hs_q->next != NULL) ?\
              HH_FROM_ELMT((head)->hh.tbl, _hs_q->next) : NULL);\
            _hs_qsize--;\
          }\
          if ( _hs_tail != NULL ) {\
            _hs_tail->next = ((_hs_e != NULL) ?\
              ELMT_FROM_HH((head)->hh.tbl, _hs_e) : NULL);\
          } else {\
            _hs_list = _hs_e;\
          }\
          if (_hs_e != NULL) {\
            _hs_e->prev = ((_hs_tail != NULL) ?\
              ELMT_FROM_HH((head)->hh.tbl, _hs_tail) : NULL);\
          }\
          _hs_tail = _hs_e;\
        }\
        _hs_p = _hs_q;\
      }\
      if (_hs_tail != NULL) {\
        _hs_tail->next = NULL;\
      }\
      if (_hs_nmerges <= 1U) {\
        _hs_looping = 0;\
        (head)->hh.tbl->tail = _hs_tail;\
        DECLTYPE_ASSIGN(head, ELMT_FROM_HH((head)->hh.tbl, _hs_list));\
      }\
      _hs_insize *= 2U;\
    }\
    HASH_FSCK(hh, head, "HASH_SRT");\
  }\
}

#define HASH_SELECT(hh_dst, dst, hh_src, src, cond) \
do {\
  unsigned _src_bkt, _dst_bkt;\
  void *_last_elt = NULL, *_elt;\
  hash_handle *_src_hh, *_dst_hh, *_last_elt_hh=NULL;\
  ptrdiff_t _dst_hho = ((char*)(&(dst)->hh_dst) - (char*)(dst));\
  if ((src) != NULL) {\
    for (_src_bkt=0; _src_bkt < (src)->hh_src.tbl->num_buckets; _src_bkt++) {\
      for (_src_hh = (src)->hh_src.tbl->buckets[_src_bkt].hh_head; \
        _src_hh != NULL; \
        _src_hh = _src_hh->hh_next) {\
        _elt = ELMT_FROM_HH((src)->hh_src.tbl, _src_hh);\
        if (cond(_elt)) {\
          IF_HASH_NONFATAL_OOM( int _hs_oomed = 0; )\
          _dst_hh = (hash_handle*)(void*)(((char*)_elt) + _dst_hho);\
          _dst_hh->key = _src_hh->key;\
          _dst_hh->keylen = _src_hh->keylen;\
          _dst_hh->hashv = _src_hh->hashv;\
          _dst_hh->prev = _last_elt;\
          _dst_hh->next = NULL;\
          if (_last_elt_hh != NULL) {\
            _last_elt_hh->next = _elt;\
          }\
          if ((dst) == NULL) {\
            DECLTYPE_ASSIGN(dst, _elt);\
            HASH_MAKE_TABLE(hh_dst, dst, _hs_oomed);\
            IF_HASH_NONFATAL_OOM(\
              if (_hs_oomed) {\
                (dst) = NULL;\
                continue;\
              }\
            )\
          } else {\
            _dst_hh->tbl = (dst)->hh_dst.tbl;\
          }\
          HASH_TO_BKT(_dst_hh->hashv, _dst_hh->tbl->num_buckets, _dst_bkt);\
          HASH_ADD_TO_BKT(_dst_hh->tbl->buckets[_dst_bkt], hh_dst, _dst_hh, _hs_oomed);\
          (dst)->hh_dst.tbl->num_items++;\
          IF_HASH_NONFATAL_OOM(\
            if (_hs_oomed) {\
              HASH_ROLLBACK_BKT(hh_dst, dst, _dst_hh);\
              HASH_DELETE_HH(hh_dst, dst, _dst_hh);\
              _dst_hh->tbl = NULL;\
              continue;\
            }\
          )\
          HASH_BLOOM_ADD(_dst_hh->tbl, _dst_hh->hashv);\
          _last_elt = _elt;\
          _last_elt_hh = _dst_hh;\
        }\
      }\
    }\
  }\
  HASH_FSCK(hh_dst, dst, "HASH_SELECT");\
} while (0)

#define HASH_CLEAR(hh,head) \
do {\
  if ((head) != NULL) {\
    HASH_BLOOM_FREE((head)->hh.tbl);\
    free((head)->hh.tbl->buckets, \
         (head)->hh.tbl->num_buckets*sizeof(struct hash_bucket));\
    free((head)->hh.tbl, sizeof(hash_table));\
    (head) = NULL;\
  }\
} while (0)

#define HASH_OVERHEAD(hh,head) (((head) != NULL) ? ((size_t)(((head)->hh.tbl->num_items * sizeof(hash_handle)) + ((head)->hh.tbl->num_buckets * sizeof(hash_bucket)) + sizeof(hash_table) + (HASH_BLOOM_BYTELEN))) : 0U)

#define HASH_ITER(hh,head,el,tmp)                                                \
for(((el)=(head)), ((tmp)=DECLTYPE(el)((head!=NULL)?(head)->hh.next:NULL)); (el) != NULL; ((el)=(tmp)), ((tmp)=DECLTYPE(el)((tmp!=NULL)?(tmp)->hh.next:NULL)))

#define HASH_COUNT(head) HASH_CNT(hh,head)
#define HASH_CNT(hh,head) ((head != NULL)?((head)->hh.tbl->num_items):0U)

typedef struct hash_bucket {
   struct hash_handle *hh_head;
   unsigned count;
   unsigned expand_mult;
} hash_bucket;

#define HASH_SIGNATURE 0xa0111fe1u
#define HASH_BLOOM_SIGNATURE 0xb12220f2u

typedef struct hash_table {
   hash_bucket *buckets;
   unsigned num_buckets, log2_num_buckets;
   unsigned num_items;
   struct hash_handle *tail;
   ptrdiff_t hho;
   unsigned ideal_chain_maxlen;
   unsigned nonideal_items;
   unsigned ineff_expands, noexpand;

   uint32_t signature;
} hash_table;

typedef struct hash_handle {
   struct hash_table *tbl;
   void *prev;
   void *next;
   struct hash_handle *hh_prev;
   struct hash_handle *hh_next;
   const void *key;
   unsigned keylen;
   unsigned hashv;
} hash_handle;

