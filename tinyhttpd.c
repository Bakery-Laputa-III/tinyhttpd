#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/stat.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <strings.h>
#include <signal.h>
#include <sys/time.h>

#define PORT 8080
#define BACKLOG 10
#define BUFFER_SIZE 4096
#define MAX_PATH_LENGTH 1024
#define DEFAULT_ROOT "./htdocs"
#define CONNECTION_TIMEOUT 30
#define MAX_CLIENTS 100

typedef struct {
    int client_socket;
    struct sockaddr_in client_addr;
} client_info_t;

const char* get_mime_type(const char* file_path) {
    const char* ext = strrchr(file_path, '.');
    if (!ext) return "text/plain";
    
    if (strcasecmp(ext, ".html") == 0 || strcasecmp(ext, ".htm") == 0)
        return "text/html";
    if (strcasecmp(ext, ".css") == 0)
        return "text/css";
    if (strcasecmp(ext, ".js") == 0)
        return "application/javascript";
    if (strcasecmp(ext, ".jpg") == 0 || strcasecmp(ext, ".jpeg") == 0)
        return "image/jpeg";
    if (strcasecmp(ext, ".png") == 0)
        return "image/png";
    if (strcasecmp(ext, ".gif") == 0)
        return "image/gif";
    if (strcasecmp(ext, ".txt") == 0)
        return "text/plain";
    if (strcasecmp(ext, ".pdf") == 0)
        return "application/pdf";
    
    return "application/octet-stream";
}

void send_response(int client_socket, const char* status_code, const char* content_type, const char* body, size_t body_length) {
    char response_header[BUFFER_SIZE];
    time_t now = time(NULL);
    char time_str[128];
    strftime(time_str, sizeof(time_str), "%a, %d %b %Y %H:%M:%S GMT", gmtime(&now));
    
    int header_len = snprintf(response_header, sizeof(response_header),
                             "HTTP/1.1 %s\r\n"
                             "Server: tinyhttpd/1.0\r\n"
                             "Date: %s\r\n"
                             "Content-Type: %s\r\n"
                             "Content-Length: %zu\r\n"
                             "Connection: close\r\n"
                             "\r\n",
                             status_code, time_str, content_type, body_length);
    
    if (header_len < 0 || header_len >= (int)sizeof(response_header)) {
        return;
    }
    
    ssize_t sent = send(client_socket, response_header, strlen(response_header), 0);
    if (sent < 0) {
        return;
    }
    
    if (body && body_length > 0) {
        send(client_socket, body, body_length, 0);
    }
}

void send_error_response(int client_socket, int error_code) {
    const char* status_message;
    const char* error_html;
    
    switch (error_code) {
        case 400:
            status_message = "Bad Request";
            error_html = "<html><body><h1>400 Bad Request</h1><p>Your browser sent a request that this server could not understand.</p></body></html>";
            break;
        case 404:
            status_message = "Not Found";
            error_html = "<html><body><h1>404 Not Found</h1><p>The requested URL was not found on this server.</p></body></html>";
            break;
        case 500:
            status_message = "Internal Server Error";
            error_html = "<html><body><h1>500 Internal Server Error</h1><p>The server encountered an internal error.</p></body></html>";
            break;
        case 501:
            status_message = "Not Implemented";
            error_html = "<html><body><h1>501 Not Implemented</h1><p>The requested method is not implemented.</p></body></html>";
            break;
        default:
            status_message = "Internal Server Error";
            error_html = "<html><body><h1>500 Internal Server Error</h1><p>An unknown error occurred.</p></body></html>";
            break;
    }
    
    char status_code[16];
    snprintf(status_code, sizeof(status_code), "%d %s", error_code, status_message);
    send_response(client_socket, status_code, "text/html", error_html, strlen(error_html));
}

void url_decode(char* dst, const char* src, size_t dst_size) {
    char* d = dst;
    const char* s = src;
    char c;
    
    while ((c = *s++) && (d - dst < (int)dst_size - 1)) {
        if (c == '%' && isxdigit(s[0]) && isxdigit(s[1])) {
            char hex_str[3] = {s[0], s[1], '\0'};
            *d++ = (char)strtol(hex_str, NULL, 16);
            s += 2;
        } else if (c == '+') {
            *d++ = ' ';
        } else {
            *d++ = c;
        }
    }
    *d = '\0';
}

