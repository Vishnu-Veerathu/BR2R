rm results.txt
make tog_classifier
make result_counter
for file in ../../data/tog/*
do
	# echo $file >> results.txt
	./tog_classifier < $file >> results.txt
done
./result_counter < results.txt
