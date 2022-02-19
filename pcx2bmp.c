/*
 * PCXTOOL - to manipulate some attributes of PCX files according to
 *           the special requirements of PC Engine developers
 */


#define	MAX_X	1024  //512
#define	MAX_Y	1024  //512
#define	MAX_PAL	256


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* PCX header info */

typedef struct
{
  char   format_id;
  char   version;
  char   compress;
  char   bpp;
  short  x_min;
  short  y_min;
  short  x_max;
  short  y_max;
  short  hres;
  short  vres;
  char   palette16[48];
  char   reserved1;
  char   planes;
  short  bytes_line;
  short  palette_type;
  char   reserved2[58];
} pcx_header;

pcx_header pcxhdr;


/* for now, maximum size of image = 1024 x 1024 pixels */

char pixel[MAX_X*MAX_Y];

int GrSizeX = 0;
int GrSizeY = 0;

char palette_reference[MAX_PAL];

unsigned char pal_r[MAX_PAL];
unsigned char pal_g[MAX_PAL];
unsigned char pal_b[MAX_PAL];

/* command-line options */

int showref, dump_palette;
int swap, swapfrom, swapto;
int pcepal;

#include "bmp.c"


//---------------------------------------
// 
//---------------------------------------
/* and now for the program */

void error(char *string, long pos)
{
	printf("\n");
	printf(string);
	printf("\n");
	printf("At position %ld in file", pos);
	printf("\n\n");
	exit(1);
}


//---------------------------------------
// 
//---------------------------------------

void init(void)
{
	int i;
	int j;

	memset(&pcxhdr, 0, 128);

	for (i = 0; i < MAX_PAL; i++)
	{
		palette_reference[i] = 0;

		pal_r[i] = 0;
		pal_g[i] = 0;
		pal_b[i] = 0;
	}

	//※※※0xffで初期化します
	//0xffというのは、圧縮形式で1バイトずつかならず出力されるパターンです。
	//元画像になるべく含まれないように画像を作るべきですが、それとは別に
	//圧縮のとき、左から1バイトずつ同じかどうか比較しています。
	//左端に達したとき、あらかじめゼロクリアしていると、右隣がゼロのとき、ゼロが連続していると判断されます。
	//0xffならそもそも連続しません。だから0xffで初期化しておいたほうがよい
	for (i = 0; i < MAX_X; i++)
	{
		for (j = 0; j < MAX_X; j++)
		{
			//pixel[i*MAX_X+j] = 0; //×
			pixel[i*MAX_X+j] = 0xff;
		}
	}
}


//---------------------------------------
// 
//---------------------------------------

void
usage(void)
{
	printf("pcx2bmp: Convert PCX Graphic image to BMP or BMP to PCX.\n\n");
	printf("Usage:\n\n");
	printf("pcxtool InputFileName OutputFileName\n\n");
	printf("The combination of input / output files is fixed.\n\n");
	printf("(ok)pcxtool input.PCX output.BMP\n");
	printf("(ok)pcxtool input.BMP output.PCX\n");
	printf("(no)pcxtool input.PCX output.PCX\n");
	printf("(no)pcxtool input.BMP output.BMP\n");
	printf("Input / output PCX and BMP are 256-color palette images.\n");
	printf("\n");
}


//---------------------------------------
// 
//---------------------------------------

int get_val(char *val)
{
	int i;
	int base, startidx, cumul_val, temp;

	cumul_val = 0;

	if (val[0] == '$')
	{
		base = 16;
		startidx = 1;
	}
	else if  ((val[0] == '0') && (val[1] == 'x'))
	{
		base = 16;
		startidx = 2;
	}
	else
	{
		base = 10;
		startidx = 0;
	}

	cumul_val = 0;

	for (i = startidx; i < strlen(val); i++)
	{
		if ((val[i] >= '0') && (val[i] <= '9'))
		{
			temp = val[i] - '0';
			cumul_val = cumul_val * base + temp;
		}
		else if ((val[i] >= 'A') && (val[i] <= 'F') && (base == 16))
		{
			temp = val[i] - 'A' + 10;
			cumul_val = cumul_val * base + temp;
		}
		else if ((val[i] >= 'a') && (val[i] <= 'f') && (base == 16))
		{
			temp = val[i] - 'a' + 10;
			cumul_val = cumul_val * base + temp;
		}
		else break;
	}

	return(cumul_val);
}



