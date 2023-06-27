# TPRA: A refinement tool for ensembling different multiple sequence alignment results

TPRA is a tool written in C++20 for combining and refining multiple MSA results using a two-pointer algorithm. It runs on Linux.

## 🔨Installation and Usage

### 1.1 OSX/Linux/WSL(Windows Subsystem for Linux ) - from Anaconda
1.Install WSL for Windows. Instructional video [1](https://www.youtube.com/watch?v=X-DHaQLrBi8&t=5s) or [2](http://lab.malab.cn/%7Etfr/1.mp4) (Copyright belongs to the original work).

2.Download and install Anaconda. Download Anaconda for different systems [here](https://www.anaconda.com/products/distribution#Downloads). Instructional video of anaconda installation [1](https://www.youtube.com/watch?v=AshsPB3KT-E) or [2](http://lab.malab.cn/%7Etfr/Install_anaconda_in_Linux.mp4) (Copyright belongs to the original work).

3.Install TPRA.
```bash
#1 Activate one of your conda environment
conda activate base

#2 Add channels to conda
conda config --add channels malab

#3 Install TPRA
conda install -c malab tpra

#4 Test TPRA
tpra -h
```

### 1.2 OSX/Linux/WSL(Windows Subsystem for Linux ) - from the source code

1. Download and Compile the source code. (Make sure your version of gcc >= 9.4.0 or clang >= 13.0.0)
```shell
#1 Download
git clone https://github.com/malabz/TPRA.git

#2 Open the folder
cd TPRA

#3 Compile
make

#4 Test TPRA
./tpra -h
```

### 2 Usage
```
./tpra -a <folder_for_initial_alignments> -r <raw_data> -o <output>
   -a is used to specify the folder containing the initial alignments, the path to the folder
   -r is used to specify the raw data, a file in FASTA format
   -o is used to specify the output file for TPRA result, a file in FASTA format
   -h print help message
```

## 🔬Test dataset and the use case
### 1. Information about the [test dataset](https://github.com/malabz/TPRA/tree/main/data)

Dataset|Sequences Num|Repeats Num|Avg Length|Similarity
:---:|:---:|:---:|:---:|:---:
16s simu|100|3|about 1550bp|14 sets of data with different similarities (99%, 98%, 97%, 96%, 95%, 94%, 93%, 92%, 91%, 90%, 85%, 80%, 75%, 70%)
23s simu|100|3|about 3900bp|14 sets of data with different similarities (99%, 98%, 97%, 96%, 95%, 94%, 93%, 92%, 91%, 90%, 85%, 80%, 75%, 70%)
RNA-255|255|10|about 1550bp|The average similarity is about 80%
RNA-511|511|10|about 1550bp|The average similarity is about 80%
16S rRNA|100|8|about 1550bp|The average similarity is about 75%
HVS-II|10|10|about 400bp|The average similarity is about 96%
23S rRNA|64|10|about 3140bp|The average similarity is about 95%
mt1x|30|4|about 16568bp|The average similarity is about 99.7%
SARS-CoV-2_20200301|39|4|about 29860bp|The average similarity is about 99.8%
SARS-CoV-2_20200417|100|4|about 27800bp|The average similarity is about 85%

### 2. The use case
```shell
# Download data
wget http://lab.malab.cn/~zyx/tools/TPRA/data/16s_rRNA.tar.gz

# Unzip data
tar -zxvf 16s_rRNA.tar.gz

# Get the folder path
cd 16s_rRNA/align_result/16s_rRNA_100_1
pwd # get the PATH

# Run TPRA
./tpra -a PATH/ -r 16s_rRNA/raw_data/16s_rRNA_100_1.fasta -o 16s_rRNA_100_1_tpra.fasta 
```
## 💡Reminder
1. Currently TPRA is **ONLY** available for DNA/RNA. 
2. The application of TPRA assumes that the sequences' IDs within the MSAs are unique.
(E.g. Due to the excessively long length of the sequence IDs in the original data set, Clustal format(the default output format of ClustalW2/PCMA/POA/T-Coffee) may truncate the IDs, resulting in consistent IDs in the alignment output that TPRA cannot process. If the IDs in the original data are too long, we suggest manually renumbering them before using MSA software).
3. TPRA will sort the sequences, but we recommend that the input sequences be in the same order as the original file.
4. TPRA will remove any bad characters that are present in the input data.

## 🖥️Environment
System|GCC version
:---:|:---:
OSX|clang 13.0.0
Linux|GCC 9.4.0
WSL|GCC 9.4.0

## 🔖Citation


## 👋Contacts
The software tools are developed and maintained by 🧑‍🏫[ZOU's lab](http://lab.malab.cn/~zq/en/index.html).

If you find any bug, welcome to contact us on the [issues page](https://github.com/malabz/TPRA/issues) or email us at 👉[📩](zhai1xiao@gmail.com).

More tools and infomation can visit our [github](https://github.com/malabz).
