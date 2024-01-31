# Guide for T-Coffee Compatible with MUSCLE5

## Prepare the script to call MUSCLE5
1. Create a new folder named bin
```shell
$ mkdir bin
$ cd bin
```
2. Write a script that calls MUSCLE5 (you can copy our run_muscle5 script locally)
```shell
$ touch run_muscle5
```
3. Add executable permissions to the script
```shell
$ chmod +x run_muscle5
```
4. Add the script to your PATH environment
```shell
# get your_path
$ pwd
# modify the PATH file
$ vim ~/.bashrc
# add the path
export PATH=your_path:$PATH
# update the PATH file
$ source ~/.bashrc
```

## Modify the source code of T-Coffee to call the script that runs MUSCLE5
1. Add the following code to the **produce_method_file** function of the **lib/util_lib/util_constraints_list.c** file
```cpp
	sprintf (list[n][0], "muscle5_msa");
	sprintf (list[n][1], "%s", vtmpnam(NULL));
	n++;if (method==NULL || strm (method, list[n-1][0])){fp=vfopen (list[n-1][1], "w");
	fprintf ( fp, "DOC Muscle [%s]\n", MUSCLE_ADDRESS);
	fprintf ( fp, "EXECUTABLE run_muscle5\n");
	fprintf ( fp, "ALN_MODE   multiple\n");
	fprintf ( fp, "OUT_MODE   aln\n");
	fprintf ( fp, "IN_FLAG    -in&bnsp\n");
	fprintf ( fp, "OUT_FLAG   -out&bnsp\n");
	fprintf ( fp, "PARAM      &bnsp>/dev/null\n");
	fprintf ( fp, "SEQ_TYPE   S\n");
	fprintf ( fp, "ADDRESS    %s\n", MUSCLE_ADDRESS);
	fprintf ( fp, "PROGRAM    %s\n", MUSCLE_4_TCOFFEE);
	vfclose (fp);}
```
2. After saving the file, recompile T-Coffee

## Instructions
```shell
$ t_coffee test.fa -method=muscle5_msa -outfile=test_mcoffee.aln
```
 
