all: child bad_child_seg_fault bad_child_free

child: fork_example.cpp
	g++ -o child fork_example.cpp

bad_child_seg_fault: fork_example_segfault.cpp
	g++ -o bad_child_seg_fault fork_example_segfault.cpp

bad_child_free: fork_example_bad_free.cpp
	g++ -o bad_child_free fork_example_bad_free.cpp

clean:
	rm bad_child_seg_fault bad_child_free child

