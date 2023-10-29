DEBUGFLAGS=-g -fsanitize=undefined,address -Wall -DDEBUG
DEBUGGERFLAGS=-g -Wall -DDEBUG
PERFORMFLAGS=-O2 -Wall
MEMORYPROFILEFLAGS=-O2 -Wall -ltcmalloc
PARALLELFLAGS=-O2 -Wall -DPARALLEL -pthread 
PARALLELDEBUGFLAGS=-O2 -Wall -DPARALLEL -pthread -g -fsanitize=thread
SRC_FILES := $(wildcard *.cpp)


canvas_search: main_canvas_search_lessmemory.cc $(SRC_FILES)
	g++ $(PERFORMFLAGS) -o canvas_search main_canvas_search_lessmemory.cc *.cpp

canvas_search_parallel: main_canvas_search_lessmemory.cc $(SRC_FILES)
	g++ $(PARALLELFLAGS) -o canvas_search_parallel main_canvas_search_lessmemory.cc *.cpp

canvas_search_parallel_debug: main_canvas_search_lessmemory.cc $(SRC_FILES)
	g++ $(PARALLELDEBUGFLAGS) -o canvas_search_parallel_debug main_canvas_search_lessmemory.cc *.cpp

canvas_search_readprev: main_canvas_search_readprev.cc $(SRC_FILES)
	g++ $(PARALLELFLAGS) -o canvas_search_readprev main_canvas_search_readprev.cc *.cpp

canvas_search_with_size_3_4_chords: main_canvas_search_with_size_3_4_chords.cc $(SRC_FILES)
	g++ $(PERFORMFLAGS) -o canvas_search_with_size_3_4_chords main_canvas_search_with_size_3_4_chords.cc *.cpp

canvas_search_with_size_3_4_chords_parallel: main_canvas_search_with_size_3_4_chords.cc $(SRC_FILES)
	g++ $(PARALLELFLAGS) -o canvas_search_with_size_3_4_chords_parallel main_canvas_search_with_size_3_4_chords.cc *.cpp

canvas_to_triangles: main_canvas_to_triangles.cc $(SRC_FILES)
	g++ $(PERFORMFLAGS) -o canvas_to_triangles main_canvas_to_triangles.cc *.cpp

canvas_to_triangles_parallel: main_canvas_to_triangles_parallel.cc $(SRC_FILES)
	g++ $(PARALLELFLAGS) -o canvas_to_triangles_parallel main_canvas_to_triangles_parallel.cc *.cpp

canvas_to_triangles_parallel_debug: main_canvas_to_triangles_parallel.cc $(SRC_FILES)
	g++ $(PARALLELDEBUGFLAGS) -o canvas_to_triangles_parallel_debug main_canvas_to_triangles_parallel.cc *.cpp

triangle_fusion_verifier: main_triangle_fusion_verifier.cc $(SRC_FILES)
	g++ $(PERFORMFLAGS) -o triangle_fusion_verifier main_triangle_fusion_verifier.cc *.cpp

triangle_fusion_verifier_parallel: main_triangle_fusion_verifier_parallel.cc $(SRC_FILES)
	g++ $(PARALLELFLAGS) -o triangle_fusion_verifier_parallel main_triangle_fusion_verifier_parallel.cc *.cpp

unpack_canvases: main_unpack_canvases.cc $(SRC_FILES)
	g++ $(PERFORMFLAGS) -o unpack_canvases main_unpack_canvases.cc *.cpp

unpack_twotrianglegraphs: main_unpack_twotrianglegraphs.cc $(SRC_FILES)
	g++ $(PERFORMFLAGS) -o unpack_twotrianglegraphs main_unpack_twotrianglegraphs.cc *.cpp

sort_codes: main_sort_codes.cc $(SRC_FILES)
	g++ $(PERFORMFLAGS) -o sort_codes main_sort_codes.cc *.cpp

triangle_identification_torus: main_triangle_identification_torus.cc $(SRC_FILES)
	g++ $(PERFORMFLAGS) -o triangle_identification_torus main_triangle_identification_torus.cc *.cpp