//---------------------------------------
// 
//---------------------------------------

void swap_palette(void)
{
	int x, y, temp;

	for ( y=0; y < (pcxhdr.y_max-pcxhdr.y_min+1); y++ )
	{
		for( x=0; x < pcxhdr.bytes_line; x++ )
		{
			if (pixel[y*MAX_X+x] == swapfrom)
			{
				pixel[y*MAX_X+x] = swapto;
			}
			else if (pixel[y*MAX_X+x] == swapto)
			{
				pixel[y*MAX_X+x] = swapfrom;
			}
		}
	}

	temp            = pal_r[swapfrom];
	pal_r[swapfrom] = pal_r[swapto];
	pal_r[swapto]   = temp;

	temp            = pal_g[swapfrom];
	pal_g[swapfrom] = pal_g[swapto];
	pal_g[swapto]   = temp;

	temp            = pal_b[swapfrom];
	pal_b[swapfrom] = pal_b[swapto];
	pal_b[swapto]   = temp;

	temp                        = palette_reference[swapfrom];
	palette_reference[swapfrom] = palette_reference[swapto];
	palette_reference[swapto]   = temp;
}



//---------------------------------------
// 
//---------------------------------------
void pcepal_adjust(void)
{
	int i;

	for (i=0; i < MAX_PAL; i++)
	{
		if ( pcepal == 1 )
		{
			pal_r[i] = pal_r[i] & 0xE0;
			pal_g[i] = pal_g[i] & 0xE0;
			pal_b[i] = pal_b[i] & 0xE0;
		}
		else if ( pcepal == 2 )
		{
			pal_r[i] = (pal_r[i] & 0xE0) * 255 / 224;
			pal_g[i] = (pal_g[i] & 0xE0) * 255 / 224;
			pal_b[i] = (pal_b[i] & 0xE0) * 255 / 224;
		}
	}
}





//---------------------------------------
// pcxファイル読み込み
//---------------------------------------

void read_pcx(FILE *in)
{
	int x,y;
	int repeat, temp;

	//128バイトヘッダ部分を読み込む
	temp = fread( &pcxhdr, 1, 128, in);

	if (temp < 128)
	{
		error("read_pcx: read header short", ftell(in));
	}

	// Y の ライン数ぶん処理
	for ( y=0 ; y < ( pcxhdr.y_max - pcxhdr.y_min + 1 ); y++)
	{
		x = 0;

		// Xの処理 = バイト数
		// 圧縮がかかっていますが、横単位で処理されるところをみると
		// ラインをまたいで圧縮されていることは無いようです。
		while ( x < pcxhdr.bytes_line )
		{
			temp = fgetc( in );  //1バイト読み込み

			if (temp == EOF)
			{
				error("read_pcx: get byte", ftell(in));
			}

			//pcxファイルはほとんどすべての場合この「1」による圧縮がかかっています
			//有効ビットが下位6ビットで、上位二ビットはコントロール
			if ((pcxhdr.compress == 1) && ((temp & 0xC0) == 0xC0))
			{
				repeat = temp & 0x3F;   //有効ビット=ループ回数になります

				temp = fgetc(in);
				if (temp == EOF)
				{
					error("read_pcx: get repeated byte", ftell(in));
				}

				palette_reference[temp]++;
				while (repeat > 0)
				{
					pixel[ y * MAX_X + x ] = temp;
					repeat--;
					x++;
				}
			}
			else  //0の場合つまり無圧縮はほとんどありえないといわれています。いちおう処理
			{
				palette_reference[temp]++;
				pixel[y*MAX_X+x] = temp;
				x++;
			}
		}
	}

	//この時点でpixel[]には画像データが無圧縮でつまっています
	//横1ラインごとにたてにプレーンが並んでいます

	//ファイルの最後にパレットデータがあります。
	//パレットデータの直前に0x0cがあります
	temp = fgetc(in);
	if (temp == EOF)
	{
		error("read_pcx: no palette", ftell(in));
	}
	if (temp != 0x0c)
	{
		error("read_pcx: unexpected byte instead of palette ID 0x0c", ftell(in));
	}

	//パレットデータを読み込みます

	for (x = 0; x < MAX_PAL; x++)
	{
		temp = fgetc(in);
		if (temp == EOF)
		{
			error("read_pcx: end in palette loop - r", ftell(in));
		}
		pal_r[x] = temp;

		temp = fgetc(in);
		if (temp == EOF)
		{
			error("read_pcx: end in palette loop - g", ftell(in));
		}
		pal_g[x] = temp;

		temp = fgetc(in);
		if (temp == EOF)
		{
			error("read_pcx: end in palette loop - b", ftell(in));
		}
		pal_b[x] = temp;
	}
}


