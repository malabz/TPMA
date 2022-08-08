# dpEnsemble

dpEnsemble is a tool for combine multiple sequence alignment. It is based on double pointer. The program is written in C++20 and works on Linux.

## Installation and Usage

### 1.1 OSX/Linux/WSL(Windows Subsystem for Linux ) - from the source code

1. Intall WSL for Windows. Instructional video [1](https://www.youtube.com/watch?v=X-DHaQLrBi8&t=5s) or [2](http://lab.malab.cn/%7Etfr/1.mp4) (Copyright belongs to the original work).
2. Download and Compile source code. (Make sure your the version of gcc >= 9.4.0 or clang >= 13.0.0)
```shell
git clone https://github.com/malabz/dpEnsemble.git
cd dpEnsemble
make
```

### 1.2 Usage for the source code
```
./dpEnsemble -r <raw_data> -c <child_msa_alignment> -o <output> 

	-r is the raw data, a FASTA file
	-c is the align result path of child MSAs, a TXT file
	-o is the output file, a FASTA file
	-h print help message
```

### 1.3 Example usage for the source code
1. Download test data.
2. Run dpEnsemble.
```
# Download data
wget 
# 
# Run dpEnsemble
./dpEnsemble -r <raw_data> -c <child_msa_alignment> -o <output> 

```

## Test Data Summary
Dataset|Count|Avg Length|Intro
:---:|:---:|:---:|:---:
23s simu|100|about 3900bp|
:---:|:---:|:---:|:---:
16s rRNA|100|about 1500bp|100 sequences were randomly selected from 16s rRNA small dataset, 8 times

## Test Environment
System|OSX|Linux|WSL
:---:|:---:|:---:|:---:
GCC version|clang 13.0.0|GCC 9.4.0|GCC 9.4.0

## Contacts
If any bugs during you run our code, please email to zhai1xiao@gmail.com
