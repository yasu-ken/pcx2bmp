
//===============================================================================
// 
//===============================================================================
#include <conio.h>		// for getch();
#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

//===============================================================================
// BMP Hedder
//===============================================================================
//�w�b�_�ۂ��Ɠǂݍ��܂��A���ڈ���ǂ݂܂�(�A���C�������g�̂��߂���Ă��܂�)
struct BMAPHEADER_2 {
	unsigned char   bfType0;        //1 �f�[�^�`��(B)
	unsigned char   bfType1;        //1 �f�[�^�`��(M)
	unsigned long   bfSize;         //4 �t�@�C���T�C�Y
	unsigned short  bfReserved1;    //2 �\��
	unsigned short  bfReserved2;    //2 �\��
	unsigned long   bfOffBits;      //4 �r�b�g�}�b�v�f�[�^�̊J�n�I�t�Z�b�g
	unsigned long   biSize;         //4 �w�b�_�[�̃T�C�Y�i�ȉ��̃f�[�^�j
	long            biWidth;        //4 �����h�b�g��
	long            biHeight;       //4 �����h�b�g��
	unsigned short  biPlanes;       //2 �v���[����
	unsigned short  biBitCount;     //2 �P�s�N�Z��������̃r�b�g��(8)
	unsigned long   biCompression;  //4 ���k�`���i�O�������j
	unsigned long   biSizeImage;    //4 �r�b�g�}�b�v�f�[�^�T�C�Y
	long            biXPixPerMeter; //4 �����𑜓x
	long            biYPixPerMeter; //4 �����𑜓x
	unsigned long   biClrUsed;      //4 �g�p�F��
	unsigned long   biClrImporant;  //4 �d�v�ȐF�H
	unsigned char   palet[256][4];  //  1024 256�F�p���b�g
}BitmapHedder;


//===============================================================================
// �ϐ�
//===============================================================================
int ReadSize;
int BmpWidth;
int BmpHeight;
//unsigned char *BmpBuff;
unsigned char BmpBuff[0x40000];

//===============================================================================
// �֐�
//===============================================================================


//**************************************
// BMP�t�@�C���ǂݍ���
//**************************************

int Read_BMP( FILE *fp )
{
	int i,j;
	unsigned char wok;

	fread(&BitmapHedder.bfType0        , 1,1,fp);
	fread(&BitmapHedder.bfType1        , 1,1,fp);
	fread(&BitmapHedder.bfSize         , 1,4,fp);
	fread(&BitmapHedder.bfReserved1    , 1,2,fp);
	fread(&BitmapHedder.bfReserved2    , 1,2,fp);
	fread(&BitmapHedder.bfOffBits      , 1,4,fp);
	fread(&BitmapHedder.biSize         , 1,4,fp);
	fread(&BitmapHedder.biWidth        , 1,4,fp);
	fread(&BitmapHedder.biHeight       , 1,4,fp);
	fread(&BitmapHedder.biPlanes       , 1,2,fp);
	fread(&BitmapHedder.biBitCount     , 1,2,fp);
	fread(&BitmapHedder.biCompression  , 1,4,fp);
	fread(&BitmapHedder.biSizeImage    , 1,4,fp);
	fread(&BitmapHedder.biXPixPerMeter , 1,4,fp);
	fread(&BitmapHedder.biYPixPerMeter , 1,4,fp);
	fread(&BitmapHedder.biClrUsed      , 1,4,fp);
	fread(&BitmapHedder.biClrImporant  , 1,4,fp);
	//fread(&BitmapHedder.palet          , 1,1024,fp);

	//�F�r�b�g��8(256�F)�łȂ��A4(16�F)�ł��Ȃ��Ƃ�
	if( BitmapHedder.biBitCount != 8 )
	{
		if( BitmapHedder.biBitCount != 4 )
		{
			printf ( "Color type must be 8-bit. ");
			return -1;
		}
	}

	//color bit 16 or 256
	//reading first 16 color.
	if( BitmapHedder.biBitCount == 8 )
		fread(&BitmapHedder.palet  , 1,1024,fp);  //1024=256x4
	else
		fread(&BitmapHedder.palet  , 1,64,fp);    //16x4

	BmpWidth  =  BitmapHedder.biWidth;
	BmpHeight =  BitmapHedder.biHeight;

	ReadSize  =  BmpWidth* BmpHeight;

	//BmpBuff   =  new char[ReadSize];   //��new���Ă��܂�

	fread( BmpBuff,1,ReadSize,fp);


	//bmp���㉺�t�ɂȂ��Ă邽�߁A����ɏ㉺�t�ɂ��܂��B
	//bmp is upside down, so turn it upside down.
	for( i= 0; i<(ReadSize/2); i+=BmpWidth ) //line
	{
		for( j=0; j<BmpWidth; j++ )  //yoko
		{
			wok = BmpBuff[i+j];
			BmpBuff[i+j] = BmpBuff[(ReadSize-BmpWidth-i)+j];
			BmpBuff[(ReadSize-BmpWidth-i)+j] = wok;
		}
	}

	return 0;
}


//**************************************
// BMP�t�@�C����������
//**************************************
//���łɃw�b�_���A�摜�f�[�^���Ƀf�[�^���Z�b�g���Ă��邱��

