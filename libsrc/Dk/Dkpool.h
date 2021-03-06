/*
 *  Dkpool.h
 *
 *  $Id$
 *
 *  Temp memory pool for objects that should be allocated one by one but freed
 *  together.
 *
 *  This file is part of the OpenLink Software Virtuoso Open-Source (VOS)
 *  project.
 *
 *  Copyright (C) 1998-2013 OpenLink Software
 *
 *  This project is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation; only version 2 of the License, dated June 1991.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */
#ifndef __DKPOOL_H
#define __DKPOOL_H

#include <stdio.h>

void mp_free (mem_pool_t * mp);

#ifdef VALGRIND
#define LACERATED_POOL
#endif
#ifdef MALLOC_DEBUG
#define LACERATED_POOL
#endif

#ifdef LACERATED_POOL
struct mem_pool_s
{
  int 			mp_fill;
  int 			mp_size;
  caddr_t *		mp_allocs;
  size_t 		mp_bytes;
  dk_hash_t *		mp_unames;
  dk_set_t 		mp_trash;		/* dk_alloc_box boxes that must be freed with the mp */
#if defined (DEBUG) || defined (MALLOC_DEBUG)
  const char *		mp_alloc_file;
  int 			mp_alloc_line;
#endif
#ifdef MALLOC_DEBUG
  const char *		mp_list_alloc_file;
  int 			mp_list_alloc_line;
#endif
};
#else
struct mem_block_s
{
  struct mem_block_s *	mb_next;
  size_t 		mb_fill;
  size_t 		mb_size;
};

typedef struct mem_block_s mem_block_t;

struct mem_pool_s
{
  mem_block_t *		mp_first;
  int 			mp_block_size;
  size_t 		mp_bytes;
  dk_hash_t *		mp_unames;
  dk_set_t 		mp_trash;
#if defined (DEBUG) || defined (MALLOC_DEBUG)
  const char *		mp_alloc_file;
  int 			mp_alloc_line;
#endif
};
#endif

EXE_EXPORT (mem_pool_t *, mem_pool_alloc, (void));
#if defined (DEBUG) || defined (MALLOC_DEBUG)
extern mem_pool_t *dbg_mem_pool_alloc (const char *file, int line);
#define mem_pool_alloc() dbg_mem_pool_alloc (__FILE__, __LINE__)
#endif

EXE_EXPORT (caddr_t, mp_alloc_box, (mem_pool_t * mp, size_t len, dtp_t dtp));
EXE_EXPORT (caddr_t, mp_box_string, (mem_pool_t * mp, const char *str));
EXE_EXPORT (caddr_t, mp_box_substr, (mem_pool_t * mp, ccaddr_t str, int n1, int n2));
EXE_EXPORT (box_t, mp_box_dv_short_nchars, (mem_pool_t * mp, const char *str, size_t len));
EXE_EXPORT (caddr_t, mp_box_dv_uname_string, (mem_pool_t * mp, const char *str));
EXE_EXPORT (box_t, mp_box_dv_uname_nchars, (mem_pool_t * mp, const char *str, size_t len));
EXE_EXPORT (caddr_t, mp_box_copy, (mem_pool_t * mp, caddr_t box));
EXE_EXPORT (caddr_t, mp_box_copy_tree, (mem_pool_t * mp, caddr_t box));
EXE_EXPORT (caddr_t, mp_full_box_copy_tree, (mem_pool_t * mp, caddr_t box));
EXE_EXPORT (caddr_t, mp_box_num, (mem_pool_t * mp, boxint num));

