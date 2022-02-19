
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
//ヘッダ丸ごと読み込まず、項目一つずつ読みます(アラインメントのためずれてしまう)
struct BMAPHEADER_2 {
	unsigned char   bfType0;        //1 データ形式(B)
	unsigned char   bfType1;        //1 データ形式(M)
	unsigned long   bfSize;         //4 ファイルサイズ
	unsigned short  bfReserved1;    //2 予約
	unsigned short  bfReserved2;    //2 予約
	unsigned long   bfOffBits;      //4 ビットマップデータの開始オフセット
	unsigned long   biSize;         //4 ヘッダーのサイズ（以下のデータ）
	long            biWidth;        //4 水平ドット数
	long            biHeight;       //4 垂直ドット数
	unsigned short  biPlanes;       //2 プレーン数
	unsigned short  biBitCount;     //2 １ピクセル当たりのビット数(8)
	unsigned long   biCompression;  //4 圧縮形式（０＝無し）
	unsigned long   biSizeImage;    //4 ビットマップデータサイズ
	long            biXPixPerMeter; //4 水平解像度
	long            biYPixPerMeter; //4 垂直解像度
	unsigned long   biClrUsed;      //4 使用色数
	unsigned long   biClrImporant;  //4 重要な色？
	unsigned char   palet[256][4];  //  1024 256色パレット
}BitmapHedder;


//===============================================================================
// 変数
//===============================================================================
int ReadSize;
int BmpWidth;
int BmpHeight;
//unsigned char *BmpBuff;
unsigned char BmpBuff[0x40000];

//===============================================================================
// 関数
//===============================================================================


//**************************************
// BMPファイル読み込み
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

	//色ビットが8(256色)でなく、4(16色)でもないとき
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

	//BmpBuff   =  new char[ReadSize];   //※newしています

	fread( BmpBuff,1,ReadSize,fp);


	//bmpが上下逆になってるため、さらに上下逆にします。
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
// BMPファイル書き込み
//**************************************
//すでにヘッダ部、画像データ部にデータがセットしてあること

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

	//色ビットが8(256色)でなく、4(16色)でもないとき
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
	//バッファは上下逆ではないので、上下逆にします
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
// BMPヘッダ部設定
//************************************************************
void Set_BMPHedder(int _wide, int _height, unsigned char *plette )
{
	BitmapHedder.bfType0       = 'B';
	BitmapHedder.bfType1       = 'M';
	//BitmapHedder.bfSize      = _wide*_height+(sizeof BMAPHEADER); //ファイルサイズ
	BitmapHedder.bfSize        = _wide*_height+(14+40+1024); //ファイルサイズ
	BitmapHedder.bfReserved1   = 0;        //予約
	BitmapHedder.bfReserved2   = 0;        //予約
	BitmapHedder.bfOffBits     = (14+40+1024);  //ビットマップデータの開始オフセット
	BitmapHedder.biSize        = 0x28;     // 40;//ヘッダーのサイズ（以下のデータ）
	BitmapHedder.biWidth       = _wide;     // 水平ドット数
	BitmapHedder.biHeight      = _height;     // 垂直ドット数
	BitmapHedder.biPlanes      = 1;        // プレーン数
	BitmapHedder.biBitCount    = 8;        // １ピクセル当たりのビット数(8)
	BitmapHedder.biCompression = 0;        // 圧縮形式（０＝無し）
	BitmapHedder.biSizeImage   = 0;        // ビットマップデータサイズ
	BitmapHedder.biXPixPerMeter= 0;        // 水平解像度
	BitmapHedder.biYPixPerMeter= 0;        // 垂直解像度
	BitmapHedder.biClrUsed     = 256;      // 使用色数
	BitmapHedder.biClrImporant = 0;        // 重要な色？

	//bitmapstr.palet[256][4]=;	//256色パレット
	//01234567を３２倍して0,32,64, 96,128,160,192,224
	//0以外のとき、+31で  0,63,95,127,159,191,223,255
	// 色の順番はB、G、R
	for(int lp=0 ; lp<256 ; lp++ )
	{
		BitmapHedder.palet[lp][0]=plette[lp*3];
		BitmapHedder.palet[lp][1]=plette[lp*3+1];
		BitmapHedder.palet[lp][2]=plette[lp*3+2];
		BitmapHedder.palet[lp][3]=0;             //reserved;
	}

}


//************************************************************
// BMPデータ部設定
//************************************************************
void Set_BMPData(int data, int grwidth, int _xx, int _yy )
{
	BmpBuff[_yy*grwidth +_xx] = data;
}

#define Set_BMPData(_data,grwidth,_xx,_yy)    BmpBuff[_yy*grwidth+_xx]=_data;


