#include <stdio.h>
#include <stdlib.h>
typedef struct
{
unsigned char B;
unsigned char G;
unsigned char R;

}RGB;


#pragma pack(push,1)

typedef struct
{
    char tag[2];
    int size;
    int unused;
    int offset;


    int bytes;
    int width;
    int height;

    short int color;
    short int nr_bytes;
    int compression;
    int raw_size;
    int pixel_h;
    int pixel_w;
    int nr_color;
    int important;

}header;
#pragma pack(pop)

typedef struct
{
  header header;
  RGB *array;
}BMP;


unsigned int XORSHIFT(unsigned int x)
{
    x=x^x<<13;
    x=x^x>>17;
    x=x^x<<5;
    return x;
}

int random(int k,int *R)
{
    static int pos=1;
    unsigned int r=R[pos++];
    return r%(k+1);

}

int random2(int k,int *R)
{
    static int pos=1;
    unsigned int r=R[pos++];
    return r%(k+1);

}

RGB produs(RGB c, RGB p, RGB r)
{
    RGB rgb;
    rgb.R=c.R^p.R^r.R;
    rgb.G=c.G^p.G^r.G;
    rgb.B=c.B^p.B^r.B;
    return rgb;
}

 BMP *BMPinternal(char *cale_imagine)
{  int size_array;
     int readed=0;
    FILE *fin=fopen(cale_imagine,"rb");
    if(!fin)
    {
        perror("eroare");
        exit(-1);
    }

  BMP *bmp=malloc(sizeof(BMP));
  fread(bmp,sizeof(header),1,fin);

  if(readed==sizeof(header))
  {
    perror("eroare");
    exit(-1);
  }

  fseek(fin,bmp->header.offset,SEEK_SET);
  bmp->array=malloc(bmp->header.raw_size);
  fread(bmp->array,bmp->header.raw_size,1,fin);
  fclose(fin);
  return bmp;
}



void BMPext(char *destination,BMP *bmp)
{
    FILE *fin=fopen(destination,"wb");
    fwrite(bmp,sizeof(header),1,fin);
    fwrite(bmp->array,bmp->header.raw_size,1,fin);

       fclose(fin);
}

RGB *pixel_adress(int i,BMP *bmp)
{
    int w=bmp->header.width;

    int i3=i*3;
    int w3i=i3/w;

    int nr_bytes=w*3;
    int padding=nr_bytes%4;
    return (i3) +w3i*padding +(char*)bmp->array;

}

void change(int *i,int *j,BMP *bmp)
{
    RGB *pi=pixel_adress(*i,bmp);
    RGB *pj=pixel_adress(*j,bmp);

    RGB aux=*pi;
    *pi=*pj;
    *pj=aux;

}

void cript(char *cale_imagine,char *destination,char *key)
{
BMP *bmp=BMPinternal(cale_imagine);
unsigned int R0,SV,i=0,aux,k;
unsigned int w=bmp->header.width;
unsigned int h=bmp->header.height;
RGB *pk,*pk1;

FILE *f=fopen(key,"r");

if(!f)
{
    perror("eroare");
    exit(-1);
}
fscanf(f,"%u",&R0);
fscanf(f,"%u",&SV);
fclose(f);

unsigned int *R=malloc(sizeof(unsigned int)*(2*w*h));

R[0]=R0;
for(i=1;i<(2*w*h);i++)
    R[i]=XORSHIFT(R[i-1]);

    for(i=w*h-1;i>0;i--)
    {
        int r=random(i,R);
        change(&i,&r,bmp);

    }


for(k=0;k<w*h;k++)
{
  pk=pixel_adress(k,bmp);

   union
   {
       unsigned int x;
       RGB r;
   }convert;

   convert.x=R[w*h+k];
   RGB x=convert.r;
   if(k==0)
   {
       convert.x=SV;
       RGB s=convert.r;
       *pk=produs(s,*pk,x);
   }
   else
    {
        *pk=produs(*pk1,*pk,x);
    }
   pk1=pk;

}

BMPext(destination,bmp);

free(bmp->array);
free(R);
free(bmp);

}