void Write_BMP( char * filename)
{
	FILE *fp;
	//char fileNbuf[100];

	if (( fp = fopen( filename, "wb" )) == NULL )
	{
		printf ( "BMP File Write ERROR : File don't open.\n");
		return;
	}

	fwrite(&BitmapHedder.bfType0        , 1,1,fp);
	fwrite(&BitmapHedder.bfType1        , 1,1,fp);
	fwrite(&BitmapHedder.bfSize         , 1,4,fp);
	fwrite(&BitmapHedder.bfReserved1    , 1,2,fp);
	fwrite(&BitmapHedder.bfReserved2    , 1,2,fp);
	fwrite(&BitmapHedder.bfOffBits      , 1,4,fp);
	fwrite(&BitmapHedder.biSize         , 1,4,fp);
	fwrite(&BitmapHedder.biWidth        , 1,4,fp);
	fwrite(&BitmapHedder.biHeight       , 1,4,fp);
	fwrite(&BitmapHedder.biPlanes       , 1,2,fp);
	fwrite(&BitmapHedder.biBitCount     , 1,2,fp);
	fwrite(&BitmapHedder.biCompression  , 1,4,fp);
	fwrite(&BitmapHedder.biSizeImage    , 1,4,fp);
	fwrite(&BitmapHedder.biXPixPerMeter , 1,4,fp);
	fwrite(&BitmapHedder.biYPixPerMeter , 1,4,fp);
	fwrite(&BitmapHedder.biClrUsed      , 1,4,fp);
	fwrite(&BitmapHedder.biClrImporant  , 1,4,fp);
	//fwrite(&BitmapHedder.palet          , 1,1024,fp);

	//�F�r�b�g��8(256�F)�łȂ��A4(16�F)�ł��Ȃ��Ƃ�
	if( BitmapHedder.biBitCount != 8 )
	{
		if( BitmapHedder.biBitCount != 4 )
		{
			printf ( "BMP File Write ERROR : ColorType Error.\n");
			return;
		}
	}

	//color bit 16 or 256
	if( BitmapHedder.biBitCount == 8 )
		fwrite(&BitmapHedder.palet  , 1,1024,fp);  //1024=256x4
	else
		fwrite(&BitmapHedder.palet  , 1,64,fp);    //16x4

	BmpWidth  =  BitmapHedder.biWidth;
	BmpHeight =  BitmapHedder.biHeight;
	int WriteSize = BmpWidth*BmpHeight;

	int i,j;
	unsigned int wok;
	//�o�b�t�@�͏㉺�t�ł͂Ȃ��̂ŁA�㉺�t�ɂ��܂�
	for( i= 0; i<(WriteSize/2); i+=BmpWidth ) //line
	{
		for( j=0; j<BmpWidth; j++ )  //yoko
		{
			wok = BmpBuff[i+j];
			BmpBuff[i+j] = BmpBuff[(WriteSize-BmpWidth-i)+j];
			BmpBuff[(WriteSize-BmpWidth-i)+j] = wok;
		}
	}

	fwrite(&BmpBuff,1,BmpWidth*BmpHeight,fp);
	fclose(fp);
}


//************************************************************
// BMP�w�b�_���ݒ�
//************************************************************
void Set_BMPHedder(int _wide, int _height, unsigned char *plette )
{
	BitmapHedder.bfType0       = 'B';
	BitmapHedder.bfType1       = 'M';
	//BitmapHedder.bfSize      = _wide*_height+(sizeof BMAPHEADER); //�t�@�C���T�C�Y
	BitmapHedder.bfSize        = _wide*_height+(14+40+1024); //�t�@�C���T�C�Y
	BitmapHedder.bfReserved1   = 0;        //�\��
	BitmapHedder.bfReserved2   = 0;        //�\��
	BitmapHedder.bfOffBits     = (14+40+1024);  //�r�b�g�}�b�v�f�[�^�̊J�n�I�t�Z�b�g
	BitmapHedder.biSize        = 0x28;     // 40;//�w�b�_�[�̃T�C�Y�i�ȉ��̃f�[�^�j
	BitmapHedder.biWidth       = _wide;     // �����h�b�g��
	BitmapHedder.biHeight      = _height;     // �����h�b�g��
	BitmapHedder.biPlanes      = 1;        // �v���[����
	BitmapHedder.biBitCount    = 8;        // �P�s�N�Z��������̃r�b�g��(8)
	BitmapHedder.biCompression = 0;        // ���k�`���i�O�������j
	BitmapHedder.biSizeImage   = 0;        // �r�b�g�}�b�v�f�[�^�T�C�Y
	BitmapHedder.biXPixPerMeter= 0;        // �����𑜓x
	BitmapHedder.biYPixPerMeter= 0;        // �����𑜓x
	BitmapHedder.biClrUsed     = 256;      // �g�p�F��
	BitmapHedder.biClrImporant = 0;        // �d�v�ȐF�H

	//bitmapstr.palet[256][4]=;	//256�F�p���b�g
	//01234567���R�Q�{����0,32,64, 96,128,160,192,224
	//0�ȊO�̂Ƃ��A+31��  0,63,95,127,159,191,223,255
	// �F�̏��Ԃ�B�AG�AR
	for(int lp=0 ; lp<256 ; lp++ )
	{
		BitmapHedder.palet[lp][0]=plette[lp*3];
		BitmapHedder.palet[lp][1]=plette[lp*3+1];
		BitmapHedder.palet[lp][2]=plette[lp*3+2];
		BitmapHedder.palet[lp][3]=0;             //reserved;
	}

}


//************************************************************
// BMP�f�[�^���ݒ�
//************************************************************
void Set_BMPData(int data, int grwidth, int _xx, int _yy )
{
	BmpBuff[_yy*grwidth +_xx] = data;
}

#define Set_BMPData(_data,grwidth,_xx,_yy)    BmpBuff[_yy*grwidth+_xx]=_data;


