# 5-List-Colorability Of Toroidal Graphs

Code for the article "Towards Characterization of 5-List-Colorability of Toroidal Graphs"

## Usage

In order to compile a program, use `make name_of_program` in the code directory, which will create an executable file with name `name_of_program`. In order to compile a version of the program using multithreading, append `_parallel` to the end of the program name. You can configure the number of threads in the `Parallelism.hh` file. 

### Chordless Critical Cycle-Canvases

Program `canvas_search` (`canvas_search_parallel`) reads an integer `l` from stdin and outputs the list of chordless critical cycle-canvases of cycle length `l` (including empty cycle).

### Critical Cycle-Canvases with Short Chords

Program `canvas_search_with_size_3_4_chords` (`canvas_search_with_size_3_4_chords_parallel`) reads an integer `l` from stdin and outputs the list of critical cycle-canvases of cycle length `l` which have at least one short chord, where a short chord is one where one of the critical canvases resulting from dividing the canvas by the chord has cycle length 3 or 4 (the ones necessary for critical prism-canvases with d >= 2).

### Critical Prism-Canvases

Program `canvas_to_triangles` (`canvas_to_triangles_parallel`) reads a list of critical cycle-canvases of cycle length 2d+6 and returns the resulting list of critical prism-canvases with distance between precolored triangles d. 

### Gluing Critical Prism-Canvases 

Program `triangle_fusion_verifier` (`triangle_fusion_verifier_parallel`) reads a list of critical prism-canvases and prints the critical prism-canvases obtained by gluing two prism-canvases of the list that do not belong to the list themselves. 

### Critical Graphs on the Torus

Program `triangle_identification_torus` reads a list of critical prism-canvases and checks if any of the 6-list-critical graphs on the torus obtained by identifying the two triangles does not contain any of Thomassen's obstructions.


### Canvas Format

In the case of multithreaded programs, canvases are printed in a non-deterministic order. Program `sort_codes` reads a list of canvases and prints it lexicographically sorted and appending the number of canvases to the beginning of the file. 

Cycle-canvases and prism-canvases are printed in a compact, machine-readable "standard form". Program `unpack_canvases` reads a list of cycle-canvases in that (with the total number of canvases at the beginning) and prints a more human-readable adjacency list format. Program `unpack_twotrianglegraphs` does the same for prism-canvases. 

 

## Acknowledgements

Uses the [parallel-hashmap](https://github.com/greg7mdp/parallel-hashmap) library. 
