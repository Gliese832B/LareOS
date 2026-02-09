#include "printf.h"
#include "uart.h"
#include "string.h"

static void print_uint(char **buf, uint64_t val, int base, int width, char pad, int uppercase) {
    char tmp[65];
    int i = 0;
    const char *digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";

    if (val == 0) {
        tmp[i++] = '0';
    } else {
        while (val > 0) {
            tmp[i++] = digits[val % base];
            val /= base;
        }
    }

    while (i < width) tmp[i++] = pad;

    while (i > 0) {
        if (buf) {
            *(*buf)++ = tmp[--i];
        } else {
            uart_putc(tmp[--i]);
        }
    }
}

static void print_int(char **buf, int64_t val, int width, char pad) {
    if (val < 0) {
        if (buf) *(*buf)++ = '-';
        else uart_putc('-');
        if (width > 0) width--;
        val = -val;
    }
    print_uint(buf, (uint64_t)val, 10, width, pad, 0);
}

static void print_str(char **buf, const char *s, int width) {
    if (!s) s = "(null)";
    int len = strlen(s);
    while (len < width) {
        if (buf) *(*buf)++ = ' ';
        else uart_putc(' ');
        width--;
    }
    while (*s) {
        if (buf) *(*buf)++ = *s++;
        else uart_putc(*s++);
    }
}

static int format_output(char *buf, const char *fmt, va_list args) {
    char *start = buf;

    while (*fmt) {
        if (*fmt != '%') {
            if (buf) *buf++ = *fmt;
            else uart_putc(*fmt);
            fmt++;
            continue;
        }
        fmt++;

        char pad = ' ';
        int width = 0;

        if (*fmt == '0') {
            pad = '0';
            fmt++;
        }

        while (*fmt >= '0' && *fmt <= '9') {
            width = width * 10 + (*fmt - '0');
            fmt++;
        }

        switch (*fmt) {
            case 'd':
            case 'i':
                print_int(buf ? &buf : NULL, va_arg(args, int64_t), width, pad);
                break;
            case 'u':
                print_uint(buf ? &buf : NULL, va_arg(args, uint64_t), 10, width, pad, 0);
                break;
            case 'x':
                print_uint(buf ? &buf : NULL, va_arg(args, uint64_t), 16, width, pad, 0);
                break;
            case 'X':
                print_uint(buf ? &buf : NULL, va_arg(args, uint64_t), 16, width, pad, 1);
                break;
            case 'p':
                if (buf) { *buf++ = '0'; *buf++ = 'x'; }
                else { uart_putc('0'); uart_putc('x'); }
                print_uint(buf ? &buf : NULL, va_arg(args, uint64_t), 16, 16, '0', 0);
                break;
            case 'c': {
                char c = (char)va_arg(args, int);
                if (buf) *buf++ = c;
                else uart_putc(c);
                break;
            }
            case 's':
                print_str(buf ? &buf : NULL, va_arg(args, const char*), width);
                break;
            case '%':
                if (buf) *buf++ = '%';
                else uart_putc('%');
                break;
            case '\0':
                goto done;
            default:
                if (buf) { *buf++ = '%'; *buf++ = *fmt; }
                else { uart_putc('%'); uart_putc(*fmt); }
                break;
        }
        fmt++;
    }

done:
    if (buf) {
        *buf = '\0';
        return (int)(buf - start);
    }
    return 0;
}

void kprintf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    format_output(NULL, fmt, args);
    va_end(args);
}

int ksprintf(char *buf, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = format_output(buf, fmt, args);
    va_end(args);
    return ret;
}