void decript(char *cale_imagine,char *destination,char *key)
{
RGB *decrip,*crip;
BMP *bmp=BMPinternal(destination);
unsigned int R0,SV,i=0,aux,k,*p;
unsigned int w=bmp->header.width;
unsigned int h=bmp->header.height;
RGB *pk,*pk1;

FILE *f=fopen(key,"r");
if(!f)
{
    perror("eroare");
    exit(-1);
}
fscanf(f,"%u",&R0);
fscanf(f,"%u",&SV);
fclose(f);

decrip=malloc(sizeof(RGB)*w*h);
unsigned int *R=malloc(sizeof(unsigned int)*(2*w*h));
R[0]=R0;
p=malloc(sizeof(unsigned int)*w*h);


for(i=1;i<(2*w*h);i++)
    R[i]=XORSHIFT(R[i-1]);


  for(k=(w*h-1);k>0;k--)
{
  pk=pixel_adress(k,bmp);

   union
   {
       unsigned int x;
       RGB rb;
   }convert;

   convert.x=R[w*h+k];
   RGB x=convert.rb;
   if(k==0)
   {
       convert.x=SV;
       RGB s=convert.rb;
       *pk=produs(s,*pk,x);
   }
   else
    {
        *pk=produs(*pk1,*pk,x);
   }

    pk1=pk;
}

for(i=0;i<w*h;i++)
  p[i]=i;

    for(i=(w*h-1);i>0;i--)
    {
        int r=random2(i,R);
        change(&p[i],&p[r],bmp);
    }

unsigned int *inv=malloc(sizeof(unsigned int)*(h*w));
for(i=0;i<w*h;i++)
  inv[p[i]]=i;


for(k=0;k<w*h;k++)
{
    pk=pixel_adress(k,bmp);
    decrip[inv[k]].B=pk->B;
    decrip[inv[k]].G=pk->G;
    decrip[inv[k]].R=pk->R;


}


BMPext(destination,bmp);
free(bmp->array);
free(R);
free(crip);
free(decrip);
free(bmp);
}


void test_chi(char *cale_imagine)
{   double chiB=0,chiG=0,chiR=0,f;
    int *fiB,*fiG,*fiR;
    int i=0,j=0;
    BMP *bmp=BMPinternal(cale_imagine);


    unsigned int width=bmp->header.width;
    unsigned int height=bmp->header.height;
    FILE *fin=fopen(cale_imagine,"rb");



    fiB=malloc(sizeof(int)*256);
    fiG=malloc(sizeof(int)*256);
    fiR=malloc(sizeof(int)*256);

    for(;j<256;j++)
    {
    fiG[j]=0;
    fiB[j]=0;
    fiR[j]=0;
    }

    f=width*height/256;

    while(i!=width*height)
    {
        fiG[bmp->array[i].G]++;
        fiB[bmp->array[i].B]++;
        fiR[bmp->array[i].R]++;

        i++;

    }
        for(j=0;j<256;j++)
        {
            chiG+=((fiG[j]-f)*(fiG[j]-f))/f;
            chiB+=((fiB[j]-f)*(fiB[j]-f))/f;
            chiR+=((fiR[j]-f)*(fiR[j]-f))/f;

        }

printf("G:%.2lf\n",chiG);
printf("B:%.2f\n",chiB);
printf("R:%.2f\n",chiR);
free(fiB);
free(fiG);
free(fiR);
fclose(fin);

}





///PARTEA 2

