gcc -o 3h 3H.c

for i in 100 200 300 400 500
do

	for j in {00..10}
	do
		for k in {1..10}
		do
			echo "----------------RUN $i $j $k"
			./3h $i $j $k
		done
	done
done
