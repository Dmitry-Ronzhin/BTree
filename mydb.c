#include "mydb.h"

int db_close(struct DB *db) {
	return db->close(db);
}

int db_delete(struct DB *db, void *key, size_t key_len) {
	struct DBT keyt = {
		.data = key,
		.size = key_len
	};
	return db->delete(db, &keyt);
}

int db_get(struct DB *db, void *key, size_t key_len,
	   void **val, size_t *val_len) {
	struct DBT keyt = {
		.data = key,
		.size = key_len
	};
	struct DBT valt = {0, 0};
	int rc = db->select(db, &keyt, &valt);
	*val = valt.data;
	*val_len = valt.size;
	return rc;
}

int db_put(struct DB *db, void *key, size_t key_len,
	   void *val, size_t val_len) {
	struct DBT keyt = {
		.data = key,
		.size = key_len
	};
	struct DBT valt = {
		.data = val,
		.size = val_len
	};
	return db->insert(db, &keyt, &valt);
}


// -------------------------------------- 
int btree_write_existing_node(struct DB *db , size_t page_num, struct BNode * Node)
{
    return 0;
    
}

int btree_write_new_node(struct DB *db , struct BNode * Node)
{
    return 0;
    
}


int btree_read_node(struct DB *db , size_t page_num, struct BNode * Node)
{
    //TODO Read Errors
    fread(&page_num,4,1,db->db_file);
    long int offset = db->conf->page_size * page_num;
    fseek(db->db_file,offset,SEEK_SET);
    fread(&(Node->is_leaf),4,1,db->db_file);
    fread(&(Node->page_num),4,1,db->db_file);
    fread(&(Node->pairs_num),4,1,db->db_file);
    void * data = malloc(db->conf->page_size);
    size_t * links = (size_t*)malloc((Node->pairs_num + 1)*sizeof(size_t));
    fread(&(links[0]),4,1,db->db_file);
    size_t data_offset = 0;
    int i = 0;
    void * key_link, *data_link;
    struct Record * records = (struct Record*)malloc(Node->pairs_num * sizeof(struct Record));
    for(i=0;i<Node->pairs_num;i++)
    {
        fread(&(data[data_offset]),4,1,db->db_file);
        size_t key_size = ((size_t *)(data))[data_offset];
        data_offset += 4;
        fread(&(data[data_offset]),key_size,1,db->db_file);
        key_link = (void*)(&(data[data_offset]));
        data_offset += key_size;
        fread(&(data[data_offset]),4,1,db->db_file);
        size_t data_size = ((size_t *)(data))[data_offset];
        data_offset += 4;
        fread(&(data[data_offset]),data_size,1,db->db_file);
        data_link = (void*)(&(data[data_offset]));
        data_offset+=data_size;
        fread(&(links[i+1]),4,1,db->db_file);
        records[i].data = data_link;
        records[i].key = key_link;
        records[i].data_size = data_size;
        records[i].key_size = key_size;
    }
    Node->links = links;
    Node->records = records;
    
    return 0;
    
}

int btree_get_root(struct DB *db, struct BNode * Root)
{
    if(btree_read_node(db,db->root_page_num,Root) != 0)
    {
        //Some Read Error.
        return -1;
    }
    return 0;
}

int btree_search(struct DB *db, struct DBT *key, struct DBT *data)
{
    struct BNode * Root = (struct BNode *)malloc(sizeof(struct BNode));
    btree_get_root(db,Root);
    struct BNode * cur_node, *child_node;
    cur_node = Root;
    while(cur_node -> is_leaf != 1)
    {
        //Search for the key or go down the tree
        int i=0;
        for(i=0;i<cur_node->pairs_num;i++)
        {
            size_t min = key->size;
            if(min > cur_node->records[i].key_size)
                min = cur_node->records[i].key_size;
            int cmp_res = memcmp(key->data,cur_node->records[i].key,min);
            if(cmp_res == 0 && key->size == cur_node->records[i].key_size)
            {
                //Key found
            }
        }      
    }
    if(cur_node -> is_leaf == 1)
    {
        //Check the leaf 
    }
    
    return 0;
}



// --------------------------------------


struct DB *dbopen(char *file, struct DBC *conf)
{
    FILE * f = fopen(file, "r");
    if(f == NULL)
    {
        //Creating DB file
        f = fopen(file,"w");
    }
    struct DB dbs = {
		.close = db_close,
                .delete = db_delete,
                .db_filename = file,
                .conf = conf,
                .db_file = f
	};
    return &dbs;
}

int main(int argc, char** argv) {

    struct DBC myconf = {
    .db_size = 524288,
    .page_size = 4,
    .cache_size = 0
    };
    struct DB * my_db = dbopen("myfile", &myconf);
    //my_db -> close(my_db);
    return 0;
}
