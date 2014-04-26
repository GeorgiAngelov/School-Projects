#define ROW 100
#define COL 60
#define TOTAL_FILES 6
#define VECTOR_SIZE 29
#define NUMBER_OF_TEST_VECTORS 1

typedef struct _message_to_worker{
	int N;
	int seed;
	char file_names[ROW][COL];
}message_to_worker;

typedef struct _message_to_master_item{
	float x;
	float y;
	float offset;
	float dist;
	float time;
}message_to_master;
int tag = 123;
