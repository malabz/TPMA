# TPMA: a two pointers meta-alignment tool to ensemble different nucleic acid multiple sequence alignment results

TPMA is a tool written in C++20 for combining and refining multiple MSA results using a two-pointer algorithm. It runs on Linux.

## 🔨Installation and Usage

### 1.1 OSX/Linux/WSL(Windows Subsystem for Linux ) - from Anaconda
1.Install WSL for Windows. Instructional video [1](https://www.youtube.com/watch?v=X-DHaQLrBi8&t=5s) or [2](http://lab.malab.cn/%7Etfr/1.mp4) (Copyright belongs to the original work).

2.Download and install Anaconda. Download Anaconda for different systems [here](https://www.anaconda.com/products/distribution#Downloads). Instructional video of anaconda installation [1](https://www.youtube.com/watch?v=AshsPB3KT-E) or [2](http://lab.malab.cn/%7Etfr/Install_anaconda_in_Linux.mp4) (Copyright belongs to the original work).

3.Install TPMA.
```bash
#1 Create and activate a conda environment for TPMA
conda create -n tpma_env
conda activate tpma_env

#2 Add channels to conda
conda config --add channels malab

#3 Install TPMA
conda install -c malab tpma

#4 Test TPMA
tpma -h
```

### 1.2 OSX/Linux/WSL(Windows Subsystem for Linux ) - from the source code

1. Download and Compile the source code. (Make sure your version of gcc >= 9.4.0 or clang >= 13.0.0)
```shell
#1 Download
git clone https://github.com/malabz/TPMA.git

#2 Open the folder
cd TPMA

#3 Compile
make

#4 Test TPMA
./tpma -h
```

### 2 Usage
```
./tpma -a <initial_alignments_list> -r <raw_data> -o <output>
   -a is used to specify the paths of all initial alignments to be merged
   -r is used to specify the path of raw data, a file in FASTA format
   -o is used to specify the output for TPMA result, a file in FASTA format
   -h print help message
```

## 🔬Test dataset and the use case
### 1. Information about the [test dataset](https://github.com/malabz/TPMA/tree/main/data)

Dataset|Sequences Num|Repeats Num|Avg Length|Similarity
:---:|:---:|:---:|:---:|:---:
mt genomes|30|4|about 16568bp|The average similarity is about 99.7%
HVS-II|10|10|about 365bp|The average similarity is about 98.6%
16S rRNA|100|8|about 1440bp|The average similarity is about 74.6%
23S rRNA|64|10|about 3113bp|The average similarity is about 92.7%
SARS-CoV-2_20200301|39|4|about 29858bp|The average similarity is about 99.8%
SARS-CoV-2_20200417|100|4|about 27623bp|The average similarity is about 85.3%
16s simu|100|3|about 1550bp|14 sets of data with different similarities (99%, 98%, 97%, 96%, 95%, 94%, 93%, 92%, 91%, 90%, 85%, 80%, 75%, 70%)
23s simu|100|3|about 3900bp|14 sets of data with different similarities (99%, 98%, 97%, 96%, 95%, 94%, 93%, 92%, 91%, 90%, 85%, 80%, 75%, 70%)
CIPRES-128|255|10|about 1550bp|The average similarity is about 80%
CIPRES-256|511|10|about 1550bp|The average similarity is about 80%


### 2. The use case
```shell
# Download data
wget http://lab.malab.cn/~zyx/tools/TPMA/data/16s_rRNA.tar.gz

# Unzip data
tar -zxvf 16s_rRNA.tar.gz

# Get the folder path
cd 16s_rRNA

# Run TPMA
./tpma -a msa_results/16s_rRNA_100seq_rep1/16s_rRNA_100seq_rep1_clustalw2.fasta msa_results/16s_rRNA_100seq_rep1/16s_rRNA_100seq_rep1_mafft.fasta msa_results/16s_rRNA_100seq_rep1/16s_rRNA_100seq_rep1_muscle3.fasta msa_results/16s_rRNA_100seq_rep1/16s_rRNA_100seq_rep1_tcoffee.fasta -r raw_data/16s_rRNA_100seq_rep1.fasta -o 16s_rRNA_100seq_rep1_tpma_c4.fasta 
```
## 📍Reminder
1. Currently TPMA is **ONLY** available for DNA/RNA. 
2. The application of TPMA assumes that the sequences' IDs within the MSAs are unique.
(E.g. Due to the excessively long length of the sequence IDs in the original data set, Clustal format(the default output format of ClustalW2/PCMA/POA/T-Coffee) may truncate the IDs, resulting in consistent IDs in the alignment output that TPMA cannot process. If the IDs in the original data are too long, we suggest manually renumbering them before using MSA software).
3. Convert Clustal format to FASTA format using tool: [aln2fasta](https://github.com/malabz/MSATOOLS/tree/main/format-converter)
4. TPMA will sort the sequences, but we recommend that the input sequences be in the same order as the original file.
5. TPMA will remove any bad characters that are present in the input data.

## 🖥️Environment
System|GCC version
:---:|:---:
OSX|clang 13.0.0
Linux|GCC 9.4.0
WSL|GCC 9.4.0

## 🔖Citation


## 👋Contacts
The software tools are developed and maintained by 🧑‍🏫[ZOU's lab](http://lab.malab.cn/~zq/en/index.html).

If you find any bug, welcome to contact us on the [issues page](https://github.com/malabz/TPMA/issues) or email us at 👉[📩](zhai1xiao@gmail.com).

More tools and infomation can visit our [github](https://github.com/malabz).
