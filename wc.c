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
int count_byte(char *); //ファイルのバイト数をカウント
int count_character(char *);  //ファイルの文字数を計算をカウント
int max_line_byte(char *);  //ファイルの最大行数をカウント
int count_words(char *);  //ファイルの単語数をカウント
int count_line(char *); //ファイルの行数をカウント

//mian
int main(int argc, char **argv){
	int ch;
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
    //getoptを用いてどのオプションが指定されたかを解析
		while((ch = getopt(argc, argv, "cmLw")) != -1){
			switch(ch){
				case 'c':
          //cオプションのフラグを立てる
					opts.c = 1;
					break;
				case 'm':
          //mオプションのフラグを立てる
					opts.m = 1;
					break;
				case 'L':
          //Lオプションのフラグを立てる
					opts.L = 1;
					break;
				case 'w':
          //wオプションのフラグを立てる
					opts.w = 1;
					break;
				default:
          //間違ったオプションを受け取ったとき
					printf("Try 'wc --help' for more information.\n");
					exit(1);
			}
		}
		target_file = argv[optind]; //ファイル名を保持
	}

  //なんのオプションもないorターゲットのファイル名がない時はエラー
	if((!opts.c && !opts.m && !opts.L && !opts.w && !opts.no_option) || target_file == NULL){
		printf("Try 'wc --help' for more information.\n");
		exit(1);
	}

  //それぞれのオプションの有無によって各関数を実行
	if(opts.no_option || opts.c) if((cb = count_byte(target_file)) == -1)exit(1);
	if(opts.m) if((cc = count_character(target_file)) == 1)exit(1);
	if(opts.no_option) if((cl = count_line(target_file)) == 1)exit(1);
	if(opts.L) if((mlb = max_line_byte(target_file)) == -1)exit(1);
	if(opts.no_option || opts.w) if((cw = count_words(target_file)) == -1)exit(1);

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

//ファイルのバイト数をカウントする関数
int count_byte(char *target_file){
	FILE *fp;
	int cnt = 0;
  //ファイルを開く
	if((fp = fopen(target_file, "r")) != NULL)
		while(fgetc(fp) != EOF) cnt++;  //1bayte毎にカウントアップ
	else {
		printf("wc: %s: No such file or directory\n", target_file);
		return -1;
	}
	fclose(fp);

	return cnt;
}
//ファイルの文字数を計算をカウントする関数
int count_character(char *target_file){
	FILE *fp;
	char c;
	int cnt = 0;
  //ファイルを開く
	if((fp = fopen(target_file, "r")) != NULL) {
		while((c = fgetc(fp)) != EOF) {
			//utf-8のマルチバイト文字の2バイト目以降の文字を検出したらカウントをスキップ
      if((c & 0xc0) == 0x80)continue;
      cnt++;
    }
	} else {
		printf("wc: %s: No such file or directory\n", target_file);
		return -1;
	}
	fclose(fp);

	return cnt;
}
//ファイルの最大バイトの行のバイト数を返す関数
int max_line_byte(char *target_file){
	FILE *fp;
	char str[N], *ptr;
	int cnt = 0;
  //ファイルを開く
	if((fp = fopen(target_file, "r")) != NULL)
		while(fgets(str, N, fp) != NULL){ //行毎に読みこむ
			int row_cnt = 0;
			ptr = str;
			while(*ptr != '\0' && *ptr != '\n' && *ptr != EOF){  //その行のバイト数をカウント
				row_cnt++;
				ptr+=1;
			}
			if(cnt < row_cnt)cnt = row_cnt;  //最大値を更新
		}
	else {
		printf("wc: %s: No such file or directory\n", target_file);
		return -1;
	}
	fclose(fp);

	return cnt;
}
//ファイルの単語数をカウントする関数
int count_words(char *target_file){
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
		return -1;
	}
	fclose(fp);

	return cnt;
}
//ファイルの行数をカウントする関数
int count_line(char *target_file){
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
		return -1;
	}
	fclose(fp);

	return cnt;
}
