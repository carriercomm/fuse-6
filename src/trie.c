/*
 * trie.c / trie.h
 * (c) 2013 ggrin // FIXME!!!
 * (c) 2013 Niklas E. Cathor
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <string.h>

#include "trie.h"

struct trie_node {
  char key; // key == 0 represents end.
  char *childkeys;
  struct trie_node **children;
  void *value;
};

TrieNode *new_node(char key, void *value) {
  TrieNode *node = malloc(sizeof(TrieNode));
  if(node == NULL) {
    return NULL;
  }
  memset(node, 0, sizeof(TrieNode));
  node->key = key;
  node->value = value;
  if((node->childkeys = malloc(1)) == NULL) {
    free(node);
    return NULL;
  }
  if((node->children = malloc(sizeof(TrieNode*))) == NULL) {
    free(node->childkeys);
    free(node);
    return NULL;
  }
  *node->childkeys = 0;
  return node;
}

TrieNode *new_trie() {
  return new_node(0, NULL);
}

int append(TrieNode *parent, TrieNode *child) {
  int len = strlen(parent->childkeys);
  parent->children = realloc(parent->children, (len + 1) * sizeof(TrieNode*));
  if(parent->children == NULL) {
    return -1;
  }
  parent->childkeys = realloc(parent->childkeys, len + 1);
  if(parent->childkeys == NULL) {
    return -1;
  }
  parent->children[len] = child;
  parent->childkeys[len] = child->key;
  parent->childkeys[++len] = 0;
  return 0;
}

TrieNode *find_child(TrieNode *node, char key) {
  char c;
  int i;
  for(i = 0; (c = node->childkeys[i]) != 0; i++) {
    if(c == key) {
      return node->children[i];
    }
  }
  return NULL;
}

int trie_insert(TrieNode *parent, const char *key, void *value, void **old_value_ptr) {
  TrieNode *child;
  if(*key) {
    child = find_child(parent, *key);
    if(! child) {
      child = new_node(*key, NULL);
      if(child == NULL) {
        return -1;
      }
      if(append(parent, child) != 0) {
        return -1;
      }
    }
    void *old_ptr = NULL;
    int result = trie_insert(child, ++key, value, &old_ptr);
    if(old_ptr) free(old_ptr);
    return result;
  } else {
    *old_value_ptr = parent->value;
    parent->value = value;
    return 0;
  }
}

void *trie_search(TrieNode *parent, const char *key) {
  TrieNode *child;
  if(*key) {
    child = find_child(parent, *key);
    if(child) {
      return trie_search(child, ++key);
    } else {
      return NULL;
    }
  } else {
    return parent->value;
  }
}

void destroy_trie(TrieNode *root) {
  int len = strlen(root->childkeys);
  int i;
  for(i=0;i<len;i++) {
    destroy_trie(root->children[i]);
  }
  if(root->childkeys) {
    free(root->childkeys);
  }
  if(root->children) {
    free(root->children);
  }
  free(root);
}

void iterate_trie(TrieNode *node, void (*cb)(void *, void *), void *userdata) {
  if(node->value) {
    cb(node->value, userdata);
  }
  int i;
  for(i=0;node->childkeys[i] != 0;i++) {
    iterate_trie(node->children[i], cb, userdata);
  }
}

#ifdef DEBUG_TRIE

#include <stdio.h>

void _dump_tree(TrieNode *node, int depth) {
  for(int d=0;d<depth;d++) {
    fprintf(stderr, " ");
  }
  if(node->key) {
    fprintf(stderr, "%c", node->key);
    if(node->value) {
      fprintf(stderr, " -> 0x%x", node->value);
    }
    fprintf(stderr, "\n", node->key);
  }
  int len = strlen(node->childkeys);
  depth++;
  for(int i=0;i<len;i++) {
    _dump_tree(node->children[i], depth);
  }
}

void dump_tree(TrieNode *root) {
  _dump_tree(root, 0);
}

void print_line(void *value, void *userdata) {
  printf("PRINT LINE: %s\n", (char*) value);
}

int main(int argc, char **argv) {
  TrieNode *root = new_trie();
  trie_insert(root, "/foo/bar", "baz");
  trie_insert(root, "/foo/blubb", "bla");
  trie_insert(root, "/asdf/dassf/fdas", "blubb");
  trie_insert(root, "/asdd/f/dsa/s", "fasd");

  //dump_tree(root);

  iterate_trie(root, print_line, NULL);

  char *result = (char*)trie_search(root, "/asdf/dassf/fdas");

  fprintf(stderr, "RESULT: %s\n", result);
  return 0;
}

#endif
