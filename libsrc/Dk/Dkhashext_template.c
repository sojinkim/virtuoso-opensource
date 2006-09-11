/*
 *  
 *  This file is part of the OpenLink Software Virtuoso Open-Source (VOS)
 *  project.
 *  
 *  Copyright (C) 1998-2006 OpenLink Software
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
 *  
*/
#define ID_HASH_ALLOCATE_INTERNALS(ht,buckets,keybytes,databytes,hf,cf) \
  memset (ht, 0, sizeof (id_hash_t)); \
  ht->ht_buckets = buckets; \
  ht->ht_key_length = keybytes; \
  ht->ht_data_length = databytes; \
  ht->ht_bucket_length = NEXT4 (keybytes) + NEXT4 (databytes) + \
     sizeof (caddr_t); \
  ht->ht_array = (char *) DBG_HASHEXT_ALLOC (buckets * ht->ht_bucket_length); \
  ht->ht_data_inx = NEXT4 (keybytes); \
  ht->ht_ext_inx = ht->ht_data_inx + NEXT4 (databytes); \
  ht->ht_hash_func = hf; \
  ht->ht_cmp = cf; \
  memset (ht->ht_array, -1, ht->ht_buckets * ht->ht_bucket_length)
  /* -1 all over, meaning that all buckets are empty. The bucket
     empty condition being indicated by BUCKET_OVERFLOW == -1 */

#define ID_CHECK_REHASH(ht) \
  if (ht->ht_rehash_threshold && ((uint32) ht->ht_rehash_threshold) < (uint32)((ht->ht_count * 100) / ht->ht_buckets)) \
    DBG_HASHEXT_NAME(id_hash_rehash) (DBG_ARGS ht, ht->ht_buckets << 1)

#define id_ht_max_sz 1048573

id_hash_t *
DBG_HASHEXT_NAME(id_hash_allocate) (DBG_PARAMS id_hashed_key_t buckets, int keybytes, int databytes,
    hash_func_t hf, cmp_func_t cf)
{
  id_hash_t *ht = (id_hash_t *)DBG_HASHEXT_ALLOC(sizeof(id_hash_t));
  ID_HASH_ALLOCATE_INTERNALS(ht,buckets,keybytes,databytes,hf,cf);
  return ht;
}


#define ID_HASH_FREE_INTERNALS(hash) \
  DBG_HASHEXT_FREE ((char *) ((hash)->ht_array), -1)

void
DBG_HASHEXT_NAME(id_hash_free) (DBG_PARAMS id_hash_t * hash)
{
  DBG_HASHEXT_NAME(id_hash_clear) (DBG_ARGS hash);
  ID_HASH_FREE_INTERNALS(hash);
  DBG_HASHEXT_FREE ((char *) hash, sizeof (id_hash_t));
}


void
DBG_HASHEXT_NAME(id_hash_clear) (DBG_PARAMS id_hash_t * hash)
{
  id_hashed_key_t n;
  for (n = 0; n < hash->ht_buckets; n++)
    {
      char *ext = BUCKET_OVERFLOW (BUCKET (hash, n), hash);
      if (ext != (char *) -1L)
	{
	  while (ext)
	    {
	      char *next = BUCKET_OVERFLOW (ext, hash);
	      DBG_HASHEXT_FREE (ext, -1L);
	      ext = next;
	    }
	  BUCKET_OVERFLOW (BUCKET (hash, n), hash) = (char *) -1L;
	}
    }
  hash->ht_inserts = 0;
  hash->ht_deletes = 0;
  hash->ht_overflows = 0;
  hash->ht_dict_refctr = 0;
  hash->ht_count = 0;
}


