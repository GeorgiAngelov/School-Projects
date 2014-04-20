#define ROW 100
#define COL 60
#define TOTAL_FILES 79
#define VECTOR_SIZE 29
#define NUMBER_OF_TEST_VECTORS 2

typedef struct _message_to_worker{
	int N;
	int seed;
	char file_names[ROW][COL];
}message_to_worker;
/*
typedef struct _return_message_to_master{
	int rank;
	int time;
	int file_num;
	float* return_result;
}message_to_master;
*/
int tag = 123;
