#include "stdio.h"
#include "dirent.h"
int main() {
   // These are data types defined in the "dirent" header
   struct dirent *next_file;
   DIR *theFolder;

   char filepath[256];

   theFolder = opendir("test");

   while(next_file = readdir(theFolder))   {
 // build the full path for each file in the folder
			sprintf(filepath, "%s/%s", "test", next_file->d_name);
    	remove(filepath);
   }
   return 0;
}
