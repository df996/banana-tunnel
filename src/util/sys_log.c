/*------------------------------------------------------
¦ 文件名
¦ 文件描述
¦ 
¦ Author: 大风
¦ Email: 1236192@qq.com
¦ Date: 2023-05-31 09:51:11
¦ Version: 1.0
¦ FilePath: src/util/sys_log.c
¦------------------------------------------------------*/
#include "sys_log.h"

static const char *level_names[6] = {
    "TRACE",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
    "FATAL"
};

static const char *level_colors[6] = {
    "\x1b[94m",
    "\x1b[36m",
    "\x1b[32m",
    "\x1b[33m",
    "\x1b[31m",
    "\x1b[35m"
};

/**
 * 系统日志
 * @param level 日志等级
 * @param content 日志内容
 */
void sys_log(SYS_LOG_LEVEL level, const char *format, ...) {
    if (level <= LOG_LEVEL_BEGIN ||
        level >= LOG_LEVEL_END) {
        return;
    }

    // 时间
    char now[100];
    time_t tt = time(NULL);
    strftime(now, sizeof(now), "%Y-%m-%d %H:%M:%S", localtime(&tt));

    fprintf(
        stdout,
        "%s %s[%-5s]\x1b[0m ",
        now,
        level_colors[level],
        level_names[level]
    );
    
    // 追加换行符
    const char *lf = "\n";
    char *new_format = (char *) malloc(strlen(format) + strlen(lf));
    strcpy(new_format, format);
    strcat(new_format, lf);

    va_list args;
    va_start(args, format);
    vfprintf(stdout, new_format, args);

    va_end(args);
    free(new_format);
}
