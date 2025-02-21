idf_component_register(SRCS "src/webserver.c" "src/index_css.c" "src/index_html.c" "src/index_script.c" "src/wifi_page.c" "src/about_page.c" 
                       INCLUDE_DIRS "inc"
                       REQUIRES settings esp_http_server)
