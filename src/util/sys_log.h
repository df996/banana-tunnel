/*------------------------------------------------------
¦ 文件名
¦ 文件描述
¦ 
¦ Author: 大风
¦ Email: 1236192@qq.com
¦ Date: 2023-05-31 09:51:29
¦ Version: 1.0
¦ FilePath: src/util/sys_log.h
¦------------------------------------------------------*/

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum SYS_LOG_LEVEL {
  LOG_LEVEL_BEGIN = -1,
  TRACE,
  DEBUG,
  INFO,
  WARN,
  ERROR,
  FATAL,
  LOG_LEVEL_END
} SYS_LOG_LEVEL;

/**
 * 系统日志
 * @param level 日志等级
 * @param content 日志内容
 */
void sys_log(SYS_LOG_LEVEL level, const char *content, ...);
