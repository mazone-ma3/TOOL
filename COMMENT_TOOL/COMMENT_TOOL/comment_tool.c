#include <stdio.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

void process_line(const char *src, char *dest) {
    char temp[MAX_LINE_LENGTH] = "";
    const char *target_rm = "\t.section";
    const char *target_old = ".bss";
    const char *target_new = ".data";

    const char *p = src;

    // 1. ".section" を除去しながら temp にコピー
    while (*p) {
        if (strncmp(p, target_rm, strlen(target_rm)) == 0) {
            p += strlen(target_rm); // 見つけたらスキップ
        } else {
            strncat(temp, p, 1);    // 1文字ずつコピー
            p++;
        }
    }

    // 2. temp 内の ".bss" を ".data" に置換して dest にコピー
    char *pos;
    char *current = temp;
    dest[0] = '\0';

/*    while ((pos = strstr(current, target_old)) != NULL) {
        // ".bss" までの部分をコピー
        strncat(dest, current, pos - current);
        // ".data" をコピー
        strcat(dest, target_new);
        // ポインタを ".bss" の後ろへ進める
        current = pos + strlen(target_old);
    }*/
    // 残りの部分をコピー
    strcat(dest, current);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "使用法: %s <入力ファイル> <出力ファイル>\n", argv[0]);
        return 1;
    }

    FILE *ifp = fopen(argv[1], "r");
    FILE *ofp = fopen(argv[2], "w");
    if (!ifp || !ofp) {
        perror("ファイルオープンエラー");
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    char processed[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), ifp)) {
        process_line(line, processed);
        fputs(processed, ofp);
    }

    fclose(ifp);
    fclose(ofp);
    printf("処理が完了しました。\n");

    return 0;
}