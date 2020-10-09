#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERROR 1
#define SUCCESS 0
#define GROUP_SIZE 4

#define GET_KERNEL_INFO(info) struct KernelInfo *kernel_info = \
                                      (struct KernelInfo *) info

#define GET_META_INFO(info) struct MetaInfo *meta_info = \
                                    (struct MetaInfo *) info

// statically allocated to avoid calling
char buffer[16][BUFFER_SIZE];
static struct KernelInfo kernel_values[MAX_NUM_PARSER];
static int kernel_info_index = 0;
static struct MetaInfo meta_values[MAX_NUM_PARSER];
static int meta_info_index = 0;

int parse_placement_(char *filename, int *num_inputs, int *inputs,
                     int *num_outputs, int *outputs, int *num_groups,
                     int *reset) {
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int max_x = 0;

    *num_inputs = 0;
    *num_outputs = 0;
    // for now we assume reset is always the fist one
    // since it's set during the pre-fix IO placement
    *reset = 0;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        return ERROR;
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        if (read == 0) continue;
        if (line[0] == '-' || line[0] == 'B') continue;

        // we parse one char at a time
        int idx = 0, buf_index = 0;
        char c;
        int count = 0;
        do {
            c = line[idx];
            if (c == ' ' || c == '\t' || c == '\n') {
                // this is one token
                if (count > 0) {
                    buffer[buf_index][count] = '\0';
                    buf_index++;
                    count = 0;
                }
            } else {
                buffer[buf_index][count] = c;
                count++;
            }
            idx++;
        } while (c != EOF && c != '\n' && idx < read);
        if (buf_index < 4) continue;
        char *s_x = buffer[1];
        char *s_y = buffer[2];
        int x = atoi(s_x);// NOLINT
        int y = atoi(s_y);// NOLINT

        if (x > max_x) max_x = x;

        char *id = buffer[3];
        char *name = buffer[0];
        if (id[1] == 'I') {
            // this is a data port
            if (strstr(name, "out") != NULL) {
                // it's output
                outputs[(*num_outputs) * 2] = x;
                outputs[(*num_outputs) * 2 + 1] = y;
                *num_outputs = *num_outputs + 1;
            } else {
                // it's input
                inputs[(*num_inputs) * 2] = x;
                inputs[(*num_inputs) * 2 + 1] = y;
                *num_inputs = *num_inputs + 1;
            }
        }
    }

    *num_groups = 0;
    while (max_x > 0) {
        *num_groups = *num_groups + 1;
        max_x -= GROUP_SIZE;
    }


    // clean up
    fclose(fp);
    if (line) free(line);
    return SUCCESS;
}

void *parse_placement(char *filename) {
    // TODO: use arena-based allocator?
    if (kernel_info_index >= MAX_NUM_PARSER) return NULL;
    struct KernelInfo *info = &kernel_values[kernel_info_index++];

    parse_placement_(filename, &info->num_inputs, info->inputs, &info->num_outputs, info->outputs,
                     &info->num_groups, &info->reset_port);

    return info;
}

void *parse_metadata(char *filename) {
    if (meta_info_index >= MAX_NUM_PARSER) return NULL;
    struct MetaInfo *info = &meta_values[meta_info_index++];

    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        return NULL;
    }

    int num_inputs = 0;
    int num_outputs = 0;

    while ((read = getline(&line, &len, fp)) != -1) {
        int idx = 0, buf_index = 0;
        char c;
        int count = 0;
        do {
            c = line[idx];
            if (c == '=' || c == '\n') {
                buffer[buf_index][count] = '\0';
                buf_index++;
                count = 0;
            } else {
                buffer[buf_index][count] = c;
                count++;
            }

            idx++;
        } while (c != '\n' && c != EOF && idx < read);
        if (buf_index != 2)
            return NULL;

        // decode the information
        if (strncmp(buffer[0], "placement", strlen("placement")) == 0) {
            // this is placement file
            strncpy(info->placement_filename, buffer[1],
                    strnlen(buffer[1], BUFFER_SIZE));
        } else if (strncmp(buffer[0], "bitstream", strlen("bitstream")) == 0) {
            strncpy(info->bitstream_filename, buffer[1],
                    strnlen(buffer[1], BUFFER_SIZE));
        } else if (strncmp(buffer[0], "input", strlen("input")) == 0) {
            strncpy(info->input_filenames[num_inputs++], buffer[1],
                    strnlen(buffer[1], BUFFER_SIZE));
        } else if (strncmp(buffer[0], "output", strlen("output")) == 0) {
            strncpy(info->output_filenames[num_outputs++], buffer[1],
                    strnlen(buffer[1], BUFFER_SIZE));
        }
    }

    // free up
    fclose(fp);
    if (line) free(line);

    // compute the input and output size
    if (info->input_filenames[0][0] != '\0') {
        fp = fopen(info->input_filenames[0], "r");
        if (fp) {
            fseek(fp, 0L, SEEK_END);
            info->input_size = (int) ftell(fp);
            fclose(fp);
        }
    }
    if (info->output_filenames[0][0] != '\0') {
        fp = fopen(info->output_filenames[0], "r");
        if (fp) {
            fseek(fp, 0L, SEEK_END);
            info->output_size = (int) ftell(fp);
            fclose(fp);
        }
    }

    return info;
}

int get_num_groups(void *info) {
    GET_KERNEL_INFO(info);
    return kernel_info->num_groups;
}
int get_num_inputs(void *info) {
    GET_KERNEL_INFO(info);
    return kernel_info->num_inputs;
}

int get_num_outputs(void *info) {
    GET_KERNEL_INFO(info);
    return kernel_info->num_outputs;
}

int get_input_x(void *info, int index) {
    GET_KERNEL_INFO(info);
    if (index >= kernel_info->num_inputs) {
        return -1;
    } else {
        return kernel_info->inputs[index * 2];
    }
}

int get_input_y(void *info, int index) {
    GET_KERNEL_INFO(info);
    if (index >= kernel_info->num_inputs) {
        return -1;
    } else {
        return kernel_info->inputs[index * 2 + 1];
    }
}

int get_output_x(void *info, int index) {
    GET_KERNEL_INFO(info);
    if (index >= kernel_info->num_outputs) {
        return -1;
    } else {
        return kernel_info->outputs[index * 2];
    }
}

int get_output_y(void *info, int index) {
    GET_KERNEL_INFO(info);
    if (index >= kernel_info->num_outputs) {
        return -1;
    } else {
        return kernel_info->outputs[index * 2 + 1];
    }
}

int get_reset_index(void *info) {
    GET_KERNEL_INFO(info);
    return kernel_info->reset_port;
}

char *get_placement_filename(void *info) {
    GET_META_INFO(info);
    return meta_info->placement_filename;
}

char *get_bitstream_filename(void *info) {
    GET_META_INFO(info);
    return meta_info->bitstream_filename;
}

char *get_input_filename(void *info, int index) {
    GET_META_INFO(info);
    return meta_info->input_filenames[index];
}

char *get_output_filename(void *info, int index) {
    GET_META_INFO(info);
    return meta_info->output_filenames[index];
}

int get_input_size(void *info) {
    GET_META_INFO(info);
    return meta_info->input_size;
}

int get_output_size(void *info) {
    GET_META_INFO(info);
    return meta_info->output_size;
}