void
DBG_HASHEXT_NAME(id_hash_set) (DBG_PARAMS id_hash_t * ht, caddr_t key, caddr_t data)
{
  id_hashed_key_t inx = ht->ht_hash_func (key);
        caddr_t place = id_hash_get_with_hash_number (ht, key, inx);
  if (place)
    {
      memcpy (place, data, ht->ht_data_length);
    }
  else
    {
      char *bucket;
      ID_HASHED_KEY_CHECK(inx);
      ID_CHECK_REHASH (ht);
      inx = (inx & ID_HASHED_KEY_MASK) % ht->ht_buckets;
      ht->ht_inserts++;
      ht->ht_count++;
      if (BUCKET_IS_EMPTY (BUCKET (ht, inx), ht))
	{
	  bucket = BUCKET (ht, inx);
	  memcpy (bucket, key, ht->ht_key_length);
	  memcpy (bucket + ht->ht_data_inx, data, ht->ht_data_length);
	  BUCKET_OVERFLOW (bucket, ht) = NULL;
	}
      else
	{
	  ht->ht_overflows++;
	  bucket = (char *) DBG_HASHEXT_ALLOC (ht->ht_bucket_length);
	  memcpy (bucket, key, ht->ht_key_length);
	  memcpy (bucket + ht->ht_data_inx, data, ht->ht_data_length);
	  BUCKET_OVERFLOW (bucket, ht) = BUCKET_OVERFLOW (BUCKET (ht, inx), ht);
	  BUCKET_OVERFLOW (BUCKET (ht, inx), ht) = bucket;
	}
    }
}


caddr_t
DBG_HASHEXT_NAME(id_hash_add_new) (DBG_PARAMS id_hash_t * ht, caddr_t key, caddr_t data)
{
  char *bucket;
  caddr_t res;
  id_hashed_key_t inx = ht->ht_hash_func (key);
  ID_HASHED_KEY_CHECK(inx);
  ID_CHECK_REHASH (ht);
  inx = (inx & ID_HASHED_KEY_MASK) % ht->ht_buckets;
  ht->ht_inserts++;
  ht->ht_count++;
  if (BUCKET_IS_EMPTY (BUCKET (ht, inx), ht))
    {
      bucket = BUCKET (ht, inx);
      memcpy (bucket, key, ht->ht_key_length);
      res = bucket + ht->ht_data_inx;
      memcpy (res, data, ht->ht_data_length);
      BUCKET_OVERFLOW (bucket, ht) = NULL;
    }
  else
    {
      ht->ht_overflows++;
      bucket = (char *) DBG_HASHEXT_ALLOC (ht->ht_bucket_length);
      memcpy (bucket, key, ht->ht_key_length);
      res = bucket + ht->ht_data_inx;
      memcpy (res, data, ht->ht_data_length);
      BUCKET_OVERFLOW (bucket, ht) = BUCKET_OVERFLOW (BUCKET (ht, inx), ht);
      BUCKET_OVERFLOW (BUCKET (ht, inx), ht) = bucket;
    }
  return res;
}


int
DBG_HASHEXT_NAME(id_hash_remove) (DBG_PARAMS id_hash_t * ht, caddr_t key)
{
  id_hashed_key_t inx = ht->ht_hash_func (key);
  ID_HASHED_KEY_CHECK(inx);
  inx = (inx & ID_HASHED_KEY_MASK) % ht->ht_buckets;

  if (BUCKET_IS_EMPTY (BUCKET (ht, inx), ht))
    return 0;
  if (ht->ht_cmp (BUCKET (ht, inx), key))
    {
      /* The thing is in the bucket. Pop first on overflow list
	 in. Mark bucket empty if no overflow list. */
      char *overflow = BUCKET_OVERFLOW (BUCKET (ht, inx), ht);
      if (overflow)
	{
	  memcpy (BUCKET (ht, inx), overflow,
	      ht->ht_data_length + ht->ht_key_length + sizeof (caddr_t));
	  DBG_HASHEXT_FREE (overflow, -1);
	}
      else
	{
	  BUCKET_OVERFLOW (BUCKET (ht, inx), ht) = (char *) -1L;
	}
      ht->ht_deletes++;
      ht->ht_count--;
      return 1;
    }
  else
    {
      char **prev = &BUCKET_OVERFLOW (BUCKET (ht, inx), ht);
      char *ext = BUCKET_OVERFLOW (BUCKET (ht, inx), ht);
      while (ext)
	{
	  if (ht->ht_cmp (ext, key))
	    {
	      *prev = BUCKET_OVERFLOW (ext, ht);
	      DBG_HASHEXT_FREE (ext, -1);
	      ht->ht_deletes++;
	      ht->ht_count--;
	      return 1;
	    }
	  prev = &BUCKET_OVERFLOW (ext, ht);
	  ext = *prev;
	}
    }
  return 0;
}


