# OS3
OS_Linux_P3

Arguments: project3 <input-file> <n> <b>

The statistical outcomes are shown in output.txt after the program finished.

Description:

In this project,  a parallel program for “word counting” is implemented. The inspiration for this parallel program comes from Google’s MapReduce. This program would be a highly simplified version of Google’s MapReduce. But it will still give some insights into how companies like Google exploit parallelism when carrying out “big data” processing. (e.g., to create search results based on data crawled from the web). The goal of this program is to count the number of times each unique word occurs in a given input file. One significant simplification is the following: whereas a real-world MapReduce application will generally run on multiple machines (and hence would have to be constructed using multiple processes), this program will run on a single machine (and will consist of multiple threads that are part of the same address space).
