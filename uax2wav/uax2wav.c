#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	int len;
	int i;
	int wavCount = 0;
	int wavStart;
	FILE* uax;
	FILE* exportFile;
	char *buffer;
	char exportName[8];

	if (argc != 2) {
		fprintf(stderr,"uax2wav: no input files\n");
		exit(1);
	}

	if (!(uax = fopen(argv[1],"rb"))) {
		fprintf(stderr,"uax2wav: %s: no such file\n",argv[1]);
		exit(1);
	}

	fseek(uax,0,SEEK_END);
	len = ftell(uax);
	rewind(uax);
	buffer = (char*) malloc(len);
	if (fread(buffer,1,len,uax) != len) {
		fprintf(stderr,"uax2wav: error: can't read file %s\n",argv[1]);
		exit(1);
	}
	fclose(uax);

	for (i = 0; i < len-4; i++) {
		if (buffer[i]=='R' && buffer[i+1]=='I' && buffer[i+2]=='F' && buffer[i+3]=='F') {
			wavCount++;
			wavStart = i;
			i++;
			while (i < len-4) {
				if (buffer[i]=='R' && buffer[i+1]=='I' && buffer[i+2]=='F' && buffer[i+3]=='F') break;
				i++;
			}
			if (i == len-4) i = len;
			sprintf(exportName,"%d.wav",wavCount);
			if (!(exportFile = fopen(exportName,"wb"))) {
				fprintf(stderr,"uax2wav: error: can't create file %s",exportName);
				exit(1);
			}
			fwrite(buffer+wavStart,1,i-wavStart,exportFile);
			fclose(exportFile);
			printf("\t%s: %d B\n",exportName,i-wavStart);
		}
	}
	
	printf("uax2wav: %s: exported %d WAV audio files\n",argv[1],wavCount);
	return 0;
}
