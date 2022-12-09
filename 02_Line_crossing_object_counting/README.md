**02_Line_crossing_object_counting**

### Installing the application

~~~ 
make
~~~

### Command to run the application

~~~ 
cd exe/
./02_Line_crossing_object_counting person 150 0 350 450 1
~~~

### Image for the sample above command

<img src="./sample_office.png" alt="Sample application output"
     margin-right=10px; 
     width=600px;
     height=334px />


### What's the command

**<application_name> <class_name> <line_pointx1> <line_pointy1> <line_pointx2> <line_pointy2> <flow_direction>**

1. Applicaiton name :- 02_Line_crossing_object_counting
2. Class_name :- person,car,cat,dog...(any class from coco dataset)(string dtype)
3. line_pointx1 :- xmin for the line (int dtype)
4. line_pointx1 :- ymin for the line (int dtype)
5. line_pointx1 :- xmax for the line (int dtype)
6. line_pointx1 :- ymax for the line (int dtype)
7. flow_direction :- 0/1 (0 for left-right/ 1 for right-left) (int dtype)
     

