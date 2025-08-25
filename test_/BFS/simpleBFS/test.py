import sys
import csv

file = sys.argv[1]
time_list = []
with open(file, newline='') as csvfile:
	rdr = csv.reader(csvfile, delimiter=' ')	
	for row in rdr:
		if len(row) > 0:
			time_list.append([row[2]])
with open(file, 'w', newline='') as csvfile:
	wrtr = csv.writer(csvfile)
	
	wrtr.writerows(time_list)