//---------------------------------------
// pcxファイル書き込み
//---------------------------------------

void write_pcx(FILE *out)
{
	int x,y;
	int repeat;
	unsigned int temp;
	unsigned int tmp2;
	int lp;

	pcxhdr.format_id  = 0x0a;    // 固定値
	pcxhdr.version    = 4;       // Windows用のPCペイントブラシ 
	pcxhdr.compress   = 1;       // 圧縮タイプは1 ランレングスエンコーディング（RLE）
	pcxhdr.bpp        = 8;       // 1つの平面を構成するビット数。ほとんどの場合、1、2、4、または8です
	pcxhdr.x_min      = 0;       // 画像位置の最小x座標
	pcxhdr.y_min      = 0;       // 画像位置の最小y座標
	pcxhdr.x_max      = GrSizeX-1; // 画像位置の最大x座標(座標なので-1します元のbmpの値は幅)
	pcxhdr.y_max      = GrSizeY-1; // 画像位置の最大y座標(座標なので-1します元のbmpの値は高さ)
	pcxhdr.hres       = 0;       // DPIでの水平方向の画像解像度
	pcxhdr.vres       = 0;       // DPIでの垂直方向の画像解像度
	//pcxhdr.palette16[48];      // 16色画像用のEGAパレット
	for( lp=0; lp<48; lp++) pcxhdr.palette16[lp] = 0xff;
	pcxhdr.reserved1  = 0;       // 最初の予約済みフィールド。通常はゼロに設定されます
	pcxhdr.planes     = 1;       // ピクセルデータを構成するカラープレーンの数。ほとんどの場合、1、3、または4に選択されます
	pcxhdr.bytes_line = GrSizeX; // 1つのスキャンラインを表す1つのカラープレーンのバイト数
	pcxhdr.palette_type = 1;     // パレットを解釈するモード 1:モノクロまたはカラーの情報含む 2:グレースケール情報含む
	//pcxhdr.reserved2[58];      // 2番目の予約済みフィールド。将来の拡張を目的としており、通常はゼロバイトに設定されます
	for( lp=0; lp<58; lp++) pcxhdr.reserved2[lp] = 0;

	//圧縮タイプは1 ランレングスエンコーディング（RLE）
	//pcxhdr.compress = 1;

	temp = fwrite(&pcxhdr, 1, 128, out);
	if (temp < 128)
	{
		error("write_pcx: write header short", ftell(out));
	}

	for ( y=0; y < (pcxhdr.y_max-pcxhdr.y_min+1); y++ )
	{
		x = 0;
		repeat = 1;

		while ( x < pcxhdr.bytes_line )
		{
			temp = pixel[ y * MAX_X + x ];
			temp &= 0x0ff;
			tmp2 = pixel[ y * MAX_X + x + 1];
			tmp2 &= 0x0ff;

			if ( x >= (pcxhdr.bytes_line-1) )
			{
				if (fputc((repeat | 0x0c0), out) == EOF)
				{
					error("write_pcx: error writing repeater", ftell(out));
				}

				if (fputc(temp, out) == EOF)
				{
					error("write_pcx: error writing byte", ftell(out));
				}
				repeat = 1;
				x++;
			}
			else if ( temp != tmp2 )
			{
				if (fputc((repeat | 0x0c0), out) == EOF)
				{
					error("write_pcx: error writing repeater", ftell(out));
				}

				if (fputc(temp, out) == EOF)
				{
					error("write_pcx: error writing byte", ftell(out));
				}
				repeat = 1;
				x++;
			}
			else if ( repeat == 0x3F )
			{
				if (fputc((repeat | 0x0c0), out) == EOF)
				{
					error("write_pcx: error writing repeater", ftell(out));
				}

				if (fputc(temp, out) == EOF)
				{
					error("write_pcx: error writing byte", ftell(out));
				}

				repeat = 1;
				x++;
			}
			else
			{
				repeat++;
				x++;
			}
		}
	}

	//パレットの開始である0x0cを出力
	fputc(0x0c, out);

	//パレットを256色ぶん保存
	for ( x = 0; x < MAX_PAL; x++ )
	{
		fputc(pal_r[x], out);
		fputc(pal_g[x], out);
		fputc(pal_b[x], out);
	}
}





