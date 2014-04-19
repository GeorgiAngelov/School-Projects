#define ROW 100
#define COL 60

typedef struct _message_to_worker{
	int N;
	int seed;
	char file_names[ROW][COL];
}message_to_worker;

int tag = 123;