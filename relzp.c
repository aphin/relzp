// Program for making .zp list from relay switching list (ExPro)
// compile with gcc -o relzp relzp.c -std=c99

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// ---- global variables ----
	int zp[3600][5];
	int nzp=0;

// relay data declaration
	struct relay_item
	{
		int value;
		int pos;
	};
	struct relay_item rl[86401];
	int nrl=0;
//---- end ----

int time_to_sec(char *dt)	// converts time (HH:MM:SS) to count of seconds
{
	int hr;
	int mn;
	int sc;
	int rs;
	sscanf(dt,"%d:%d:%d",&hr,&mn,&sc);
	rs = sc + (mn*60) + (hr*3600);
	return rs;
}

int main(int argc, char *argv[])
{
	FILE *fi;
	FILE *fo;
	char *fon;
	char zps[9]="_zp.txt";
	int i;
	int ppat[4];
	int npat[4];

	if (argc != 4)
	{
		printf("Wrong arguments!");
		exit(0);
	}
	fi = fopen(argv[1],"rt");
	if (fi==NULL)
	{
		printf("Error opening input file %s!",argv[1]);
		exit(0);
	}
	if (sscanf(argv[2],"%d:%d:%d:%d",&ppat[0],&ppat[1],&ppat[2],&ppat[3])<4)
	{
		printf("Wrong positive pattern!");
		exit(0);
	}
	if (sscanf(argv[3],"%d:%d:%d:%d",&npat[0],&npat[1],&npat[2],&npat[3])<4)
	{
		printf("Wrong negative pattern!");
		exit(0);
	}

	char dat_time[10];
	int n1;
	int n2;

	while (!feof(fi))
	{
		fscanf(fi,"%8s\t%d\t%d\t%d\n",dat_time,&n1,&n2,&rl[nrl].value);
		rl[nrl].pos = time_to_sec(dat_time);
		rl[nrl].value &= 0x1C; // discard heat bits
		nrl++;
	}
	fclose(fi);

	i = 0;
	while (i<nrl)
	{
		if (rl[i].value == ppat[0])		// positive pattern search
			if ((i+3) < nrl)
				if ((rl[i+1].value == ppat[1])&&(rl[i+2].value == ppat[2])&&(rl[i+3].value == ppat[3]))	// positive pattern found
				{
					zp[nzp][0] = rl[i].pos;
					zp[nzp][1] = rl[i+3].pos;
					zp[nzp][2] = rl[i+1].pos - rl[i].pos;
					zp[nzp][3] = rl[i+2].pos - rl[i].pos;
					zp[nzp][4] = 0;
					nzp++;
					i += 4;
					continue;
				}
		if (rl[i].value == npat[0])	// negative pattern search
			if ((i+3) < nrl)
				if ((rl[i+1].value == npat[1])&&(rl[i+2].value == npat[2])&&(rl[i+3].value == npat[3]))	// negative pattern found
				{
					zp[nzp][0] = rl[i].pos;
					zp[nzp][1] = rl[i+3].pos;
					zp[nzp][2] = rl[i+1].pos - rl[i].pos;
					zp[nzp][3] = rl[i+2].pos - rl[i].pos;
					zp[nzp][4] = 1;
					nzp++;
					i += 4;
					continue;
				}
		// if nothing found
		i++;
	}

	fon=malloc(strlen(argv[1])+10);
	strncpy(fon,argv[1],strlen(argv[1]));
	sprintf(fon,"%s%s",fon,zps);
	fo = fopen(fon,"wt");
	if (fo==NULL)
	{
		printf("Error opening output file %s!",fon);
		exit(0);
	}

	for (i=0;i<nzp;i++)
	{
		fprintf(fo,"%d\t%d\t%d\t%d\t%d\n",zp[i][0],zp[i][1],zp[i][2],zp[i][3],zp[i][4]);
	}
	fclose(fo);
	return 0;
}