char inputFilename[1024];
char outputFilename[1024];
unsigned char src_plette[256*3];
int inFileType   = 0;
int outFileType  = 0;


int main( int argc, char ** argv )
{
	FILE *fp;
	int rtn;
	int argcnt=0;
	int filecnt=0;

	init();
	printf("argc = %d\n", argc);

	for(int i=0; i<argc; i++ ){
		printf("argv[%d] = %s\n",i, argv[i]);
	}

	//----------------------------------
	if( argc <= 1 ){ usage(); exit(0); }


//	strcpy( inputFilename, argv[1] );
//	strcpy( outputFilename, argv[2] );

	argcnt = 1;

	filecnt = 0;

	for( argcnt=1; argcnt<argc ; argcnt++ )
	{
		//strncasecmp() ? 大/小文字を区別しないストリングの比較

		if (strncasecmp(argv[argcnt], "-h", 2) == 0)
		{
			printf("HELP 出力\n");

			usage();
			exit(0);
		}
		else if (strncasecmp(argv[argcnt], "-P256", 5) == 0)
		{
			pcepal = 256;
		}
		else if (strncasecmp(argv[argcnt], "-P16", 4) == 0)
		{
			pcepal = 16;
		}
		else
		{
			if(filecnt==0)  //最初のファイル名
			{
				strcpy( inputFilename , argv[argcnt] );

				//BMPファイルのとき
				if( ( NULL != strstr( inputFilename,".bmp" ))||
					( NULL != strstr( inputFilename,".BMP" )))
				{
					printf("ok... FirstFileName%s\n", inputFilename);
					inFileType=1;  //bmp
				}
				//PCXファイルのとき
				else if( ( NULL != strstr( inputFilename,".pcx" ))||
					( NULL != strstr( inputFilename,".PCX" )))
				{
					printf("ok... FirstFileName%s\n", inputFilename);
					inFileType=2;  //pcx
				}
				else{
					printf("err...input File Name err.\n");
					usage();
					exit(0);
				}
			}
			else if(filecnt==1)  //２個めのファイル名
			{
				strcpy( outputFilename, argv[argcnt] );

				//BMPファイルのとき
				if( ( NULL != strstr( outputFilename,".bmp" ))||
					( NULL != strstr( outputFilename,".BMP" )))
				{
					printf("ok... SecondtFileName%s\n", outputFilename);
					outFileType=1;  //bmp
				}
				//PCXファイルのとき
				else if( ( NULL != strstr( outputFilename,".pcx" ))||
					     ( NULL != strstr( outputFilename,".PCX" )))
				{
					printf("ok... SecondtFileName%s\n", outputFilename);
					outFileType=2;  //pcx
				}
				else{
					printf("err...output File Name err.\n");
					usage();
					exit(0);
				}

			}
			else{
				printf("err...UnKnown Parameter.\n");
				usage();
				exit(0);
			}

			filecnt++;  //0->1 1->2
		}
	}

	//-----------------------------------

	//正当性チェック
	if( inFileType == 0 ){ //入力ファイル名無し
		printf("err... input File Name none.\n");
		usage();
		exit(0);
	}

	if( outFileType == 0 ){ //出力ファイル名無し
		printf("err... output File Name none.\n");
		usage();
		exit(0);
	}

	int inout = 0;  //set err.
	//in outの関係が bmp->pcx pcx->bmpか
	if( inFileType == 1){
		if( outFileType == 2){
			inout=1;  //ok
		}
	}
	if( outFileType == 1){
		if( inFileType == 2){
			inout=1;  //ok
		}
	}
	if( inout == 0 ){  //err
		printf("err... in,out missmatch.\n");
		usage();
		exit(0);
	}

	//-----------------------------------
	if( inFileType == 2 )  //pcx
	{
		// PCX-->BMP変換

		fp = fopen( inputFilename, "rb" );
		if( fp == NULL )
		{
			printf("PCXfile not found. \n");
			exit(1);
		}
		read_pcx(fp);

		fclose(fp);

		GrSizeX = pcxhdr.bytes_line;
		GrSizeY = ( pcxhdr.y_max - pcxhdr.y_min + 1 );

		//画像をコピーします
		for ( int y=0 ; y < GrSizeY ; y++)
		{
			for ( int x=0 ; x < GrSizeX ; x++ )
			{
				Set_BMPData( pixel[y*MAX_X+x] , pcxhdr.bytes_line , x, y );
			}
		}

		//パレットをコピーします
		//BMPの色の並び、BGR
		for(int i=0; i<256; i++ )
		{
			src_plette[i*3+2]=pal_r[i];
			src_plette[i*3+1]=pal_g[i];
			src_plette[i*3+0]=pal_b[i];
		}

		Set_BMPHedder(GrSizeX, GrSizeY, src_plette );

		Write_BMP( outputFilename );

	}

	//--------------------------------------------------
	else if( inFileType == 1 )  //bmp
	{
		fp = fopen( inputFilename, "rb" );
		if( fp == NULL )
		{
			printf("BMPfile not found. \n");
			exit(1);
		}

		rtn = Read_BMP( fp );
		if( rtn == -1 )
		{
			printf("BMPfile not found. \n");
			exit(1);
		}
		fclose(fp);

		GrSizeX = BmpWidth;
		GrSizeY = BmpHeight;

		//画像をコピーします
		for ( int y=0 ; y < GrSizeY ; y++)
		{
			for ( int x=0 ; x < GrSizeX ; x++ )
			{
				pixel[y*MAX_X+x] = BmpBuff[y*BmpWidth+x];
			}
		}

		//パレットをコピーします
		//BMPの色の並び、BGR
		for(int i=0; i<256; i++ )
		{
			pal_r[i] = BitmapHedder.palet[i][2];
			pal_g[i] = BitmapHedder.palet[i][1];
			pal_b[i] = BitmapHedder.palet[i][0];
		}


		fp = fopen( outputFilename, "wb" );
		if( fp == NULL )
		{
			printf("Output File Error. \n");
			exit(1);
		}

		write_pcx( fp );

		fclose(fp);

	}
	else{
		usage();
		exit(0);
	}
}