#ifdef MALLOC_DEBUG
extern caddr_t dbg_mp_alloc_box (const char *file, int line, mem_pool_t * mp, size_t len, dtp_t dtp);
extern caddr_t dbg_mp_box_string (const char *file, int line, mem_pool_t * mp, const char *str);
extern caddr_t dbg_mp_box_substr (const char *file, int line, mem_pool_t * mp, ccaddr_t str, int n1, int n2);
extern box_t dbg_mp_box_dv_short_nchars (const char *file, int line, mem_pool_t * mp, const char *str, size_t len);
extern caddr_t dbg_mp_box_dv_uname_string (const char *file, int line, mem_pool_t * mp, const char *str);
extern box_t dbg_mp_box_dv_uname_nchars (const char *file, int line, mem_pool_t * mp, const char *str, size_t len);
extern caddr_t dbg_mp_box_copy (const char *file, int line, mem_pool_t * mp, caddr_t box);
extern caddr_t dbg_mp_box_copy_tree (const char *file, int line, mem_pool_t * mp, caddr_t box);
extern caddr_t dbg_mp_full_box_copy_tree (const char *file, int line, mem_pool_t * mp, caddr_t box);
extern caddr_t dbg_mp_box_num (const char *file, int line, mem_pool_t * mp, boxint num);
#ifndef _USRDLL
#ifndef EXPORT_GATE
#define mp_alloc_box(mp,len,dtp) dbg_mp_alloc_box (__FILE__, __LINE__, (mp), (len), (dtp))
#define mp_box_string(mp, str) dbg_mp_box_string (__FILE__, __LINE__, (mp), (str))
#define mp_box_substr(mp, str, n1, n2) dbg_mp_box_substr (__FILE__, __LINE__, (mp), (str), (n1), (n2))
#define mp_box_dv_short_nchars(mp, str, len) dbg_mp_box_dv_short_nchars (__FILE__, __LINE__, (mp), (str), (len))
#define mp_box_dv_uname_string(mp, str) dbg_mp_box_dv_uname_string (__FILE__, __LINE__, (mp), (str))
#define mp_box_dv_uname_nchars(mp, str, len) dbg_mp_box_dv_uname_nchars (__FILE__, __LINE__, (mp), (str), (len))
#define mp_box_copy(mp, box) dbg_mp_box_copy (__FILE__, __LINE__, (mp), (box))
#define mp_box_copy_tree(mp, box) dbg_mp_box_copy_tree (__FILE__, __LINE__, (mp), (box))
#define mp_full_box_copy_tree(mp, box) dbg_mp_full_box_copy_tree (__FILE__, __LINE__, (mp), (box))
#define mp_box_num(mp, num) dbg_mp_box_num (__FILE__, __LINE__, (mp), (num))
#endif
#endif
#endif
caddr_t mp_alloc_sized (mem_pool_t * mp, size_t len);

#ifdef LACERATED_POOL
void mp_alloc_box_assert (mem_pool_t * mp, caddr_t box);
#else
#define mp_alloc_box_assert(mp,box) ;
#endif

caddr_t *mp_list (mem_pool_t * mp, long n, ...);
#define mp_alloc(mp, n) mp_alloc_box (mp, n, DV_CUSTOM)

#define THR_TMP_POOL  ((mem_pool_t *) THREAD_CURRENT_THREAD->thr_tmp_pool)
#define SET_THR_TMP_POOL(v)  (THREAD_CURRENT_THREAD->thr_tmp_pool = (void*) v)

#ifdef DEBUG						   /* Not MALLOC_DEBUG */
#define MP_START() \
  do { \
    mem_pool_t *thread_mem_pool = THR_TMP_POOL; \
    if (thread_mem_pool != NULL) \
      GPF_T1 ("MP reallocated"); \
    SET_THR_TMP_POOL (dbg_mem_pool_alloc (__FILE__, __LINE__)); \
  } while (0)
#else
#define MP_START() \
  do { \
    if (THR_TMP_POOL != NULL) \
      GPF_T1 ("MP reallocated"); \
    SET_THR_TMP_POOL (mem_pool_alloc ()); \
  } while (0)
#endif

#define MP_DONE() \
  do { \
    mp_free (THR_TMP_POOL); \
  SET_THR_TMP_POOL (NULL);	\
    } while (0)

#ifdef _DEBUG
#define mp_box_tag_modify_impl(box,new_tag) \
 do { \
   box_tag_aux((box)) = (new_tag); \
   } while (0)
#else
#define mp_box_tag_modify_impl(box,new_tag) (box_tag_aux((box)) = (new_tag))
#endif

