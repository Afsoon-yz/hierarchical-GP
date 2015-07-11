# hierarchical-GP
The code and data for hierarchical and non-hierarchical GP

This repository contains all the material to replicate the results provided in paper "A Genetic Programming Approach to Cost-Sensitive Control in Resource Constrained Sensor Systems" poblished in GECCO ’15, July 11 - 15, 2015, Madrid, Spain.

There are four folders and two code files in this repository:

1- Convert Synthesized data.
This folder helps you to create synthesized data the way we did for this paper.
You just need to change j and k variables to determine the number of training and testing records, and then symply compile and run the code.

>> gcc -o create dataCreator.c
>> ./create

2- Convert UCI data
This folder containes the original datasets, and the converted testing and training datasets we used. In this folder the datasets are sorted based on the cost of the features and then are separated to two parts in testing and training files.

3- Some different setups and their results
provides some examples of different setups we used to generate the results. the readme file in this folder explaines who to interpret each file name.

4- some part of results we got
This folder contais some part of the results we used to generate the figures in the paper. 

5- Commented NH code.
this is the file to train the non-hierarchical model and then test the model on the testing data.
There are some variables you should setup in the code to use it for different datasets. These variables and commands are commented in the file for your ease.
After changing the variables according to your dataset, can simply compile and run the code without any compile or run time variables.

>> gcc -o NH NH.c
>> ./NH

6- Commented H code
this is the file to train the hierarchical model and then test the model on the testing data.
There are some variables you should setup in the code to use it for different datasets. These variables and commands are commented in the file for your ease.
After changing the variables according to your dataset, can simply compile and run the code without any compile or run time variables.

>> gcc -o H H.c
>> ./H

If you have any problem or question regarding this files and paper, please do not hesitat to reach me at ayousef1@uvm.edu

Thank you.