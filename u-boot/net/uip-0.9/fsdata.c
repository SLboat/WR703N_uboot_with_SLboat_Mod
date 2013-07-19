static const char data_flashing_html[] =
"HTTP/1.0 200 OK\n"
"Server: uIP/0.9 (http://dunkels.com/adam/uip/)\n"
"Content-type: text/html\n"
"\n"
"<html>\n"
"\t<head>\n"
"\t\t<title>\n"
"\t\t\tFailsafe UI\n"
"\t\t</title>\n"
"\t</head>\n"
"\t<body>\n"
"\t\t<center><h1>Upgrading system...</h1></center>\n"
"\t</body>\n"
"</html>\n";

static const char data_fail_html[] =
"HTTP/1.0 200 OK\n"
"Server: uIP/0.9 (http://dunkels.com/adam/uip/)\n"
"Content-type: text/html\n"
"\n"
"<html>\n"
"\t<head>\n"
"\t\t<title>\n"
"\t\t\tFailsafe UI\n"
"\t\t</title>\n"
"\t</head>\n"
"\t<body>\n"
"\t\t<h1>Flashing failed</h1>\n"
"\t\tERROR - the image you uploaded failed to pass verification.<br>\n"
"\t\tPlease make sure to use an official update provided by http://lantiq.com/\n"
"\t</body>\n"
"</html>\n";

static const char data_404_html[] =
"HTTP/1.0 404 File not found\n"
"Server: uIP/0.9 (http://dunkels.com/adam/uip/)\n"
"Content-type: text/html\n"
"\n"
"<html>\n"
"\t<head>\n"
"\t\t<title>\n"
"\t\t\tFailsafe UI\n"
"\t\t</title>\n"
"\t</head>\n"
"\t<body>\n"
"\t\t<center><h1>404 - file not found</h1></center>\n"
"\t</body>\n"
"</html>\n";

static const char data_index_html[] =
"HTTP/1.0 200 OK\n"
"Server: uIP/0.9 (http://dunkels.com/adam/uip/)\n"
"Content-type: text/html\n"
"\n"
"<html>\n"
"\t<head>\n"
"\t\t<title>\n"
"\t\t\tFailsafe UI\n"
"\t\t</title>\n"
"\t</head>\n"
"\t<body>\n"
"\t\t<h1>Failsafe UI</h1>\n"
"\t\t<form method=\"post\" enctype=\"multipart/form-data\">\n"
"\t\t\t<input type=file name=firmware>\n"
"\t\t\t<input type=submit>\n"
"\t\t</form>\n"
"\t</body>\n"
"</html>\n";

static const char data_flash_html[] =
"HTTP/1.0 200 OK\n"
"Server: uIP/0.9 (http://dunkels.com/adam/uip/)\n"
"Content-type: text/html\n"
"\n"
"<html>\n"
"\t<head>\n"
"\t\t<title>\n"
"\t\t\tFailsafe UI\n"
"\t\t</title>\n"
"\t</head>\n"
"\t<body>\n"
"\t\t<h1>Flashing...</h1>\n"
"\t\tThe system is now trying to flash. If there is a problem, the LEDs will "
"start to blink.<br>\n"
"\n"
"\t\tAfter a successful update the box will reboot\n"
"\t</body>\n"
"</html>\n";

const struct fsdata_file file_flashing_html[] =
{{NULL, "/flashing.html", data_flashing_html, sizeof(data_flashing_html)}};

const struct fsdata_file file_fail_html[] =
{{file_flashing_html, "/fail.html", data_fail_html, sizeof(data_fail_html)}};

const struct fsdata_file file_404_html[] =
{{file_fail_html, "/404.html", data_404_html, sizeof(data_404_html)}};

const struct fsdata_file file_index_html[] =
{{file_404_html, "/index.html", data_index_html, sizeof(data_index_html)}};

const struct fsdata_file file_flash_html[] =
{{file_index_html, "/flash.html", data_flash_html, sizeof(data_flash_html)}};

#define FS_ROOT file_flash_html

#define FS_NUMFILES 5
