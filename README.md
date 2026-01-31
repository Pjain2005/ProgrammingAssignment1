# ProgrammingAssignment1

TEAM MEMBERS:
    Alicia Ellis, UFID: 79954495
    Priyanka Jain, UFID: 31478022

FILES AND FOLDERS:
    1. src/PA1.cpp contains the full program
    2. data/ is used for Task C timing output CSV files
    3. data/taskc_inputs/ contains the Task C generated input and output files
    4. Our task C graph and reasoning are located in C Programming Assignment 1- Task C.pdf.
    5. tests/ contains additional verifier test cases (one unstable and one invalid). These are separate from the data/ folder.

COMPILATION INSTRUCTIONS:
    1. Use a compiler like MinGW.
    2. In command prompt or the terminal, cd into the src directory:
        cd ./ProgrammingAssignment1/src
    2. Run the following compile and link command:
        g++ PA1.cpp -o program

RUN INSTRUCTIONS:
    1. While in command prompt/terminal, run the executable with one of the following commands:
        ./program match <input_file>
        ./program verify <input_file> <output_file>
    <input_file> refers to any valid preference input file, and <output_file> is the corresponding matching output file (e.g., invalid.in -> invalid.out).
    2. The available input files to find matches for are:
        example_1.in: This file contains the input provided to us in the Canvas description for the assignment.
        invalid.in
        unstable.in
    3. When the match function finishes running, it will have written an output file with a name in this format:
        <input file name>.out
    The matcher writes its output to a file with the same base name as the input file and a .out extension. For example, running the matcher on example_1.in will produce example_1.out. To avoid confusion, our program also prints the name of the output file it generated.

TIMING OUTPUT (TASK C):
    When running the matcher or verifier, the program records execution time information.
    These results are automatically appended to CSV files in the data folder:
        data/matcher_times.csv   - timing results for the matcher
        data/verifier_times.csv  - timing results for the verifier
    Each row corresponds to a run with a specific n value and records the total runtime
    as well as time spent in major stages of execution.

TEST FILES:
    The tests folder contains additional input and output files used to test edge cases.
    These include one unstable matching case and one invalid matching case.
    These files are intended to be used with the verify command only and are separate
    from the data folder used for timing experiments.

ASSUMPTIONS:
    1. n > 0 and is always included in input file.
    2. Hospitals and students will be represented by integers.
    3. Files do not include empty lines.
