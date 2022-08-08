# dpEnsemble

dpEnsemble is a tool for combine multiple sequence alignment. It is based on double pointer. The program is written in C++20 and works on Linux.

## Installation and Usage
### 1.1 OSX/Linux/WSL(Windows Subsystem for Linux ) - from the source code
1.Intall WSL for Windows. Instructional video [1](https://www.youtube.com/watch?v=X-DHaQLrBi8&t=5s) or [2](http://lab.malab.cn/%7Etfr/1.mp4) (Copyright belongs to the original work).

```
git clone https://github.com/malabz/dpEnsemble.git
cd dpEnsemble
make
```
### Command-line and options
```
./dpEnsemble -r <raw_data> -c <child_msa_alignment> -o <output> 

	-r is the raw data, a FASTA file
	-c is the align result path of child MSAs, a TXT file
	-o is the output file, a FASTA file
	-h print help message
```
## Contacts
If any bugs during you run our code, please email to zhai1xiao@gmail.com
