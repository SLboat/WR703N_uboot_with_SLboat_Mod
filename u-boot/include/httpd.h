#ifndef _UIP_HTTPD_H__
#define _UIP_HTTPD_H__

void HttpdStart(void);
void HttpdHandler(void);

/* board specific implementation */
extern int do_http_upgrade(const ulong size, const int upgrade_type);
extern int do_http_progress(const int state);
extern void ar7240_all_led_on(void);
extern void ar7240_all_led_off(void);

#endif