#ifdef MALLOC_DEBUG
#define mp_box_tag_modify(box,new_tag) \
 do { \
   if (DV_UNAME == new_tag) \
     GPF_T1 ("Can't make UNAME by mp_box_tag_modify"); \
   if (DV_UNAME == box_tag_aux(box)) \
     GPF_T1 ("Can't alter UNAME by mp_box_tag_modify"); \
   if (DV_REFERENCE == new_tag) \
     GPF_T1 ("Can't make REFERENCE by mp_box_tag_modify"); \
   if (DV_REFERENCE == box_tag_aux(box)) \
     GPF_T1 ("Can't alter REFERENCE by mp_box_tag_modify"); \
   if (TAG_FREE == new_tag) \
     GPF_T1 ("Can't make TAG_FREE box by mp_box_tag_modify"); \
   if (TAG_FREE == box_tag_aux(box)) \
     GPF_T1 ("Can't alter TAG_FREE by mp_box_tag_modify"); \
   if (TAG_BAD == new_tag) \
     GPF_T1 ("Can't make TAG_BAD box by mp_box_tag_modify"); \
   if (TAG_BAD == box_tag_aux(box)) \
     GPF_T1 ("Can't alter TAG_BAD by mp_box_tag_modify"); \
   mp_box_tag_modify_impl(box,new_tag); \
   } while (0);
#else
#define mp_box_tag_modify(box,new_tag) mp_box_tag_modify_impl(box,new_tag)
#endif



#define dbg_t_alloc_box(len,dtp)		dbg_mp_alloc_box (DBG_ARGS THR_TMP_POOL, (len), (dtp))
#define dbg_t_box_string(str)			dbg_mp_box_string (DBG_ARGS THR_TMP_POOL, (str))
#define dbg_t_box_substr(str,n1,n2)		dbg_mp_box_substr (DBG_ARGS THR_TMP_POOL, (str), (n1), (n2))
#define dbg_t_box_dv_short_nchars(str,len) 	dbg_mp_box_dv_short_nchars (DBG_ARGS THR_TMP_POOL, (str), (len))
#define dbg_t_box_dv_uname_string(str)		dbg_mp_box_dv_uname_string (DBG_ARGS THR_TMP_POOL, (str))
#define dbg_t_box_dv_uname_nchars(str,len) 	dbg_mp_box_dv_uname_nchars (DBG_ARGS THR_TMP_POOL, (str), (len))
#define dbg_t_box_copy(box)			dbg_mp_box_copy (DBG_ARGS THR_TMP_POOL, (box))
#define dbg_t_box_copy_tree(box)		dbg_mp_box_copy_tree (DBG_ARGS THR_TMP_POOL, (box))
#define dbg_t_full_box_copy_tree(box)		dbg_mp_full_box_copy_tree (DBG_ARGS THR_TMP_POOL, (box))
#define t_alloc_box(len,dtp)			mp_alloc_box (THR_TMP_POOL, (len), (dtp))
#define t_box_string(str)			mp_box_string (THR_TMP_POOL, (str))
#define t_box_substr(str,n1,n2)			mp_box_substr (THR_TMP_POOL, (str), (n1), (n2))
#define t_box_dv_short_nchars(str,len)		mp_box_dv_short_nchars (THR_TMP_POOL, (str), (len))
#define t_box_dv_uname_string(str)		mp_box_dv_uname_string (THR_TMP_POOL, (str))
#define t_box_dv_uname_nchars(str,len)		mp_box_dv_uname_nchars (THR_TMP_POOL, (str), (len))
#define t_box_copy(box)				mp_box_copy (THR_TMP_POOL, (box))
#define t_box_copy_tree(box)			mp_box_copy_tree (THR_TMP_POOL, (box))
#define t_full_box_copy_tree(box)		mp_full_box_copy_tree (THR_TMP_POOL, (box))

#define t_alloc_list(n) 			((caddr_t *)t_alloc_box ((n) * sizeof (caddr_t), DV_ARRAY_OF_POINTER))
extern caddr_t *t_list_concat_tail (caddr_t list, long n, ...);
extern caddr_t *t_list_concat (caddr_t list1, caddr_t list2);
extern caddr_t *t_list_remove_nth (caddr_t list, int pos);
extern caddr_t *t_list_insert_before_nth (caddr_t list, caddr_t new_item, int pos);
extern caddr_t *t_list_insert_many_before_nth (caddr_t list, caddr_t * new_items, int ins_count, int pos);
caddr_t *t_sc_list (long n, ...);

#define t_NEW_DB_NULL 				t_alloc_box (0, DV_DB_NULL)

