#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include "headers.h"
#include <microhttpd.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/json.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <string>
#include <algorithm>
#include <sstream>
#include <chrono>

#define PORT 60000

char *readTextFile(const char *path)
{
    long size;
    FILE *f;
    char *page;

    // Read page
    f = fopen(path, "r");

    // Get page size
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    rewind(f);

    // Allocate memory to char pointer
    page = (char *)malloc(sizeof(char) * size);

    // Reading it
    fread(page, 1, size, f);

    return page;
}

MHD_Response *json(std::string url, struct MHD_Connection *connection)
{

    MHD_Result ret;
    struct MHD_Response *response;

    // url = url[5:]
    url = url.substr(5);

    // Replace / by blank space
    replace(url.begin(), url.end(), '/', ' ');

    // Stream object to deal with std::cin
    std::istringstream float_urls(url);

    // Vars
    float lat_start, long_start, lat_end, long_end;
    bool a_star;

    // Read vars
    float_urls >> lat_start >> long_start >> lat_end >> long_end >> a_star;

    // Read nodes
    std::vector<Node *> nodes;
    std::tuple<int, int> values = readFile(nodes, "../../data/RJ");

    // Find nearest nodes to points where user clicked
    std::pair<int, float> info_start = findNearestNode(lat_start, long_start, nodes);
    std::pair<int, float> info_end = findNearestNode(lat_end, long_end, nodes);

    // Ids
    int id_start = info_start.first, id_end = info_end.first;

    // If click point is > 200m away from nearest node, return invalid path alert
    if ((info_start.second > 200) | (info_end.second > 200))
    {
        const char *notValid = "Caminho não é válido!";

        response = MHD_create_response_from_buffer(strlen(notValid),
                                                   (void *)notValid, MHD_RESPMEM_PERSISTENT);
    }
    else
    {
        // Measure time of shortest path
        auto start = std::chrono::high_resolution_clock::now();
        std::vector<int> path = shortestPath(nodes[id_start], nodes[id_end], nodes, a_star);
        auto stop = std::chrono::high_resolution_clock::now();

        // Final total time in milliseconds
        auto time = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

        // Writes json file
        rebuildJson((float)time.count() / 1000, path, nodes);

        // Read json file
        int fd;
        struct stat sbuf;

        fd = open("data.json", O_RDONLY);
        fstat(fd, &sbuf);

        // Method for responses that may be too big
        response =
            MHD_create_response_from_fd_at_offset64(sbuf.st_size, fd, 0);
    }
    return response;
}

MHD_Result answer_to_connection(void *cls, struct MHD_Connection *connection,
                                const char *url,
                                const char *method, const char *version,
                                const char *upload_data,
                                size_t *upload_data_size, void **con_cls)
{
    std::string _url = url;

    std::cout << _url << std::endl;

    MHD_Result ret;
    struct MHD_Response *response;

    FILE *f;
    long size;
    char *page;

    if (_url == "/" | _url == "/index.js")
    {
        std::string ext, mime_type;
        if (_url == "/")
        {
            // Root page
            ext = ".html", mime_type = "text/html";
        }

        else
        {
            // Javascript
            ext = ".js", mime_type = "application/javascript";
        }

        // Read html base page
        std::string myPath = "../front-end/index" + ext;
        const char *page = readTextFile(myPath.c_str());

        // Create response
        response = MHD_create_response_from_buffer(strlen(page),
                                                   (void *)page, MHD_RESPMEM_PERSISTENT);
        // Add header with data type info
        MHD_add_response_header(response, "Content-Type", mime_type.c_str());
    }

    else if (_url.compare(0, 5, "/api/") == 0)
    {
        // Return json to client
        response = json(_url, connection);

        MHD_add_response_header(response, "Content-Type", "application/json");
    }

    if (!response)
    {
        // Error page
        const char *error = "<html>Endereço não encontrado</html>";

        // Create response
        response = MHD_create_response_from_buffer(strlen(page),
                                                   (void *)page, MHD_RESPMEM_PERSISTENT);
        // Add header with data type info
        MHD_add_response_header(response, "Content-Type", "text/html");
    }

    // Queue it
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);

    // Destroy response
    MHD_destroy_response(response);

    return ret;
}

int main()
{
    struct MHD_Daemon *daemon;

    daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, PORT, NULL, NULL,
                              &answer_to_connection, NULL, MHD_OPTION_END);
    if (NULL == daemon)
        return 1;

    getchar();

    MHD_stop_daemon(daemon);
    return 0;
}