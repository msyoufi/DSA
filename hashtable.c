#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct node
{
  char *key;
  char *value;
  struct node *next;
} node;

typedef struct
{
  node **entries;
} h_table;

unsigned int get_hash(const char *key);
h_table *ht_init(void);
bool ht_set(h_table *hash_table, const char *key, const char *value);
node *create_entry(const char *key, const char *value);
char *ht_get(h_table *hash_table, const char *key);
bool ht_del(h_table *hash_table, const char *key);
void ht_dump(h_table *hash_table);

#define TABLE_SIZE 100

int main(int argc, char *argv[])
{
  h_table *hash_table = ht_init();
  if (hash_table == NULL)
    return 1;

  ht_set(hash_table, "name1", "My name is name1");
  ht_set(hash_table, "name2", "My name is name2");
  ht_set(hash_table, "name3", "My name is name3");

  ht_dump(hash_table);

  char *value = ht_get(hash_table, "name2");
  if (value == NULL)
    printf("No entry found\n");
  else
    printf("Value found: %s\n", value);

  bool is_deleted = ht_del(hash_table, "name2");
  if (is_deleted)
    printf("Entry deleted\n");
  else
    printf("Entry cannot be deleted\n");

  ht_dump(hash_table);

  return 0;
}

unsigned int get_hash(const char *key)
{
  unsigned int hash = 0;
  int key_len = strlen(key);

  for (int i = 0; i < key_len; i++)
    hash = hash * 37 + key[i];

  hash %= TABLE_SIZE;

  return hash;
}

h_table *ht_init(void)
{
  h_table *hash_table = malloc(sizeof(h_table));
  if (hash_table == NULL)
    return NULL;

  hash_table->entries = malloc(sizeof(node *) * TABLE_SIZE);
  if (hash_table->entries == NULL)
    return NULL;

  for (int i = 0; i < TABLE_SIZE; i++)
    hash_table->entries[i] = NULL;

  return hash_table;
}

bool ht_set(h_table *hash_table, const char *key, const char *value)
{
  unsigned int slot = get_hash(key);

  node *entry = hash_table->entries[slot];

  if (entry == NULL)
  {
    hash_table->entries[slot] = create_entry(key, value);
    if (hash_table->entries[slot] == NULL)
      return false;

    return true;
  }

  node *prev;

  while (entry != NULL)
  {
    if (!strcmp(entry->key, key))
    {
      free(entry->value);
      entry->value = malloc(sizeof(strlen(value) + 1));

      if (entry->value == NULL)
        return false;

      strcpy(entry->value, value);
      return true;
    }

    prev = entry;
    entry = prev->next;
  }

  prev->next = create_entry(key, value);
  if (prev->next == NULL)
    return false;

  return true;
}

node *create_entry(const char *key, const char *value)
{
  node *entry = malloc(sizeof(node));
  entry->key = malloc(sizeof(strlen(key)) + 1);
  entry->value = malloc(sizeof(strlen(value)) + 1);

  if (entry == NULL || entry->key == NULL || entry->value == NULL)
    return NULL;

  strcpy(entry->key, key);
  strcpy(entry->value, value);

  entry->next = NULL;

  return entry;
}

char *ht_get(h_table *hash_table, const char *key)
{
  unsigned int slot = get_hash(key);

  node *entry = hash_table->entries[slot];

  if (entry == NULL)
    return NULL;

  while (entry != NULL)
  {
    if (!strcmp(entry->key, key))
      return entry->value;

    entry = entry->next;
  }

  return NULL;
}

bool ht_del(h_table *hash_table, const char *key)
{
  unsigned int slot = get_hash(key);
  node *entry = hash_table->entries[slot];

  if (entry == NULL)
  {
    printf("No entry found\n");
    return false;
  }

  node *prev;
  int inx = 0;

  while (entry != NULL)
  {
    if (!strcmp(entry->key, key))
    {
      if (entry->next == NULL && inx == 0)
        hash_table->entries[slot] = NULL;

      if (entry->next != NULL && inx == 0)
        hash_table->entries[slot] = entry->next;

      if (entry->next == NULL && inx > 0)
        prev->next = NULL;

      if (entry->next != NULL && inx > 0)
        prev->next = entry->next;

      free(entry->key);
      free(entry->value);
      free(entry);

      return true;
    }

    prev = entry;
    entry = prev->next;

    inx++;
  }

  printf("No entry found\n");
  return false;
}

void ht_dump(h_table *hash_table)
{
  for (int i = 0; i < TABLE_SIZE; i++)
  {
    node *entry = hash_table->entries[i];

    if (entry == NULL)
      continue;

    printf("Slot[%i]:\n", i);

    while (true)
    {
      printf("Key: %s, Value: %s\n", entry->key, entry->value);
      if (entry->next == NULL)
        break;

      entry = entry->next;
    }
  }
}