#ifdef MALLOC_DEBUG
caddr_t dbg_t_box_num (const char *file, int line, boxint box);
caddr_t dbg_t_box_num_and_zero (const char *file, int line, boxint box);
box_t dbg_t_box_double (const char *file, int line, double d);
box_t dbg_t_box_float (const char *file, int line, float d);
caddr_t dbg_t_box_iri_id (const char *file, int line, int64 n);
#define t_box_num(box) 				dbg_t_box_num (__FILE__, __LINE__, (box))
#define t_box_double(d) 			dbg_t_box_double (__FILE__, __LINE__, (d))
#define t_box_float(d) 				dbg_t_box_float (__FILE__, __LINE__, (d))
#define t_box_iri_id(d) 			dbg_t_box_iri_id (__FILE__, __LINE__, (d))
#define t_box_num_and_zero(box) 		dbg_t_box_num_and_zero (__FILE__, __LINE__, (box))
extern caddr_t *t_list_impl (long n, ...);
typedef caddr_t *(*t_list_impl_ptr_t) (long n, ...);
extern t_list_impl_ptr_t t_list_cock (const char *file, int line);
#define t_list 					(t_list_cock (__FILE__, __LINE__))
#else
caddr_t t_box_num (boxint box);
caddr_t t_box_num_and_zero (boxint box);
box_t t_box_double (double d);
box_t t_box_float (float d);
caddr_t t_box_iri_id (int64 n);
extern caddr_t *t_list (long n, ...);
#endif

#define t_alloc(sz) 				t_alloc_box ((sz), DV_CUSTOM)
#define t_box_num_nonull 			t_box_num_and_zero
#define t_box_dv_short_string 			t_box_string

#define TNEW(dt, v) \
  dt * v = (dt *) t_alloc (sizeof (dt))

#define t_NEW_VARZ(dt, v) \
  TNEW(dt, v); \
  memset (v, 0, sizeof (dt))

#define t_NEW_VAR(dt, v) \
  TNEW(dt, v)

#ifdef MALLOC_DEBUG
void dbg_mp_set_push (const char *file, int line, mem_pool_t * mp, dk_set_t * set, void *elt);
dk_set_t dbg_t_cons (const char *file, int line, void *car, dk_set_t cdr);
void dbg_t_set_push (const char *file, int line, dk_set_t * set, void *elt);
int dbg_t_set_pushnew (const char *file, int line, s_node_t ** set, void *item);
int dbg_t_set_push_new_string (const char *file, int line, s_node_t ** set, void *item);
void *dbg_t_set_pop (const char *file, int line, dk_set_t * set);
dk_set_t dbg_t_set_union (const char *file, int line, dk_set_t s1, dk_set_t s2);
dk_set_t dbg_t_set_intersect (const char *file, int line, dk_set_t s1, dk_set_t s2);
dk_set_t dbg_t_set_diff (const char *file, int line, dk_set_t s1, dk_set_t s2);
caddr_t *dbg_t_list_to_array (const char *file, int line, dk_set_t list);
caddr_t *dbg_t_revlist_to_array (const char *file, int line, dk_set_t list);
int dbg_t_set_delete (const char *file, int line, dk_set_t * set, void *item);
dk_set_t dbg_t_set_copy (const char *file, int line, dk_set_t s);
#define mp_set_push(mp,set,elt)			dbg_mp_set_push (__FILE__, __LINE__, (mp), (set), (elt))
#define t_cons(car,cdr)				dbg_t_cons (__FILE__, __LINE__, (car), (cdr))
#define t_set_push(set,elt)			dbg_t_set_push (__FILE__, __LINE__, (set), (elt))
#define t_set_pushnew(set,item)			dbg_t_set_pushnew (__FILE__, __LINE__, (set), (item))
#define t_set_push_new_string(set,item)		dbg_t_set_push_new_string (__FILE__, __LINE__, (set), (item))
#define t_set_pop(set)				dbg_t_set_pop (__FILE__, __LINE__, (set))
#define t_set_union(s1,s2)			dbg_t_set_union (__FILE__, __LINE__, (s1), (s2))
#define t_set_intersect(s1,s2)			dbg_t_set_intersect (__FILE__, __LINE__, (s1), (s2))
#define t_set_diff(s1,s2)			dbg_t_set_diff (__FILE__, __LINE__, (s1), (s2))
#define t_list_to_array(list)			dbg_t_list_to_array (__FILE__, __LINE__, (list))
#define t_revlist_to_array(list)		dbg_t_revlist_to_array (__FILE__, __LINE__, (list))
#define t_set_delete(set,item)			dbg_t_set_delete (__FILE__, __LINE__, (set), (item))
#define t_set_copy(s)				dbg_t_set_copy (__FILE__, __LINE__, (s))
#else
void mp_set_push (mem_pool_t * mp, dk_set_t * set, void *elt);
dk_set_t t_cons (void *car, dk_set_t cdr);
void t_set_push (dk_set_t * set, void *elt);
int t_set_pushnew (s_node_t ** set, void *item);
int t_set_push_new_string (s_node_t ** set, void *item);
void *t_set_pop (dk_set_t * set);
dk_set_t t_set_union (dk_set_t s1, dk_set_t s2);
dk_set_t t_set_intersect (dk_set_t s1, dk_set_t s2);
dk_set_t t_set_diff (dk_set_t s1, dk_set_t s2);
caddr_t *t_list_to_array (dk_set_t list);
caddr_t *t_revlist_to_array (dk_set_t list);
int t_set_delete (dk_set_t * set, void *item);
dk_set_t t_set_copy (dk_set_t s);
#endif
#define mp_set_nreverse(mp,s) dk_set_nreverse((s))
#define t_set_nreverse(s) dk_set_nreverse((s))
#define t_revlist_to_array_or_null(list)	((NULL != (list)) ? t_revlist_to_array ((list)) : NULL)