int
DBG_HASHEXT_NAME(id_hash_remove_rnd) (DBG_PARAMS id_hash_t * ht, int inx, caddr_t key, caddr_t data)
{
  inx = (inx & ID_HASHED_KEY_MASK) % ht->ht_buckets;
  if (BUCKET_IS_EMPTY (BUCKET (ht, inx), ht))
    return 0;
  {
    /* The thing is in the bucket. Pop first on overflow list
     * in. Mark bucket empty if no overflow list. */
    
    char *overflow = BUCKET_OVERFLOW (BUCKET (ht, inx), ht);
    memcpy (key, BUCKET (ht, inx), ht->ht_key_length);
    memcpy (data, BUCKET (ht, inx) + ht->ht_data_inx, ht->ht_data_length);
    if (overflow)
      {
	memcpy (BUCKET (ht, inx), overflow,
		ht->ht_data_length + ht->ht_key_length + sizeof (caddr_t));
	DBG_HASHEXT_FREE (overflow, -1);
      }
    else
      {
	BUCKET_OVERFLOW (BUCKET (ht, inx), ht) = (char *) -1L;
      }
    ht->ht_deletes++;
    ht->ht_count--;
    return 1;
  }
}

id_hash_t *
DBG_HASHEXT_NAME(id_tree_hash_create) (DBG_PARAMS id_hashed_key_t buckets)
{
  return (DBG_HASHEXT_NAME(id_hash_allocate) (DBG_ARGS buckets, sizeof (void *), sizeof (void *),
      treehash, treehashcmp));
}


id_hash_t *
DBG_HASHEXT_NAME(id_str_hash_create) (DBG_PARAMS id_hashed_key_t buckets)
{
  return (DBG_HASHEXT_NAME(id_hash_allocate) (DBG_ARGS buckets, sizeof (void *), sizeof (void *),
      strhash, strhashcmp));
}


id_hash_t *
DBG_HASHEXT_NAME(id_strcase_hash_create) (DBG_PARAMS id_hashed_key_t buckets)
{
  return (DBG_HASHEXT_NAME(id_hash_allocate) (DBG_ARGS buckets, sizeof (void *), sizeof (void *),
      strhashcase, strhashcasecmp));
}


void
DBG_HASHEXT_NAME(id_hash_copy) (DBG_PARAMS id_hash_t * to, id_hash_t * from)
{
  id_hash_iterator_t hit;
  char *kp;
  char *dp;
  id_hash_iterator (&hit, from);
  while (hit_next (&hit, &kp, &dp))
    DBG_HASHEXT_NAME(id_hash_set) (DBG_ARGS to, kp, dp);
}

/*#define ID_HT_STATS*/
void
DBG_HASHEXT_NAME (id_hash_rehash) (DBG_PARAMS id_hash_t *ht, id_hashed_key_t new_sz)
{
  id_hash_t ht_buffer;
  new_sz = hash_nextprime (new_sz);

  if (ht->ht_buckets >= id_ht_max_sz)
    {
#ifdef ID_HT_STATS
      fprintf (stderr, "*** ID HASH TABLE %p FULL ***\n", ht);
#endif
      return;
    }

#ifdef ID_HT_STATS
  fprintf (stderr, "*** ID HASH TABLE %p REHASH TO %lu\n", ht, new_sz);
#endif

  ID_HASH_ALLOCATE_INTERNALS((&ht_buffer),new_sz,ht->ht_key_length,ht->ht_data_length,
      ht->ht_hash_func, ht->ht_cmp);
  ht_buffer.ht_rehash_threshold = ht->ht_rehash_threshold;

#if 0 /* There's a faster way. Moreover it will works with context-sensitive cmp function */
  DBG_HASHEXT_NAME (id_hash_copy) (DBG_ARGS  &ht_buffer, ht);
#else
  do
    {
      id_hash_iterator_t hit;
      char *kp;
      char *dp;
      id_hash_iterator (&hit, ht);
      while (hit_next (&hit, &kp, &dp))
        DBG_HASHEXT_NAME(id_hash_add_new) (DBG_ARGS &ht_buffer, kp, dp);
    } while (0);
  DBG_HASHEXT_NAME (id_hash_clear) (DBG_ARGS ht);
  ID_HASH_FREE_INTERNALS(ht);
  memcpy (ht, &ht_buffer, sizeof (id_hash_t));
}
#endif