void handle_get_request(int client_socket, const char* request_path, const char* root_dir) {
    char file_path[MAX_PATH_LENGTH];
    char decoded_path[MAX_PATH_LENGTH];
    
    url_decode(decoded_path, request_path, sizeof(decoded_path));
    
    if (strstr(decoded_path, "..") != NULL) {
        send_error_response(client_socket, 400);
        return;
    }
    
    if (strcmp(decoded_path, "/") == 0) {
        snprintf(file_path, sizeof(file_path), "%s/index.html", root_dir);
    } else {
        snprintf(file_path, sizeof(file_path), "%s%s", root_dir, decoded_path);
    }
    
    struct stat file_stat;
    if (stat(file_path, &file_stat) != 0) {
        send_error_response(client_socket, 404);
        return;
    }
    
    if (!S_ISREG(file_stat.st_mode)) {
        send_error_response(client_socket, 404);
        return;
    }
    
    FILE* file = fopen(file_path, "rb");
    if (!file) {
        send_error_response(client_socket, 500);
        return;
    }
    
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char* file_content = malloc(file_size);
    if (!file_content) {
        fclose(file);
        send_error_response(client_socket, 500);
        return;
    }
    
    size_t bytes_read = fread(file_content, 1, file_size, file);
    fclose(file);
    
    const char* mime_type = get_mime_type(file_path);
    send_response(client_socket, "200 OK", mime_type, file_content, bytes_read);
    
    free(file_content);
}

void* handle_client(void* arg) {
    client_info_t* client_info = (client_info_t*)arg;
    int client_socket = client_info->client_socket;
    char buffer[BUFFER_SIZE];
    
    struct timeval timeout;
    timeout.tv_sec = CONNECTION_TIMEOUT;
    timeout.tv_usec = 0;
    if (setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        close(client_socket);
        free(client_info);
        return NULL;
    }
    
    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0) {
        close(client_socket);
        free(client_info);
        return NULL;
    }
    
    buffer[bytes_received] = '\0';
    
    char method[16] = {0};
    char path[MAX_PATH_LENGTH] = {0};
    char protocol[16] = {0};
    
    if (sscanf(buffer, "%15s %1023s %15s", method, path, protocol) != 3) {
        send_error_response(client_socket, 400);
        close(client_socket);
        free(client_info);
        return NULL;
    }
    
    if (strcmp(method, "GET") != 0) {
        send_error_response(client_socket, 501);
        close(client_socket);
        free(client_info);
        return NULL;
    }
    
    const char* root_dir = getenv("TINYHTTPD_ROOT");
    if (!root_dir) {
        root_dir = DEFAULT_ROOT;
    }
    
    handle_get_request(client_socket, path, root_dir);
    
    close(client_socket);
    free(client_info);
    return NULL;
}

static volatile sig_atomic_t running = 1;

void signal_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        running = 0;
    }
}

int main(void) {
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    if (sigaction(SIGINT, &sa, NULL) < 0 || sigaction(SIGTERM, &sa, NULL) < 0) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    
    if (listen(server_socket, BACKLOG) < 0) {
        perror("listen");
        close(server_socket);
        exit(EXIT_FAILURE);
    }
    
    printf("tinyhttpd server running on port %d\n", PORT);
    printf("Root directory: %s\n", getenv("TINYHTTPD_ROOT") ? getenv("TINYHTTPD_ROOT") : DEFAULT_ROOT);
    printf("Press Ctrl+C to stop the server\n");
    
    while (running) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("accept");
            continue;
        }
        
        client_info_t* client_info = malloc(sizeof(client_info_t));
        if (!client_info) {
            perror("malloc");
            close(client_socket);
            continue;
        }
        
        client_info->client_socket = client_socket;
        client_info->client_addr = client_addr;
        
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client, client_info) != 0) {
            perror("pthread_create");
            close(client_socket);
            free(client_info);
            continue;
        }
        
        pthread_detach(thread_id);
    }
    
    printf("\nShutting down server...\n");
    close(server_socket);
    return 0;
}
