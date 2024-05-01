idf_component_register(SRCS "fota.c" "smartconfig_tasks.c" "mqtt_events.c" "smartconfig_main.c"
                       INCLUDE_DIRS "."
                       EMBED_TXTFILES ${project_dir}/server_certs/ca_cert.pem)