#ifdef MALLOC_DEBUG
void mp_check_tree (mem_pool_t * mp, box_t box);
#define t_check_tree(box) 			mp_check_tree (THR_TMP_POOL, (box))
#else
#define mp_check_tree(mp,box)			;
#define t_check_tree(box)			;
#endif

#ifdef _DKSYSTEM_H
caddr_t t_box_vsprintf (size_t buflen_eval, const char *format, va_list tail);
caddr_t t_box_sprintf (size_t buflen_eval, const char *format, ...);
#endif

void mp_trash (mem_pool_t * mp, caddr_t box);
caddr_t mp_alloc_box_ni (mem_pool_t * mp, int len, dtp_t dtp);
extern box_tmp_copy_f box_tmp_copier[256];

#ifdef LACERATED_POOL
#define MP_BYTES(x, mp, len)  			x = mp_alloc_box (mp, len, DV_NON_BOX)
#else
#define MP_BYTES(x, mp, len2) \
  { \
    int __len = ALIGN_8 (len2); \
    mem_block_t * f = mp->mp_first; \
    if (f && f->mb_fill + __len <= f->mb_size) \
      { \
	x = ((char*)f) + f->mb_fill; \
	f->mb_fill += __len; \
      } \
    else \
      x = mp_alloc_box (mp, len2, DV_NON_BOX); \
  }
#endif

#define MP_INT(x, mp, v, tag_word)		\
  { \
    MP_BYTES (x, mp, 16); \
    x = ((char *)x) + 8; \
    *(int64 *)x = v; \
    ((int32*)x)[-1] = tag_word; \
  }


typedef struct auto_pool_s
{
  caddr_t 	ap_area;
  int 		ap_size;
  int 		ap_fill;
} auto_pool_t;

#define AUTO_POOL(n) \
  int64  area[n];  \
  auto_pool_t ap; \
  ap.ap_area = (caddr_t) &area;			\
  ap.ap_fill = 0; \
  ap.ap_size = sizeof (area); \

caddr_t ap_box_num (auto_pool_t * ap, int64 i);
caddr_t ap_alloc_box (auto_pool_t * ap, int n, dtp_t tag);
caddr_t *ap_list (auto_pool_t * apool, long n, ...);
caddr_t ap_box_iri_id (auto_pool_t * ap, int64 n);


#define WITHOUT_TMP_POOL \
  { \
    mem_pool_t * __mp = THR_TMP_POOL; \
    SET_THR_TMP_POOL (NULL);

#define END_WITHOUT_TMP_POOL \
    SET_THR_TMP_POOL (__mp); \
  }

#define NO_TMP_POOL \
  if (THR_TMP_POOL) GPF_T1 ("not supposed to have a tmp pool in effect here");

#endif /* ifdef __DKPOOL_H */
