import os;
path ="."  
count = 0
for file in os.listdir(path):
	if(file.startswith("ce")):	
		os.rename(file, str(count))
		count = count+1