/*
int main( int argc, char ** argv )
{
	char *filename1, *filename2;
	char *ref_string;
	FILE *infile, *outfile;
	int argcnt;
	int i, palcount;
	int cmd_err;

	// initialize global command-line variables

	showref = 0;
	dump_palette = 0;

	swap = 0;
	swapfrom = 0;
	swapto = 0;

	cmd_err = 0;

	init();


	printf("argc = %d\n", argc);
	printf("argv[1] = %s\n", argv[1]);

	argcnt = 1;

	while ((argcnt < argc) && (argv[argcnt][0] == '-'))
	{
		//strncasecmp() ? 大/小文字を区別しないストリングの比較

		if (strncasecmp(argv[argcnt], "-help", 5) == 0)
		{
			usage();
			exit(0);
		}
		else if (strncasecmp(argv[argcnt], "-pcepal2", 8) == 0)
		{
			pcepal = 2;
			argcnt++;
		}
		else if (strncasecmp(argv[argcnt], "-pcepal", 7) == 0)
		{
			pcepal = 1;
			argcnt++;
		}
		else if (strncasecmp(argv[argcnt], "-dump", 5) == 0)
		{
			dump_palette = 1;
			argcnt++;
		}
		else if (strncasecmp(argv[argcnt], "-ref", 4) == 0)
		{
			showref = 1;
			argcnt++;
		}
		else if (strncasecmp(argv[argcnt], "-swap", 5) == 0)
		{
			if ((argcnt + 3) >= argc)
			{
				cmd_err = 1;
				break;
			}
			swap = 1;
			argcnt++;
			swapfrom = get_val(argv[argcnt++]);
			swapto   = get_val(argv[argcnt++]);
			printf("swapfrom = %d\nswapto = %d\n", swapfrom, swapto);
		}
		else
		{
			cmd_err = 1;
			break;
		}
	}

	if ((argcnt >= argc) || (cmd_err != 0))
	{
		printf("\nWrong command-line parameters\n");
		usage();
		exit(1);
	}

	filename1 = argv[argcnt++];
	if (argcnt >= argc)
		filename2 = NULL;
	else
		filename2 = argv[argcnt];

	infile  = fopen(filename1, "rb");
	if (infile == NULL)
		error("main: couldn't open infile", 0);

	read_pcx(infile);
	fclose(infile);


	if (swap)
	{
		swap_palette();
	}

	if (pcepal)
	{
		pcepal_adjust();
	}

	palcount = 0;
	for (i = 0; i < MAX_PAL; i++)
	{
		if (palette_reference[i] > 0)
		palcount++;
	}

	if (dump_palette)
	{
		printf("\nPalette Entries:\n\n");

		for (i = 0; i < MAX_PAL; i++)
		{
			if (palette_reference[i] > 0)
				ref_string = "(referenced)";
			else
				ref_string = "";

			printf("Palette $%02X: RGB = %02X%02X%02X   %s\n",
				i, pal_r[i], pal_g[i], pal_b[i], ref_string);
		}
	}
	else if (showref)
	{
		printf("\nList of Referenced Palette Entries:\n\n");

		for (i = 0; i < MAX_PAL; i++)
		{
			if (palette_reference[i] > 0)
				printf("Palette $%02X: RGB = %02X%02X%02X\n", i, pal_r[i], pal_g[i], pal_b[i]);
		}
	}

	printf("\nNumber of colours actually referenced = %d\n\n", palcount);

	if (filename2 != NULL)
	{
		outfile = fopen(filename2, "wb");
		if (outfile == NULL)
			error("main: couldn't open outfile", 0);

		write_pcx(outfile);
		fclose(outfile);
	}
	exit(0);
}
*/
