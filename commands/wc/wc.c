//ヘッダー
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdbool.h>

//マクロ
#define N 1024
#define TOK_DELIM " \t\r\n\a"

//オプションのフラグをまとめた構造体
typedef struct {
	unsigned int c:1;	//オプションに-cが選択されているか
	unsigned int m:1;	//オプションに-mが選択されているか
	unsigned int L:1;	//オプションに-Lが選択されているか
	unsigned int w:1;	//オプションに-wが選択されているか
	unsigned int no_option:1;	//オプションが何も選択されていない
} options;

//プロトタイプ宣言
void parse_options(options *, int, char **);	//オプションの解析

int count_byte_f(char *, int *); //ファイルのバイト数をカウント
int count_character_f(char *, int *);  //ファイルの文字数を計算をカウント
int count_character_str(char *);  //文字列の文字数を計算をカウント
int max_line_byte_f(char *, int *);  //ファイルの最大行数をカウント
int count_words_f(char *, int *);  //ファイルの単語数をカウント
int count_line_f(char *, int *); //ファイルの行数をカウント

//mian
int main(int argc, char **argv){
	int cb, cc, mlb, cw, cl;
	char *target_file;
	options opts = {};

	//オプションの解析
	if(argc == 2 && argv[1][0] != '-'){
    //オプション指定がなく、ファイルのみの指定のとき
		opts.no_option = 1; //no_optionのフラグを立てる
		target_file = argv[1];  //ファイル名を保持
	} else {
     //オプションの指定があるとき
		parse_options(&opts, argc,argv);	//オプションの解析をparse_optionsで行う
		target_file = argv[optind]; //ファイル名を保持
	}

  //なんのオプションもないorターゲットのファイル名がない時はエラー
	if((!opts.c && !opts.m && !opts.L && !opts.w && !opts.no_option) || target_file == NULL){
		printf("Try 'wc --help' for more information.\n");
		exit(1);
	}

	int err = 0;	//エラー変数　コレが-1になるとエラー
  //それぞれのオプションの有無によって各関数を実行
	if(opts.no_option || opts.c) cb = count_byte_f(target_file, &err);
	if(opts.m) cc = count_character_f(target_file, &err);
	if(opts.no_option) cl = count_line_f(target_file, &err);
	if(opts.L) mlb = max_line_byte_f(target_file, &err);
	if(opts.no_option || opts.w) cw = count_words_f(target_file, &err);

	if(err == -1) goto ERROR;
	else goto SUCCSESS;

	ERROR:
	exit(1);
	SUCCSESS:

  //no_optionの時の出力
	if(opts.no_option) printf(" %d %d %d %s\n", cl, cw, cb, target_file);
	else {
    //オプション指定があるときの出力
		if(opts.w) printf(" %d", cw);
		if(opts.c) printf(" %d", cb);
		if(opts.m) printf(" %d", cc);
		if(opts.L) printf(" %d", mlb);
		printf(" %s\n", target_file);
	}

	return 0;
}

//オプションの解析
void parse_options(options *opts, int argc, char **argv){
	int ch;
	//getoptを用いてどのオプションが指定されたかを解析
	while((ch = getopt(argc, argv, "cmLw")) != -1){
		switch(ch){
			case 'c':
				//cオプションのフラグを立てる
				opts->c = 1;
				break;
			case 'm':
				//mオプションのフラグを立てる
				opts->m = 1;
				break;
			case 'L':
				//Lオプションのフラグを立てる
				opts->L = 1;
				break;
			case 'w':
				//wオプションのフラグを立てる
				opts->w = 1;
				break;
			default:
				//間違ったオプションを受け取ったとき
				printf("Try 'wc --help' for more information.\n");
				exit(1);
		}
	}
	return;
}

//ファイルのバイト数をカウントする関数
int count_byte_f(char *target_file, int *err){
	FILE *fp;
	int cnt = 0;
  //ファイルを開く
	if((fp = fopen(target_file, "r")) != NULL)
		while(fgetc(fp) != EOF) cnt++;  //1bayte毎にカウントアップ
	else {
		printf("wc: %s: No such file or directory\n", target_file);
		*err = -1;	//エラー変数に-1を格納
		return 0;
	}
	fclose(fp);

	return cnt;
}
//ファイルの文字数を計算をカウントする関数
int count_character_f(char *target_file, int *err){
	FILE *fp;
	char c;
	int cnt = 0;
  //ファイルを開く
	if((fp = fopen(target_file, "r")) != NULL) {
		while((c = fgetc(fp)) != EOF) {
			//utf-8のマルチバイト文字の2バイト目以降の文字を検出したらカウントをスキップ
      if((c & 0xc0) == 0x80)continue;
      cnt++;
      //if(true)cnt++;
    }
	} else {
		printf("wc: %s: No such file or directory\n", target_file);
		*err = -1;	//エラー変数に-1を格納
		return 0;
	}
	fclose(fp);

	return cnt;
}
//ファイルの最大バイトの行のバイト数を返す関数
int max_line_byte_f(char *target_file, int *err){
	FILE *fp;
	char str[N];
	int cnt = 0, row_cnt;
  //ファイルを開く
	if((fp = fopen(target_file, "r")) != NULL)
		while(fgets(str, N, fp) != NULL){ //行毎に読みこむ
			row_cnt = count_character_str(str);
			if(cnt < row_cnt)cnt = row_cnt;  //最大値を更新
		}
	else {
		printf("wc: %s: No such file or directory\n", target_file);
		*err = -1;	//エラー変数に-1を格納
		return 0;
	}
	fclose(fp);

	return cnt;
}
//文字列の文字数を計算をカウント
int count_character_str(char *str){
	int row_cnt = 0;
	char *ptr = str;
	while(*ptr != '\0' && *ptr != '\n' && *ptr != EOF){  //その行のバイト数をカウント
		row_cnt++;
		ptr+=1;
	}
	return row_cnt;
}
//ファイルの単語数をカウントする関数
int count_words_f(char *target_file, int *err){
	FILE *fp;
	int cnt = 0;
	char str[N];
  //ファイルを開く
	if((fp = fopen(target_file, "r")) != NULL) {
		while(fgets(str, N, fp) != NULL){ //行ごとに読み込む
			char *word = strtok(str, TOK_DELIM); //区切り文字毎にカウントアップする
			while(word != NULL) {
				cnt++;
				word = strtok(NULL, TOK_DELIM);
			}
		}
	} else {
		printf("wc: %s: No such file or directory\n", target_file);
		*err = -1;	//エラー変数に-1を格納
		return 0;
	}
	fclose(fp);

	return cnt;
}
//ファイルの行数をカウントする関数
int count_line_f(char *target_file, int *err){
	FILE *fp;
	char c;
	int cnt = 0;
  //ファイルを開く
	if((fp = fopen(target_file, "r")) != NULL) {
		while(1){
			c = fgetc(fp); //1byte毎に読み込む
			if(c == '\n')cnt++;  //改行コードがあればカウントアップ
			if(c == EOF)break; //EOFを検出したら終了
		}
	} else {
		printf("wc: %s: No such file or directory\n", target_file);
		*err = -1;	//エラー変数に-1を格納
		return 0;
	}
	fclose(fp);

	return cnt;
}
