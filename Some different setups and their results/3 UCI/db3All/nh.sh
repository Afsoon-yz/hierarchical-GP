gcc -o 3nh 3NH.c

for i in 100 200 300 400 500
do

	for j in {1..10}
	do
			echo "----------------RUN $i $j"
			./3nh $i $j
	done
done