void grayscale_image(char* nume_fisier_sursa,char* nume_fisier_destinatie)
{
   FILE *fin, *fout;
   unsigned int dim_img, latime_img, inaltime_img;
   unsigned char pRGB[3], header[54], aux;

   printf("nume_fisier_sursa = %s \n",nume_fisier_sursa);

   fin = fopen(nume_fisier_sursa, "rb");
   if(fin == NULL)
   	{
   		printf("nu am gasit imaginea sursa din care citesc");
   		return;
   	}

   fout = fopen(nume_fisier_destinatie, "wb+");

   fseek(fin, 2, SEEK_SET);
   fread(&dim_img, sizeof(unsigned int), 1, fin);
   printf("Dimensiunea imaginii in octeti: %u\n", dim_img);

   fseek(fin, 18, SEEK_SET);
   fread(&latime_img, sizeof(unsigned int), 1, fin);
   fread(&inaltime_img, sizeof(unsigned int), 1, fin);
   printf("Dimensiunea imaginii in pixeli (latime x inaltime): %u x %u\n",latime_img, inaltime_img);

   //copiaza octet cu octet imaginea initiala in cea noua
	fseek(fin,0,SEEK_SET);
	unsigned char c;
	while(fread(&c,1,1,fin)==1)
	{
		fwrite(&c,1,1,fout);
		fflush(fout);
	}
	fclose(fin);

	//calculam padding-ul pentru o linie
	int padding;
    if(latime_img % 4 != 0)
        padding = 4 - (3 * latime_img) % 4;
    else
        padding = 0;

    printf("padding = %d \n",padding);

	fseek(fout, 54, SEEK_SET);
	int i,j;
	for(i = 0; i < inaltime_img; i++)
	{
		for(j = 0; j < latime_img; j++)
		{
			//citesc culorile pixelului
			fread(pRGB, 3, 1, fout);
			//fac conversia in pixel gri
			aux = 0.299*pRGB[2] + 0.587*pRGB[1] + 0.114*pRGB[0];
			pRGB[0] = pRGB[1] = pRGB[2] = aux;
        	fseek(fout, -3, SEEK_CUR);
        	fwrite(pRGB, 3, 1, fout);
        	fflush(fout);
		}
		fseek(fout,padding,SEEK_CUR);
	}
	fclose(fout);
}
int intesitate_pixel(int i,int j,char *sablon)
{
    unsigned char pixel[3];
    unsigned int width;
    unsigned int height;

    FILE *f=fopen(sablon,"r");
     if(!f)
     {
        perror("eroare");
        exit(-1);
     }
 fseek(f,18,SEEK_SET);
 fread(&width,sizeof(unsigned int),1,f);
 fread(&height,sizeof(unsigned int),1,f);

 fseek(f, 54, SEEK_SET);
 fseek(f,i*width+j,SEEK_CUR);
 fread(pixel,3,1,f);
 fclose(f);
 return pixel[2];

}

double medie_intesitate(char *sablon)
{   unsigned char pixel[3];
    unsigned int width;
    unsigned int height;
    int i,j,intens;
    long long int S=0;
    FILE *f=fopen(sablon,"r");
     if(!f)
     {
        perror("eroare");
        exit(-1);
     }

fseek(f,18,SEEK_SET);
 fread(&width,sizeof(unsigned int),1,f);
 fread(&height,sizeof(unsigned int),1,f);

 for(i=0;i<width;i++)
 {
     for(j=0;j<height;j++)
        intens=intesitate_pixel(i,j,sablon);

        S=S+intens;

 }
 printf("\n%d",S);
 fclose(f);
 return S/(width*height);


}

 void corr(char *sablon,RGB *f1)
 {   int n=165;
     FILE *f=fopen(sablon,"r");
     if(!f)
     {
        perror("eroare");
        exit(-1);
     }

 }



int main()
{
    BMP *bmp=BMPinternal("peppers.bmp");
    BMPext("imagine.bmp",bmp);
    cript("peppers.bmp","imagine_criptata.bmp","key.txt");


    decript("imagine_criptata.bmp","imagine_decriptata.bmp","key.txt");


   test_chi("peppers.bmp");
   test_chi("imagine_decriptata.bmp");


    ////PARTEA 2


/*
	printf("%d\n",intesitate_pixel(100,234,"test_grayscale.bmp"));
	printf("\n%d",medie_intesitate("cifra0.bmp"));*/

	return 0;
}









